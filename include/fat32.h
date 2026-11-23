#pragma once

#include "lexer.h"
#include <stdint.h>
#include <stdio.h>

// FAT32 Constants
#define ATTR_DIRECTORY 0x10
#define ATTR_LONG_NAME 0x0F
#define EOC_THRESHOLD  0x0FFFFFF8

// Structures for FAT32 Boot Sector and Directory Entry
typedef struct __attribute__((packed)) {
    uint8_t  BS_jmpBoot[3];
    uint8_t  BS_OEMName[8];
    uint16_t BPB_BytsPerSec;
    uint8_t  BPB_SecPerClus;
    uint16_t BPB_RsvdSecCnt;
    uint8_t  BPB_NumFATs;
    uint16_t BPB_RootEntCnt;
    uint16_t BPB_TotSec16;
    uint8_t  BPB_Media;
    uint16_t BPB_FATSz16;
    uint16_t BPB_SecPerTrk;
    uint16_t BPB_NumHeads;
    uint32_t BPB_HiddSec;
    uint32_t BPB_TotSec32;
    uint32_t BPB_FATSz32;
    uint16_t BPB_ExtFlags;
    uint16_t BPB_FSVer;
    uint32_t BPB_RootClus;
    uint16_t BPB_FSInfo;
    uint16_t BPB_BkBootSec;
    uint8_t  BPB_Reserved[12];
    uint8_t  BS_DrvNum;
    uint8_t  BS_Reserved1;
    uint8_t  BS_BootSig;
    uint32_t BS_VolID;
    uint8_t  BS_VolLab[11];
    uint8_t  BS_FilSysType[8];
} BPB;

// Structure for FAT32 Directory Entry
typedef struct __attribute__((packed)) {
    unsigned char  DIR_Name[11];
    unsigned char  DIR_Attr;
    unsigned char  DIR_NTRes;
    unsigned char  DIR_CrtTimeTenth;
    unsigned short DIR_CrtTime;
    unsigned short DIR_CrtDate;
    unsigned short DIR_LstAccDate;
    unsigned short DIR_FstClusHi;
    unsigned short DIR_WrtTime;
    unsigned short DIR_WrtDate;
    unsigned short DIR_FstClusLo;
    unsigned int   DIR_FileSize;
} DIR;

// Global variables for FAT32 file system state
extern FILE *fp;
extern BPB bpb;
extern char imgName[256];
extern char cwdPath[1024];
extern unsigned int cwdCluster;

// Function declarations for FAT32 operations
int mountFat32(const char *path);
void wrapper(void (*func)(tokenlist *tokens), tokenlist *tokens);

// Helper functions for FAT32 operations
unsigned int clusterToOffset(unsigned int cluster);
unsigned int getNextCluster(unsigned int cluster);
void cleanDirName(const unsigned char raw[11], char out[12]);

// Command functions for FAT32 operations 
void cmdInfo(tokenlist *tokens);
void cmdls(tokenlist *tokens);
void cmdCd(tokenlist *tokens);