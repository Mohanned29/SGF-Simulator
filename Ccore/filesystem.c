/* DONE BY JINX */
#include "filesystem.h"
#include <io.h>
#include <stdio.h>
#include <stdint.h>

//buffer file operations
void buffer_write_record(Record *buffer, int *buffer_index, Record *record, int max_buffer_size) {
    if (*buffer_index < max_buffer_size) {
        buffer[*buffer_index] = *record;
        (*buffer_index)++;
    }
}

void buffer_read_record(Record *buffer, int *buffer_index, Record *record) {
    if (*buffer_index > 0) {
        *record = buffer[*buffer_index - 1];
        (*buffer_index)--;
    }
}

void create_file(SecondaryMemory *sm, const char *filename, int num_records, int global_org_choice, int internal_org_choice) {
    //buffer for file creation
    Record *file_buffer = (Record *)malloc(sizeof(Record) * num_records);
    int buffer_index = 0;

    GlobalOrganization global_org = (global_org_choice == 1) ? CONTIGUOUS : CHAINED;
    InternalOrganization internal_org = (internal_org_choice == 1) ? UNSORTED : SORTED;

    if (find_file(sm, filename) != NULL) {
        printf("File '%s' already exists.\n", filename);
        free(file_buffer);
        return;
    }

    //buffer file metadata creation
    File *new_file = (File *)malloc(sizeof(File));
    strcpy(new_file->metadata.filename, filename);
    new_file->metadata.size_in_records = num_records;
    new_file->metadata.global_org = global_org;
    new_file->metadata.internal_org = internal_org;
    new_file->next = NULL;

    //3mr buffer with sample records
    for (int i = 0; i < num_records; i++) {
        Record record;
        record.id = i + 1;
        sprintf(record.data, "Sample Data %d", i + 1);
        buffer_write_record(file_buffer, &buffer_index, &record, num_records);
    }

    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Error creating file '%s'.\n", filename);
        free(new_file);
        free(file_buffer);
        return;
    }

    fwrite(file_buffer, sizeof(Record), buffer_index, fp);
    fclose(fp);

    free(file_buffer);
    printf("File '%s' created successfully.\n", filename);
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

    // Buffer for reading existing records
    Record *file_buffer = (Record *)malloc(sizeof(Record) * (file->metadata.size_in_records + 1));
    int buffer_index = 0;

    // Read existing records into buffer
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Error opening file '%s'.\n", filename);
        free(file_buffer);
        return;
    }

    // Buffer read of existing records
    while (fread(&file_buffer[buffer_index], sizeof(Record), 1, fp) == 1) {
        buffer_index++;
    }
    fclose(fp);

    // Create new record in buffer
    Record new_record;
    printf("Enter Record ID: ");
    scanf("%d", &new_record.id);
    printf("Enter Record Data: ");
    scanf("%s", new_record.data);

    // Buffer write of new record
    buffer_write_record(file_buffer, &buffer_index, &new_record, file->metadata.size_in_records + 1);

    // Write buffer back to file
    fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Error writing to file '%s'.\n", filename);
        free(file_buffer);
        return;
    }

    // Buffer flush to file
    fwrite(file_buffer, sizeof(Record), buffer_index, fp);
    fclose(fp);

    file->metadata.size_in_records++;
    free(file_buffer);
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

    // Buffer for reading and filtering records
    Record *file_buffer = (Record *)malloc(sizeof(Record) * file->metadata.size_in_records);
    int buffer_index = 0;

    // Read records into buffer, skipping the one to delete
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Error opening file '%s'.\n", filename);
        free(file_buffer);
        return;
    }

    Record temp_record;
    int found = 0;
    while (fread(&temp_record, sizeof(Record), 1, fp) == 1) {
        if (temp_record.id != record_id) {
            file_buffer[buffer_index++] = temp_record;
        } else {
            found = 1;
        }
    }
    fclose(fp);

    if (found) {
        // Buffer flush to file
        fp = fopen(filename, "wb");
        if (fp == NULL) {
            printf("Error writing to file '%s'.\n", filename);
            free(file_buffer);
            return;
        }

        fwrite(file_buffer, sizeof(Record), buffer_index, fp);
        fclose(fp);

        file->metadata.size_in_records--;
        printf("Record with ID %d deleted from file '%s'.\n", record_id, filename);
    } else {
        printf("Record with ID %d not found in file '%s'.\n", record_id, filename);
    }

    free(file_buffer);
}
