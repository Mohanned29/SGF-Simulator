# File System Simulator

[🎥 Watch the Demo Video](https://github.com/Mohanned29/SGF-Simulator/sfsd.mp4)


This project is a **File System Simulator**, implemented in **C** with a graphical interface using **Raylib**. It manages file operations such as **creating files, inserting records, searching, deleting, defragmenting, and compacting memory**. The system simulates secondary storage with a **hash-based allocation mechanism**.

## Features

- **File Management**  
  - Create, rename, delete files.
  - Display file metadata.
  - Defragment files for better memory management.

- **Record Management**  
  - Insert new records.
  - Search records by unique ID (matricule).
  - Logical and physical deletion of records.

- **Memory Management**  
  - Simulates secondary memory using hash tables and an allocation table.
  - Compact and clear secondary memory.


## Compilation & Execution

### Prerequisites

- **C Compiler** (GCC or Clang)
- **Raylib** (For GUI elements)
- **Make** (Optional, for easy compilation)

### Compilation

To compile the project, use:

```bash
gcc -o filesystem main.c filesystem.c -I/path/to/raylib/include -L/path/to/raylib/lib -lraylib -lm -ldl -lpthread -lGL -lX11

