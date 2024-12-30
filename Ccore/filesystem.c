/*        DONE BY JINX          */

#include "filesystem.h"
#include <io.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
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


int find_free_block(SecondaryMemory *sm) {
    for (int i = 0; i < sm->total_blocks; i++) {
        if (sm->allocation_table[i] == 0) {
            return i;
        }
    }
    return -1;
}

bool create_file(SecondaryMemory *sm, const char *filename, GlobalOrganization global_org, InternalOrganization internal_org, char *error_msg) {
    if (find_file(sm, filename, error_msg) != NULL) {
        strcpy(error_msg, "File already exists");
        return false;
    }

    File *new_file = (File *)malloc(sizeof(File));
    strcpy(new_file->metadata.filename, filename);
    new_file->metadata.size_in_records = 0;
    new_file->metadata.global_org = global_org;
    new_file->metadata.internal_org = internal_org;
    new_file->next = NULL;

    int first_block = -1;
    if (global_org == CONTIGUOUS) {
        first_block = find_free_block(sm);
        if (first_block != -1) {
            sm->allocation_table[first_block] = 1;
        }
    } else {
        first_block = find_free_block(sm);
        if (first_block != -1) {
            sm->allocation_table[first_block] = 1;
        }
    }

    if (first_block == -1) {
        strcpy(error_msg, "No free blocks available");
        free(new_file);
        return false;
    }

    new_file->metadata.size_in_blocks = 1;
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

    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        strcpy(error_msg, "Error creating file");
        sm->allocation_table[first_block] = 0;
        free(new_file);
        return false;
    }
    fclose(fp);

    return true;
}


void display_memory_state(SecondaryMemory *sm, char* buffer) {
    int offset = 0;
    offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "Memory State:\n");
    
    for (int i = 0; i < sm->total_blocks; i++) {
        offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "[Block %d: %s] ",i, sm->allocation_table[i] == 0 ? "Free" : "Occupied");

        if ((i + 1) % 10 == 0) {
            offset += snprintf(buffer + offset, BUFFER_SIZE - offset, "\n");
        }
    }
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



bool insert_record(SecondaryMemory *sm, const char* filename, int record_id, const char* record_data, char* error_msg) {
    File *file = find_file(sm, filename, error_msg);
    if (file == NULL) {
        strcpy(error_msg, "File not found");
        return false;
    }

    FILE *fp = fopen(filename, "ab");
    if (fp == NULL) {
        strcpy(error_msg, "Error opening file");
        return false;
    }

    Record new_record;
    new_record.id = record_id;
    strncpy(new_record.data, record_data, 255);
    new_record.data[255] = '\0';

    if (fwrite(&new_record, sizeof(Record), 1, fp) != 1) {
        strcpy(error_msg, "Error writing record to file");
        fclose(fp);
        return false;
    }

    fclose(fp);
    file->metadata.size_in_records++;
    update_memory_allocation(sm, file);
    return true;
}

bool delete_record(SecondaryMemory *sm, const char* filename, int record_id, bool is_physical, char* error_msg) {
    File *file = find_file(sm, filename, error_msg);
    if (file == NULL) {
        strcpy(error_msg, "File not found");
        return false;
    }

    FILE *fp = fopen(filename, "rb+");
    if (fp == NULL) {
        strcpy(error_msg, "Error opening file");
        return false;
    }

    if (is_physical) {
        Record *records = malloc(sizeof(Record) * file->metadata.size_in_records);
        int count = 0;
        int found = 0;
        Record temp;

        //read all valid records (not logically deleted)
        while (fread(&temp, sizeof(Record), 1, fp)) {
            if (temp.id == record_id) {
                if (temp.is_deleted) {
                    found = 1;
                } else {
                    strcpy(error_msg, "Record must be logically deleted first");
                    free(records);
                    fclose(fp);
                    return false;
                }
            } else if (!temp.is_deleted) {
                records[count++] = temp;
            }
        }
        
        if (found) {
            rewind(fp);
            fwrite(records, sizeof(Record), count, fp);
            ftruncate(fileno(fp), count * sizeof(Record));
            file->metadata.size_in_records--;
            free(records);
            fclose(fp);
            strcpy(error_msg, "Record physically deleted");
            return true;
        }
        free(records);
    } else {
        Record record;
        while (fread(&record, sizeof(Record), 1, fp)) {
            if (record.id == record_id && !record.is_deleted) {
                record.is_deleted = true;
                fseek(fp, -(long)sizeof(Record), SEEK_CUR);
                fwrite(&record, sizeof(Record), 1, fp);
                fclose(fp);
                strcpy(error_msg, "Record logically deleted");
                return true;
            }
        }
    }
    
    fclose(fp);
    strcpy(error_msg, "Record not found");
    return false;
}




bool defragment_file(SecondaryMemory *sm, const char* filename, char* error_msg) {
    File *file = find_file(sm, filename, error_msg);
    if (file == NULL) {
        strcpy(error_msg, "File not found");
        return false;
    }

    if (file->metadata.global_org != CHAINED) {
        strcpy(error_msg, "Only chained files need defragmentation");
        return false;
    }

    int blocks_needed = file->metadata.size_in_blocks;
    int new_start_block = -1;
    
    for (int i = 0; i <= sm->total_blocks - blocks_needed; i++) {
        bool space_available = true;
        for (int j = 0; j < blocks_needed; j++) {
            if (sm->allocation_table[i + j] != 0) {
                space_available = false;
                i = i + j;
                break;
            }
        }
        if (space_available) {
            new_start_block = i;
            break;
        }
    }

    if (new_start_block == -1) {
        strcpy(error_msg, "No contiguous space available for defragmentation");
        return false;
    }

    for (int i = 0; i < sm->total_blocks; i++) {
        if (sm->allocation_table[i] == 1) {
            sm->allocation_table[i] = 0;
        }
    }

    for (int i = 0; i < blocks_needed; i++) {
        sm->allocation_table[new_start_block + i] = 1;
    }

    file->metadata.first_block_address = new_start_block;
    file->metadata.global_org = CONTIGUOUS;

    return true;
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