

/*        DONE BY JINX          */


#include "filesystem.h"

int main() {
    SecondaryMemory sm;
    int choice;
    int initialized = 0;

    while (1) {
        printf("\n========== File System Simulator ==========\n");
        printf("1. Initialize Secondary Memory\n");
        printf("2. Create a File\n");
        printf("3. Display Memory State\n");
        printf("4. Display File Metadata\n");
        printf("5. Search a Record by ID\n");
        printf("6. Insert a New Record\n");
        printf("7. Delete a Record\n");
        printf("8. Defragment a File\n");
        printf("9. Delete a File\n");
        printf("10. Rename a File\n");
        printf("11. Compact Memory\n");
        printf("12. Clear Secondary Memory\n");
        printf("13. Quit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice != 1 && !initialized) {
            printf("Please initialize the secondary memory first.\n");
            continue;
        }

        switch (choice) {
           case 1: {
                int total_blocks, block_size;
                printf("Enter total number of blocks: ");
                scanf("%d", &total_blocks);
                do {
                    printf("Enter block size (in bytes): ");
                    scanf("%d", &block_size);
                } while (block_size < 261);

                initialize_secondary_memory(&sm, total_blocks, block_size);
                if (block_size >= sizeof(Record)) {
                    initialized = 1;
                }
                break;
            }
            
            case 2:
                create_file(&sm);
                break;
            case 3:
                display_memory_state(&sm);
                break;
            case 4:
                display_file_metadata(&sm);
                break;
            case 5:
                search_record(&sm);
                break;
            case 6:
                insert_record(&sm);
                break;
            case 7:
                delete_record(&sm);
                break;
            case 8:
                defragment_file(&sm);
                break;
            case 9:
                delete_file(&sm);
                break;
            case 10:
                rename_file(&sm);
                break;
            case 11:
                compact_memory(&sm);
                break;
            case 12:
                clear_memory(&sm);
                break;
            case 13:
                if (initialized) {
                    free_secondary_memory(&sm);
                }
                printf("Exiting program.\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
