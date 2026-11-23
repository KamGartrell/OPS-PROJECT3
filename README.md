# FAT32 File System Utility

A C-based, shell-like utility for reading, navigating, and modifying FAT32 file system images without corrupting the disk image. It supports mounting an image, inspecting boot sector metadata, navigating directories, creating and deleting files/directories, and performing file I/O.

## Group Members
- **Kameryn Gartrell**: ksg22b@fsu.edu
- **Matthew Chen**: mc23c@fsu.edu
- **Tyler Dussault**: tjd22@fsu.edu

## Division of Labor

### Part 1: Mounting the Image
- **Responsibilities**: Mounted the FAT32 image, parsed the Boot Sector (BPB), built the shell loop and prompt, and implemented `info` and `exit`.
- **Assigned to**: Kameryn Gartrell

### Part 2: Navigation
- **Responsibilities**: Implemented FAT cluster traversal, directory entry parsing, and the `ls` and `cd` commands with path tracking and error handling.
- **Assigned to**: Kameryn Gartrell

### Part 3: Create
- **Responsibilities**: [Description]
- **Assigned to**: Alex Brown

### Part 4: Read
- **Responsibilities**: [Description]
- **Assigned to**: Alex Brown, Jane Smith

### Part 5: Update
- **Responsibilities**: [Description]
- **Assigned to**: Alex Brown, Jane Smith

### Part 6: Delete
- **Responsibilities**: [Description]
- **Assigned to**: Jane Smith

### Extra Credit
- **Responsibilities**: [Description]
- **Assigned to**: Alex Brown

## File Listing
```
filesys/
│
├── src/
│ ├── lexer.c
│ └── fat32.c
│ └── main.c
│
├── include/
│ └── lexer.h
│ └── fat32.h
│
├── README.md
└── Makefile
```
## How to Compile & Execute

### Requirements
- **Compiler**: e.g., `gcc` for C/C++, `rustc` for Rust.
- **Dependencies**: List any libraries or frameworks necessary (rust only).

### Compilation
For a C/C++ example:
```bash
make
```
This will build the executable in ...
### Execution
```bash
make run
```
This will run the program ...

## Bugs
- **Bug 1**: This is bug 1.
- **Bug 2**: This is bug 2.
- **Bug 3**: This is bug 3.

## Extra Credit
- **Extra Credit 1**: [Extra Credit Option]
- **Extra Credit 2**: [Extra Credit Option]
- **Extra Credit 3**: [Extra Credit Option]

## Considerations
[Description]
