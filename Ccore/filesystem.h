
/*        DONE BY JINX          */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
    char filename[MAX_FILENAME];
    int record_id;

    File *file_list;
    File *hash_table[HASH_TABLE_SIZE];  // adding hash table for flex
} SecondaryMemory;


void initialize_secondary_memory(SecondaryMemory *sm, int total_blocks, int block_size, char *buffer); 
bool create_file(SecondaryMemory *sm, const char *filename, GlobalOrganization global_org, InternalOrganization internal_org, char *error_msg);
File* find_file(SecondaryMemory *sm, const char *filename, char *buffer);
void display_memory_state(SecondaryMemory *sm, char* buffer); // kayen
void display_file_metadata(SecondaryMemory *sm);
Record* search_record(SecondaryMemory *sm, const char* filename, int record_id, bool* success, char* error_msg);
void insert_record(SecondaryMemory *sm); //mkch
char* delete_record(SecondaryMemory *sm, const char* filename, int record_id, char* buffer);
void defragment_file(SecondaryMemory *sm);
void delete_file(SecondaryMemory *sm);
void rename_file(SecondaryMemory *sm); //mkch
void compact_memory(SecondaryMemory *sm);
void clear_memory(SecondaryMemory *sm);
void free_secondary_memory(SecondaryMemory *sm);
unsigned int hash_function(const char *filename);
void update_memory_allocation(SecondaryMemory *sm, File *file);


#endif
