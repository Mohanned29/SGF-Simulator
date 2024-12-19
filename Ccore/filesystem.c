/*        DONE BY JINX          */

#include "filesystem.h"
#include <io.h>
#include <stdio.h>
#include <stdint.h>

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

unsigned int hash_function(const char *filename) {
    unsigned int hash = 0;
    while (*filename) {
        hash = (hash * 31) + *filename++;
    }
    return hash % HASH_TABLE_SIZE;
}

// recherche hachage for flex
File* find_file(SecondaryMemory *sm, const char *filename) {
    unsigned int index = hash_function(filename);
    File *file = sm->hash_table[index];

    while (file != NULL) {
        if (strcmp(file->metadata.filename, filename) == 0) {
            return file;
        }
        file = file->next;
    }

    file = sm->file_list;
    while (file != NULL) {
        if (strcmp(file->metadata.filename, filename) == 0) {
            return file;
        }
        file = file->next;
    }
    
    return NULL;
}

void initialize_secondary_memory(SecondaryMemory *sm, int total_blocks, int block_size) {
    if (block_size < sizeof(Record)) {
        printf("Error: Block size (%d bytes) is too small to hold a record (%lu bytes).\n", block_size, sizeof(Record));
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

    printf("Secondary memory initialized with %d blocks of size %d.\n", total_blocks, block_size);
}
void create_file(SecondaryMemory *sm, const char *filename, int num_records, int global_org_choice, int internal_org_choice) {
    GlobalOrganization global_org = (global_org_choice == 1) ? CONTIGUOUS : CHAINED;
    InternalOrganization internal_org = (internal_org_choice == 1) ? UNSORTED : SORTED;

    if (find_file(sm, filename) != NULL) {
        printf("File '%s' already exists.\n", filename);
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
        printf("Error: Block size (%d bytes) is too small for a record (%lu bytes).\n", sm->block_size, sizeof(Record));
        free(new_file);
        return;
    }
    int blocks_needed = (num_records + records_per_block - 1) / records_per_block;

    int blocks_allocated = 0;
    int first_block = -1;
    int buffer_blocks_needed = blocks_needed + 1;

    if (global_org == CONTIGUOUS) {
        for (int i = 0; i <= sm->total_blocks - buffer_blocks_needed; i++) {
            int j;
            for (j = 0; j < buffer_blocks_needed; j++) {
                if (sm->allocation_table[i + j] != 0) {
                    break;
                }
            }
            if (j == buffer_blocks_needed) {
                first_block = i;
                for (j = 0; j < buffer_blocks_needed; j++) {
                    sm->allocation_table[i + j] = 1;
                }
                blocks_allocated = buffer_blocks_needed;
                break;
            }
        }
        if (blocks_allocated == 0) {
            printf("Not enough contiguous space available.\n");
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
            printf("Not enough space available.\n");
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
        printf("Error creating file '%s'.\n", filename);
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

    printf("File '%s' created successfully.\n", filename);
}


void display_memory_state(SecondaryMemory *sm) {
    printf("\nMemory State:\n");
    for (int i = 0; i < sm->total_blocks; i++) {
        if (sm->allocation_table[i] == 0) {
            printf("[Block %d: Free] ", i);
        } else {
            printf("[Block %d: Occupied] ", i);
        }
        /*
            heka rahi tthsb :
                records_per_block = block_size / record_size
                records_per_block = 512 / 260 ≈ 1.96 ( heda exemple lokan size of blocks is 512 , 260 = 256 + 4)
        */
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

void search_record(SecondaryMemory *sm) {
    char filename[MAX_FILENAME];
    int record_id;
    File *file;
    Record record;

    printf("Enter file name: ");
    scanf("%s", filename);

    file = find_file(sm, filename);
    if (file == NULL) {
        printf("File '%s' not found.\n", filename);
        return;
    }

    printf("Enter record ID to search: ");
    scanf("%d", &record_id);

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file '%s'.\n", filename);
        return;
    }

    fseek(fp, (record_id - 1) * sizeof(Record), SEEK_SET);
    fread(&record, sizeof(Record), 1, fp);
    fclose(fp);

    printf("Record found: ID = %d, Data = %s\n", record.id, record.data);
}

void insert_record(SecondaryMemory *sm) {
    char filename[MAX_FILENAME];
    printf("Enter the file name to insert into: ");
    scanf("%s", filename);

    File *file = find_file(sm, filename);
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

    int records_per_block = sm->block_size / sizeof(Record);
    int total_blocks_used = (file->metadata.size_in_records + records_per_block - 1) / records_per_block;

    if (total_blocks_used > file->metadata.size_in_blocks) {
        if (file->metadata.global_org == CONTIGUOUS) {
            int next_block = file->metadata.first_block_address + file->metadata.size_in_blocks;
            if (next_block < sm->total_blocks && sm->allocation_table[next_block] == 0) {
                sm->allocation_table[next_block] = 1;
                file->metadata.size_in_blocks++;
            } else {
                printf("No more contiguous space available to expand the file.\n");
                return;
            }
        } else if (file->metadata.global_org == CHAINED) {
            int allocated = 0;
            for (int i = 0; i < sm->total_blocks; i++) {
                if (sm->allocation_table[i] == 0) {
                    sm->allocation_table[i] = 1;
                    file->metadata.size_in_blocks++;
                    allocated = 1;
                    break;
                }
            }
            if (!allocated) {
                printf("No more space available to expand the file.\n");
                return;
            }
        }
    }

    printf("Record inserted successfully into file '%s'.\n", filename);
}


void delete_record(SecondaryMemory *sm) {
    char filename[MAX_FILENAME];
    int record_id;
    printf("Enter the file name: ");
    scanf("%s", filename);
    File *file = find_file(sm, filename);
    if (file == NULL) {
        printf("File '%s' not found.\n", filename);
        return;
    }

    printf("Enter Record ID to delete: ");
    scanf("%d", &record_id);

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Error opening file '%s'.\n", filename);
        return;
    }

    Record *records = (Record *)malloc(sizeof(Record) * file->metadata.size_in_records);
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
            printf("Error writing to file '%s'.\n", filename);
            free(records);
            return;
        }
        fwrite(records, sizeof(Record), count, fp);
        fclose(fp);
        file->metadata.size_in_records--;
        printf("Record with ID %d deleted from file '%s'.\n", record_id, filename);
    } else {
        printf("Record with ID %d not found in file '%s'.\n", record_id, filename);
    }
    free(records);
}

void defragment_file(SecondaryMemory *sm) {
    char filename[MAX_FILENAME];
    printf("Enter the file name to defragment: ");
    scanf("%s", filename);
    File *file = find_file(sm, filename);
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
    File *file = find_file(sm, old_filename);
    if (file == NULL) {
        printf("File '%s' not found.\n", old_filename);
        return;
    }
    printf("Enter the new file name: ");
    scanf("%s", new_filename);

    if (find_file(sm, new_filename) != NULL) {
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
    printf("Memory compaction completed.\n");
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