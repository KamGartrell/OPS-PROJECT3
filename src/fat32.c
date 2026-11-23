#include "fat32.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// Global variables for FAT32 file system state
FILE *fp = NULL;
BPB bpb;
char imgName[256];
char cwdPath[1024];
unsigned int cwdCluster = 0;

// Mounts the FAT32 file system from the specified image file
int mountFat32(const char *path) {
    if ((fp = fopen(path, "r+")) == NULL) {
        return -1;
    }

    fseek(fp, 0, SEEK_SET);
    if (fread(&bpb, sizeof(BPB), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }

    const char *slash = strrchr(path, '/');
    strncpy(imgName, slash ? slash + 1 : path, sizeof(imgName) - 1);
    imgName[sizeof(imgName) - 1] = '\0';

    cwdPath[0] = '\0';
    cwdCluster = bpb.BPB_RootClus;
    return 0;
}

// Wrapper function to save and restore the file pointer position
void wrapper(void (*func)(tokenlist *tokens), tokenlist *tokens) {
    long initPos = ftell(fp);
    func(tokens);
    fseek(fp, initPos, SEEK_SET);
}

// Helper function to convert a cluster number to a byte offset in the image file
unsigned int clusterToOffset(unsigned int cluster) {
    unsigned int first_data_sec = bpb.BPB_RsvdSecCnt + (bpb.BPB_NumFATs * bpb.BPB_FATSz32);
    unsigned int first_sec = first_data_sec + ((cluster - 2) * bpb.BPB_SecPerClus);
    return first_sec * bpb.BPB_BytsPerSec;
}

// Helper function to get the next cluster number from the FAT
unsigned int getNextCluster(unsigned int cluster) {
    unsigned int fat_offset = (bpb.BPB_RsvdSecCnt * bpb.BPB_BytsPerSec) + (cluster * 4);
    unsigned int next = 0;

    fseek(fp, fat_offset, SEEK_SET);
    fread(&next, sizeof(unsigned int), 1, fp);
    return next & 0x0FFFFFFF;
}

// Helper function to clean up the directory name by removing trailing spaces
void cleanDirName(const unsigned char raw[11], char out[12]) {
    int len = 11;
    while (len > 0 && raw[len - 1] == ' ') {
        len--;
    }
    memcpy(out, raw, len);
    out[len] = '\0';
}

// Command function to display information about the FAT32 file system
void cmdInfo(tokenlist *tokens) {
    if (tokens->size != 1) {
        printf("Error: info takes no arguments.\n");
        return;
    }

    unsigned int first_data_sec = bpb.BPB_RsvdSecCnt + (bpb.BPB_NumFATs * bpb.BPB_FATSz32);
    unsigned int data_secs = bpb.BPB_TotSec32 - first_data_sec;
    unsigned int total_clusters = data_secs / bpb.BPB_SecPerClus;
    unsigned int fat_entries = (bpb.BPB_FATSz32 * bpb.BPB_BytsPerSec) / 4;

    fseek(fp, 0, SEEK_END);
    unsigned long img_size = (unsigned long)ftell(fp);

    printf("position of root cluster: %u\n", bpb.BPB_RootClus);
    printf("bytes per sector: %u\n", bpb.BPB_BytsPerSec);
    printf("sectors per cluster: %u\n", bpb.BPB_SecPerClus);
    printf("total # of clusters in data region: %u\n", total_clusters);
    printf("# of entries in one FAT: %u\n", fat_entries);
    printf("size of image (in bytes): %lu\n", img_size);
}

// Command function to list the contents of the current directory
void cmdls(tokenlist *tokens) {
    if (tokens->size != 1) {
        printf("Error: ls takes no arguments.\n");
        return;
    }

    // Start from the current working directory cluster
    unsigned int cluster = cwdCluster;
    unsigned int bytes_per_clus = bpb.BPB_BytsPerSec * bpb.BPB_SecPerClus;
    unsigned int entries_per_clus = bytes_per_clus / sizeof(DIR);
    DIR entry;
    char name[12];

    while (cluster < EOC_THRESHOLD && cluster >= 2) {
        fseek(fp, clusterToOffset(cluster), SEEK_SET);

        for (unsigned int i = 0; i < entries_per_clus; i++) {
            fread(&entry, sizeof(DIR), 1, fp);

            if (entry.DIR_Name[0] == 0x00) {
                return;
            }
            if (entry.DIR_Name[0] == 0xE5 ||
                entry.DIR_Attr == ATTR_LONG_NAME) {
                continue;
            }

            cleanDirName(entry.DIR_Name, name);
            printf("%s\n", name);
        }
        cluster = getNextCluster(cluster);
    }
}

// Helper function to compare strings case-insensitively
static int strEqNocase(const char *a, const char *b) {
    while (*a && *b) {
        if (toupper((unsigned char)*a) != toupper((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == *b;
}

// Changes the current working directory to the specified directory name
void cmdCd(tokenlist *tokens) {
    if (tokens->size != 2) {
        printf("Error: Usage: cd [DIRNAME]\n");
        return;
    }

    char *dirname = tokens->items[1];
    if (strcmp(dirname, ".") == 0) {
        return;
    }

    // Start from the current working directory cluster
    unsigned int cluster = cwdCluster;
    unsigned int bytes_per_clus = bpb.BPB_BytsPerSec * bpb.BPB_SecPerClus;
    unsigned int entries_per_clus = bytes_per_clus / sizeof(DIR);
    DIR entry;
    char name[12];

    // Traverse the directory entries to find the target directory
    while (cluster < EOC_THRESHOLD && cluster >= 2) {
        fseek(fp, clusterToOffset(cluster), SEEK_SET);

        for (unsigned int i = 0; i < entries_per_clus; i++) {
            fread(&entry, sizeof(DIR), 1, fp);

            if (entry.DIR_Name[0] == 0x00) {
                printf("Error: %s does not exist.\n", dirname);
                return;
            }
            if (entry.DIR_Name[0] == 0xE5 ||
                entry.DIR_Attr == ATTR_LONG_NAME) {
                continue;
            }

            cleanDirName(entry.DIR_Name, name);
            if (strEqNocase(name, dirname)) {
                if (!(entry.DIR_Attr & ATTR_DIRECTORY)) {
                    printf("Error: %s is not a directory.\n", dirname);
                    return;
                }

                unsigned int target = entry.DIR_FstClusLo + (65536 * entry.DIR_FstClusHi);
                cwdCluster = (target == 0) ? bpb.BPB_RootClus : target;

                if (strcmp(dirname, "..") == 0) {
                    char *last = strrchr(cwdPath, '/');
                    if (last) {
                        *last = '\0';
                    } else {
                        cwdPath[0] = '\0';
                    }
                } else {
                    if (cwdPath[0] != '\0') {
                        strcat(cwdPath, "/");
                    }
                    strcat(cwdPath, name);
                }
                return;
            }
        }
        cluster = getNextCluster(cluster);
    }

    printf("Error: %s does not exist.\n", dirname);
}