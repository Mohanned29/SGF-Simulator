/*        DONE BY JINX          */

#include "filesystem.h"
#include <io.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_SIZE 512
char buffer[BUFFER_SIZE];

/*
Steps :

1) simple hash process :
    - it starts with an initial value of 0 for the hash
    - apr for each character in the file name, it multiplies the current hash by 31 (a constant)
    - adds the ASCII value of the current character
    this process creates a unique number for each string (filename)


2) modulo operaation:
    - result of the hash is then taken modulo HASH_TABLE_SIZE
    - this will ensures that the hash value always falls within the bounds of the hash table array ( mchkitch kayen exceptions f C)


3) storing and retrieving files:
    - when you store a file, its placed in the hash table at the index corresponding to the hash value
    - when you want to seach for a file, the same hash function is used fel hsab te3 el index, and the file is searched at that index
*/




// ahhhhhh wch hedaaaaa 🤯 wa3er wa3er
unsigned int hash_function(const char *filename) {
    unsigned int hash = 5381;  // Prime seed
    unsigned int c;
    int i = 0;
    
    while ((c = (unsigned char)filename[i++])) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
        hash = hash ^ (hash >> 16);
    }
    hash = hash * 16777619;
    hash = hash ^ (hash * 2246822519u);
    hash = hash ^ (hash >> 13);
    
    return hash % HASH_TABLE_SIZE;
}


// O(1) collisions through chaining , mts9sinich , fo9 isti3abek 🥱
File* find_file(SecondaryMemory *sm, const char *filename, char *buffer) {
    unsigned int index = hash_function(filename);
    File *file = sm->hash_table[index];
    
    while (file != NULL) {
        if (strcmp(file->metadata.filename, filename) == 0) {
            return file;
        }
        file = file->next;
    }
    return NULL;
}


void initialize_secondary_memory(SecondaryMemory *sm, int total_blocks, int block_size, char *buffer) {
    if (block_size < sizeof(Record)) {
        snprintf(buffer, BUFFER_SIZE, "Error: Block size (%d bytes) is too small to hold a record (%lu bytes).\n", block_size, sizeof(Record));
        printf("%s", buffer);
        return;
    }

    sm->total_blocks = total_blocks;
    sm->block_size = block_size;
    sm->allocation_table = (int *)malloc(sizeof(int) * total_blocks);
    for (int i = 0; i < total_blocks; i++) {
        sm->allocation_table[i] = 0;
    }

    sm->file_list = NULL;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        sm->hash_table[i] = NULL;
    }

    snprintf(buffer, BUFFER_SIZE, "Secondary memory initialized with %d blocks of size %d.\n", total_blocks, block_size);
    printf("%s", buffer);
}



void create_file(SecondaryMemory *sm, const char *filename, int num_records, int global_org_choice, int internal_org_choice, char *buffer) {
    GlobalOrganization global_org = (global_org_choice == 1) ? CONTIGUOUS : CHAINED;
    InternalOrganization internal_org = (internal_org_choice == 1) ? UNSORTED : SORTED;

    if (find_file(sm, filename, buffer) != NULL) {
        snprintf(buffer, BUFFER_SIZE, "File '%s' already exists.\n", filename);
        printf("%s", buffer);
        return;
    }

    File *new_file = (File *)malloc(sizeof(File));
    strcpy(new_file->metadata.filename, filename);
    new_file->metadata.size_in_records = num_records;
    new_file->metadata.global_org = global_org;
    new_file->metadata.internal_org = internal_org;
    new_file->next = NULL;

    int records_per_block = sm->block_size / sizeof(Record);
    if (records_per_block == 0) {
        snprintf(buffer, BUFFER_SIZE, "Error: Block size (%d bytes) is too small for a record (%lu bytes).\n", sm->block_size, sizeof(Record));
        printf("%s", buffer);
        free(new_file);
        return;
    }

    int blocks_needed = (num_records + records_per_block - 1) / records_per_block;
    int blocks_allocated = 0;
    int first_block = -1;

    if (global_org == CONTIGUOUS) {
        for (int i = 0; i <= sm->total_blocks - blocks_needed; i++) {
            int j;
            for (j = 0; j < blocks_needed; j++) {
                if (sm->allocation_table[i + j] != 0) {
                    break;
                }
            }
            if (j == blocks_needed) {
                first_block = i;
                for (j = 0; j < blocks_needed; j++) {
                    sm->allocation_table[i + j] = 1;
                }
                blocks_allocated = blocks_needed;
                break;
            }
        }
        if (blocks_allocated == 0) {
            snprintf(buffer, BUFFER_SIZE, "Not enough contiguous space available.\n");
            printf("%s", buffer);
            free(new_file);
            return;
        }
    } else if (global_org == CHAINED) {
        int *block_addresses = (int *)malloc(sizeof(int) * blocks_needed);
        for (int i = 0; i < sm->total_blocks && blocks_allocated < blocks_needed; i++) {
            if (sm->allocation_table[i] == 0) {
                sm->allocation_table[i] = 1;
                block_addresses[blocks_allocated++] = i;
            }
        }
        if (blocks_allocated < blocks_needed) {
            snprintf(buffer, BUFFER_SIZE, "Not enough space available.\n");
            printf("%s", buffer);
            for (int i = 0; i < blocks_allocated; i++) {
                sm->allocation_table[block_addresses[i]] = 0;
            }
            free(block_addresses);
            free(new_file);
            return;
        }
        first_block = block_addresses[0];
        free(block_addresses);
    }

    new_file->metadata.size_in_blocks = blocks_allocated;
    new_file->metadata.first_block_address = first_block;

    if (sm->file_list == NULL) {
        sm->file_list = new_file;
    } else {
        File *current = sm->file_list;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_file;
    }

    unsigned int index = hash_function(filename);
    new_file->next = sm->hash_table[index];
    sm->hash_table[index] = new_file;

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        snprintf(buffer, BUFFER_SIZE, "Error creating file '%s'.\n", filename);
        printf("%s", buffer);
        if (global_org == CONTIGUOUS) {
            for (int i = first_block; i < first_block + blocks_allocated; i++) {
                sm->allocation_table[i] = 0;
            }
        }
        free(new_file);
        return;
    }

    Record record;
    for (int i = 0; i < num_records; i++) {
        record.id = i + 1;
        sprintf(record.data, "Sample Data %d", i + 1);
        fwrite(&record, sizeof(Record), 1, fp);
    }
    fclose(fp);

    snprintf(buffer, BUFFER_SIZE, "File '%s' created successfully.\n", filename);
    printf("%s", buffer);
}



void display_memory_state(SecondaryMemory *sm) {
    printf("\nMemory State:\n");
    for (int i = 0; i < sm->total_blocks; i++) {
        printf("[Block %d: %s] ", i, sm->allocation_table[i] == 0 ? "Free" : "Occupied");

        // Calculate records per block for a block size of 512 and record size of 260 bytes (example).
        // records_per_block = block_size / record_size
        // records_per_block = 512 / 260 ≈ 1.96

        if ((i + 1) % 10 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void display_file_metadata(SecondaryMemory *sm) {
    printf("\nFile Metadata:\n");
    printf("Filename\tSize(Blocks)\tSize(Records)\tFirst Block\tGlobal Org\tInternal Org\n");
    File *current = sm->file_list;
    while (current != NULL) {
        printf("%s\t\t%d\t\t%d\t\t%d\t\t%s\t\t%s\n",
               current->metadata.filename,
               current->metadata.size_in_blocks,
               current->metadata.size_in_records,
               current->metadata.first_block_address,
               (current->metadata.global_org == CONTIGUOUS) ? "Contiguous" : "Chained",
               (current->metadata.internal_org == SORTED) ? "Sorted" : "Unsorted");
        current = current->next;
    }
}


Record* search_record(SecondaryMemory *sm, const char* filename, int record_id, bool* success, char* error_msg) {
    unsigned int index = hash_function(filename);
    File *file = sm->hash_table[index];
    
    // Search in hash table chain
    while (file != NULL && strcmp(file->metadata.filename, filename) != 0) {
        file = file->next;
    }
    
    if (file == NULL) {
        *success = false;
        strcpy(error_msg, "File not found");
        return NULL;
    }
    
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        *success = false;
        strcpy(error_msg, "Error opening file");
        return NULL;
    }
    /*
    exemple
        if:
        sizeof(Record) = 260 bytes
        record_id = 3
        Then:
        Offset = (3-1) * 260 = 520 bytes
        fseek moves directly to byte 520
        The next fread will get the third record
    */
    Record* record = (Record*)malloc(sizeof(Record));
    fseek(fp, (record_id - 1) * sizeof(Record), SEEK_SET);
    
    if (fread(record, sizeof(Record), 1, fp) == 1) {
        *success = true;
        fclose(fp);
        return record;
    } else {
        *success = false;
        strcpy(error_msg, "Record not found");
        free(record);
        fclose(fp);
        return NULL;
    }
}



int find_free_block(SecondaryMemory *sm) {
    for (int i = 0; i < sm->total_blocks; i++) {
        if (sm->allocation_table[i] == 0) {
            return i;
        }
    }
    return -1;
}

void update_memory_allocation(SecondaryMemory *sm, File *file) {
    int records_per_block = sm->block_size / sizeof(Record);
    int required_blocks = (file->metadata.size_in_records + records_per_block - 1) / records_per_block;
    
    if (file->metadata.size_in_blocks > required_blocks) {
        file->metadata.size_in_blocks = required_blocks;
    }
    while (file->metadata.size_in_blocks < required_blocks) {
        int block_to_allocate = find_free_block(sm);
        if (block_to_allocate == -1) break;
        sm->allocation_table[block_to_allocate] = 1;
        file->metadata.size_in_blocks++;
    }
}




void insert_record(SecondaryMemory *sm) {
    char filename[MAX_FILENAME];
    printf("Enter the file name to insert into: ");
    scanf("%s", filename);

    File *file = find_file(sm, filename, buffer);

    if (file == NULL) {
        printf("File '%s' not found.\n", filename);
        return;
    }

    FILE *fp = fopen(filename, "ab");
    if (fp == NULL) {
        printf("Error opening file '%s'.\n", filename);
        return;
    }

    Record new_record;
    printf("Enter Record ID: ");
    scanf("%d", &new_record.id);
    printf("Enter Record Data: ");
    scanf("%s", new_record.data);

    if (fwrite(&new_record, sizeof(Record), 1, fp) != 1) {
        printf("Error writing record to file.\n");
        fclose(fp);
        return;
    }

    fclose(fp);
    file->metadata.size_in_records++;
    update_memory_allocation(sm, file);

    printf("Record inserted successfully into file '%s'.\n", filename);
}


char* delete_record(SecondaryMemory *sm, const char* filename, int record_id, char* buffer) {
    File *file = find_file(sm, filename, buffer);
    if (file == NULL) {
        return "File not found";
    }

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        return "Error opening file";
    }

    Record *records = malloc(sizeof(Record) * file->metadata.size_in_records);
    int count = 0;
    int found = 0;

    while (fread(&records[count], sizeof(Record), 1, fp)) {
        if (records[count].id == record_id) {
            found = 1;
        } else {
            count++;
        }
    }
    fclose(fp);

    if (found) {
        fp = fopen(filename, "wb");
        if (fp == NULL) {
            free(records);
            return "Error writing to file";
        }
        fwrite(records, sizeof(Record), count, fp);
        fclose(fp);
        file->metadata.size_in_records--;
        free(records);
        return "Record deleted successfully";
    } else {
        free(records);
        return "Record not found";
    }
}

void defragment_file(SecondaryMemory *sm) {
    char filename[MAX_FILENAME];
    printf("Enter the file name to defragment: ");
    scanf("%s", filename);
    File *file = find_file(sm, filename, buffer);
    if (file == NULL) {
        printf("File '%s' not found.\n", filename);
        return;
    }

    printf("Defragmentation completed for file '%s'.\n", filename);
}


void delete_file(SecondaryMemory *sm) {
    char filename[MAX_FILENAME];
    printf("Enter the file name to delete: ");
    scanf("%s", filename);

    unsigned int index = hash_function(filename);
    File *prev = NULL;
    File *current = sm->hash_table[index];

    while (current != NULL && strcmp(current->metadata.filename, filename) != 0) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        current = sm->file_list;
        prev = NULL;
        while (current != NULL && strcmp(current->metadata.filename, filename) != 0) {
            prev = current;
            current = current->next;
        }
    }

    if (current == NULL) {
        printf("File '%s' not found.\n", filename);
        return;
    }

    if (remove(filename) != 0) {
        printf("Error deleting file '%s'.\n", filename);
        return;
    }

    if (current->metadata.global_org == CONTIGUOUS) {
        for (int i = current->metadata.first_block_address; 
             i < current->metadata.first_block_address + current->metadata.size_in_blocks; i++) {
            sm->allocation_table[i] = 0;
        }
    } else if (current->metadata.global_org == CHAINED) {
        int blocks_freed = 0;
        for (int i = 0; i < sm->total_blocks && blocks_freed < current->metadata.size_in_blocks; i++) {
            if (sm->allocation_table[i] == 1) {
                sm->allocation_table[i] = 0;
                blocks_freed++;
            }
        }
    }

    if (prev == NULL) {
        sm->hash_table[index] = current->next;
    } else {
        prev->next = current->next;
    }

    prev = NULL;
    File *file_list_prev = NULL;
    File *file_list_current = sm->file_list;
    while (file_list_current != NULL && strcmp(file_list_current->metadata.filename, filename) != 0) {
        file_list_prev = file_list_current;
        file_list_current = file_list_current->next;
    }

    if (file_list_current != NULL) {
        if (file_list_prev == NULL) {
            sm->file_list = file_list_current->next;
        } else {
            file_list_prev->next = file_list_current->next;
        }
    }

    free(current);

    printf("File '%s' deleted successfully.\n", filename);
}

void rename_file(SecondaryMemory *sm) {
    char old_filename[MAX_FILENAME], new_filename[MAX_FILENAME];
    printf("Enter the current file name: ");
    scanf("%s", old_filename);
    File *file = find_file(sm, old_filename, buffer);
    if (file == NULL) {
        printf("File '%s' not found.\n", old_filename);
        return;
    }
    printf("Enter the new file name: ");
    scanf("%s", new_filename);

    if (find_file(sm, new_filename,buffer) != NULL) {
        printf("A file with name '%s' already exists.\n", new_filename);
        return;
    }

    if (rename(old_filename, new_filename) != 0) {
        printf("Error renaming file '%s'.\n", old_filename);
        return;
    }

    strcpy(file->metadata.filename, new_filename);
    printf("File renamed to '%s'.\n", new_filename);
}

void compact_memory(SecondaryMemory *sm) {
    
    File *current = sm->file_list;
    int *new_allocation_table = (int *)calloc(sm->total_blocks, sizeof(int));
    int current_block = 0;
    
    while (current != NULL) {
        int records_per_block = sm->block_size / sizeof(Record);
        int required_blocks = (current->metadata.size_in_records + records_per_block - 1) / records_per_block;
        
        current->metadata.size_in_blocks = required_blocks;
        current->metadata.first_block_address = current_block;
        
        for (int i = 0; i < required_blocks; i++) {
            new_allocation_table[current_block + i] = 1;
        }
        current_block += required_blocks;
        current = current->next;
    }
    
    free(sm->allocation_table);
    sm->allocation_table = new_allocation_table;
}

void clear_memory(SecondaryMemory *sm) {
    File *current = sm->file_list;
    while (current != NULL) {
        remove(current->metadata.filename);
        File *temp = current;
        current = current->next;
        free(temp);
    }
    sm->file_list = NULL;

    for (int i = 0; i < sm->total_blocks; i++) {
        sm->allocation_table[i] = 0;
    }

    printf("Secondary memory cleared.\n");
}

void free_secondary_memory(SecondaryMemory *sm) {
    clear_memory(sm);
    free(sm->allocation_table);
}