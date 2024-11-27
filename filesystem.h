
/*        DONE BY JINX          */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME 100
#define MAX_FILES 50
#define MAX_RECORDS 1000
#define HASH_TABLE_SIZE 100

typedef enum {
    CONTIGUOUS = 1,
    CHAINED
} GlobalOrganization;

typedef enum {
    UNSORTED = 1,
    SORTED
} InternalOrganization;

typedef struct Record {
    int id; //4 bits
    char data[256];
} Record;

typedef struct FileMetadata {
    char filename[MAX_FILENAME];
    int size_in_blocks;
    int size_in_records;
    int first_block_address;
    GlobalOrganization global_org;
    InternalOrganization internal_org;
} FileMetadata;

typedef struct File {
    FileMetadata metadata;
    struct File *next;
} File;

typedef struct SecondaryMemory {
    int total_blocks;
    int block_size;
    int *allocation_table;
    File *file_list;
    File *hash_table[HASH_TABLE_SIZE];  // adding hash table for flex
} SecondaryMemory;


void initialize_secondary_memory(SecondaryMemory *sm, int total_blocks, int block_size);
void create_file(SecondaryMemory *sm);
void display_memory_state(SecondaryMemory *sm);
void display_file_metadata(SecondaryMemory *sm);
void search_record(SecondaryMemory *sm);
void insert_record(SecondaryMemory *sm);
void delete_record(SecondaryMemory *sm);
void defragment_file(SecondaryMemory *sm);
void delete_file(SecondaryMemory *sm);
void rename_file(SecondaryMemory *sm);
void compact_memory(SecondaryMemory *sm);
void clear_memory(SecondaryMemory *sm);
void free_secondary_memory(SecondaryMemory *sm);

unsigned int hash_function(const char *filename);
File* find_file(SecondaryMemory *sm, const char *filename);

#endif
