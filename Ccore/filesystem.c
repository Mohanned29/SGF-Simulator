/*        DONE BY JINX          */

#include "filesystem.h"
#include <io.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
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
        strcpy(error_msg, "File already exists.");
        return false;
    }

    File *new_file = (File *)malloc(sizeof(File));
    if (new_file == NULL) {
        strcpy(error_msg, "Memory allocation failed for new file.");
        return false;
    }
    strcpy(new_file->metadata.filename, filename);
    new_file->metadata.size_in_records = 0;
    new_file->metadata.global_org = global_org;
    new_file->metadata.internal_org = internal_org;
    new_file->metadata.next_id = 1;
    new_file->next = NULL;

    int first_block = find_free_block(sm);
    if (first_block != -1) {
        sm->allocation_table[first_block] = 1;
    }

    if (first_block == -1) {
        strcpy(error_msg, "No free blocks available.");
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

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        strcpy(error_msg, "Error creating file.");
        sm->allocation_table[first_block] = 0;
        sm->hash_table[index] = new_file->next;
        if (sm->file_list == new_file) {
            sm->file_list = NULL;
        } else {
            File *prev = sm->file_list;
            while (prev->next != new_file) {
                prev = prev->next;
            }
            prev->next = NULL;
        }
        free(new_file);
        return false;
    }

    fprintf(fp, "ID,Matricule,Name,is_deleted\n");
    fclose(fp);

    strcpy(error_msg, "File created successfully.");
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



extern char buffer[BUFFER_SIZE];
void trim_trailing_whitespace(char *str) {
    if (str == NULL) return;
    int len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[len - 1] = '\0';
        len--;
    }
}

Record* search_record(SecondaryMemory *sm, const char* filename, int matricule, bool* success) {
    if (success) *success = false;

    File *file = find_file(sm, filename, buffer);
    if (file == NULL) {
        strcpy(buffer, "File not found.");
        return NULL;
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        strcpy(buffer, "Error opening file for reading.");
        return NULL;
    }

    Record* record = NULL;
    bool found = false;

    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        strcpy(buffer, "File is empty.");
        fclose(fp);
        return NULL;
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
        int id, mat;
        char name[256];
        char is_deleted_str[10] = {0};
        bool is_deleted = false;

        int num_fields = sscanf(buffer, "%d,%d,%255[^,],%9s", &id, &mat, name, is_deleted_str);
        if (num_fields < 3) {
            continue;
        }

        if (num_fields == 4) {
            trim_trailing_whitespace(is_deleted_str);
            if (strcmp(is_deleted_str, "true") == 0) {
                is_deleted = true;
            }
        }

        if (matricule == mat && !is_deleted) {
            record = (Record*)malloc(sizeof(Record));
            if (record == NULL) {
                strcpy(buffer, "Memory allocation failed.");
                fclose(fp);
                return NULL;
            }
            record->id = id;
            record->matricule = mat;
            strncpy(record->name, name, 255);
            record->name[255] = '\0';
            record->is_deleted = is_deleted;
            found = true;
            break;
        }
    }

    fclose(fp);

    if (found) {
        if (success) *success = true;
        strcpy(buffer, "Record found.");
        return record;
    } else {
        strcpy(buffer, "Record not found.");
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

extern char buffer[BUFFER_SIZE];
bool insert_record(SecondaryMemory *sm, const char* filename, int matricule, const char* name) {
    buffer[0] = '\0';

    File *file = find_file(sm, filename, buffer);
    if (file == NULL) {
        strcat(buffer, "File not found.");
        return false;
    }

    bool success_flag = false;
    Record* existing_record = search_record(sm, filename, matricule, &success_flag);
    if (success_flag && existing_record != NULL) {
        strcat(buffer, "Matricule already exists.");
        free(existing_record);
        return false;
    }
    if (existing_record != NULL) {
        free(existing_record);
    }

    FILE *fp = fopen(filename, "a");
    if (fp == NULL) {
        strcat(buffer, "Error opening file for appending.");
        return false;
    }

    Record new_record;
    new_record.id = file->metadata.next_id++;
    new_record.matricule = matricule;
    strncpy(new_record.name, name, 255);
    new_record.name[255] = '\0';
    new_record.is_deleted = false;

    fprintf(fp, "%d,%d,%s,false\n", new_record.id, new_record.matricule, new_record.name);
    fclose(fp);

    file->metadata.size_in_records++;
    update_memory_allocation(sm, file);

    strcpy(buffer, "Record inserted successfully.");
    return true;
}



extern char buffer[BUFFER_SIZE];
bool delete_record(SecondaryMemory *sm, const char* filename, int matricule, bool is_physical) {
    buffer[0] = '\0';

    File *file = find_file(sm, filename, buffer);
    if (file == NULL) {
        strcat(buffer, "File not found.");
        return false;
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        strcat(buffer, "Error opening file for reading.");
        return false;
    }

    FILE *temp_fp = fopen("temp.csv", "w");
    if (temp_fp == NULL) {
        fclose(fp);
        strcat(buffer, "Error creating temporary file.");
        return false;
    }

    bool found = false;
    bool was_active = false;
    int new_id = 1;

    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strstr(buffer, "is_deleted") == NULL) {
            trim_trailing_whitespace(buffer);
            fprintf(temp_fp, "%s,is_deleted\n", buffer);
        } else {
            fputs(buffer, temp_fp);
        }
    } else {
        fclose(fp);
        fclose(temp_fp);
        remove("temp.csv");
        strcat(buffer, "File is empty.");
        return false;
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
        int id, mat;
        char name[256];
        char is_deleted_str[10] = {0};
        bool is_deleted = false;

        int num_fields = sscanf(buffer, "%d,%d,%255[^,],%9s", &id, &mat, name, is_deleted_str);
        if (num_fields < 3) {
            
            continue;
        }

        if (num_fields == 4) {
            trim_trailing_whitespace(is_deleted_str);
            if (strcmp(is_deleted_str, "true") == 0) {
                is_deleted = true;
            }
        }

        if (matricule == mat) {
            if (!is_deleted) {
                found = true;
                if (is_physical) {
                    was_active = true;
                    continue;
                } else {
                    fprintf(temp_fp, "%d,%d,%s,true\n", id, mat, name);
                    file->metadata.size_in_records--;
                }
            } else {
                if (is_physical) {
                    fprintf(temp_fp, "%d,%d,%s,true\n", new_id++, mat, name);
                } else {
                    fprintf(temp_fp, "%d,%d,%s,true\n", id, mat, name);
                }
            }
        } else {
            if (is_physical) {
                fprintf(temp_fp, "%d,%d,%s,%s\n", new_id++, mat, name, is_deleted ? "true" : "false");
            } else {
                fprintf(temp_fp, "%d,%d,%s,%s\n", id, mat, name, is_deleted ? "true" : "false");
            }
        }
    }

    fclose(fp);
    fclose(temp_fp);

    if (found) {
        if (remove(filename) != 0 || rename("temp.csv", filename) != 0) {
            strcat(buffer, "Error updating the file after deletion.");
            return false;
        }

        if (is_physical) {
            if (was_active) {
                file->metadata.size_in_records--;
            }
            file->metadata.next_id = new_id;
            update_memory_allocation(sm, file);
            strcat(buffer, "Record physically deleted and IDs reassigned successfully.");
        } else {
            update_memory_allocation(sm, file);
            strcat(buffer, "Record logically deleted successfully.");
        }
        return true;
    } else {
        remove("temp.csv");
        strcat(buffer, "Record not found or already deleted.");
        return false;
    }
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