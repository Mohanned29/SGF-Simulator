#include "C:\raylib\raylib\src\raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "filesystem.h"
#include "C:\raylib\raylib\src\raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "filesystem.h"

void ShowInitializationScreen();
void ShowMainMenu();

int main(void) {
    InitWindow(800, 600, "File System Simulator");

    SecondaryMemory sm;
    int initialized = 0;
    int total_blocks = 0;
    int block_size = 0;
    int choice = 0;
    while (!WindowShouldClose()) {

        if (!initialized) {
            ShowInitializationScreen(&initialized, &sm, &total_blocks, &block_size);
        } else {
            ShowMainMenu(&sm, &choice);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

// Show initialization screen (Step 1)
void ShowInitializationScreen(int *initialized, SecondaryMemory *sm, int *total_blocks, int *block_size) {
    static char blocks_input[10] = "\0";
    static char size_input[10] = "\0";
    static int active_input = 0;

    DrawText("Initialisation de la mémoire secondaire", 50, 50, 30, BLACK);
    DrawText("Entrez le nombre total de blocs:", 50, 100, 20, BLACK);
    DrawText("Entrez la taille des blocs (en octets):", 50, 160, 20, BLACK);

    DrawRectangle(300, 90, 200, 40, LIGHTGRAY);
    DrawText(blocks_input, 310, 100, 20, DARKGRAY);

    DrawRectangle(300, 150, 200, 40, LIGHTGRAY);
    DrawText(size_input, 310, 160, 20, DARKGRAY);

    if (active_input == 0) {
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(blocks_input) > 0) {
            blocks_input[strlen(blocks_input) - 1] = '\0';
        } else {
            for (int i = 32; i < 128; i++) {
                if (IsKeyPressed(i)) {
                    char str[2] = { (char)i, '\0' };
                    strcat(blocks_input, str);
                }
            }
        }
    }
    if (active_input == 1) {
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(size_input) > 0) {
            size_input[strlen(size_input) - 1] = '\0';
        } else {
            for (int i = 32; i < 128; i++) {
                if (IsKeyPressed(i)) {
                    char str[2] = { (char)i, '\0' };
                    strcat(size_input, str);
                }
            }
        }
    }

    Rectangle submit_button = { 350, 210, 100, 40 };
    Color button_color = DARKBLUE;
    if (CheckCollisionPointRec(GetMousePosition(), submit_button)) {
        button_color = BLUE;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            *total_blocks = atoi(blocks_input);
            *block_size = atoi(size_input);

            if (*total_blocks > 0 && *block_size > 0) {
                initialize_secondary_memory(sm, *total_blocks, *block_size);
                *initialized = 1;

                printf("Mémoire secondaire initialisée avec %d blocs de taille %d.\n", *total_blocks, *block_size);
            }
        }
    }

    DrawRectangleRec(submit_button, button_color);
    DrawText("Soumettre", 375, 220, 20, WHITE);

    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){300, 90, 200, 40}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        active_input = 0;
    } else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){300, 150, 200, 40}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        active_input = 1;
    }
}

// Show the main menu (Step 2)
void ShowMainMenu(SecondaryMemory *sm, int *choice) {
    int option_selected = 0;


    const char *menu_options[] = {
        "Create a file",
        "Display Memory State",
        "Display File Metadata",
        "Search a Record by ID",
        "Insert a New Record",
        "Delete a Record",
        "Defragment a File",
        "Delete a File",
        "Rename a File",
        "Compact Memory",
        "Clear Secondary Memory",
        "Quit"
    };

    DrawText("========== File System Simulator ==========", 50, 50, 20, BLACK);

    for (int i = 0; i < 12; i++) {
        Rectangle button = { 50, 100 + i * 40, 250, 30 };
        if (CheckCollisionPointRec(GetMousePosition(), button)) {
            DrawRectangleRec(button, BLUE);
            DrawText(menu_options[i], 60, 100 + i * 40, 20, WHITE);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                *choice = i + 1;
            }
        } else {
            DrawRectangleRec(button, LIGHTGRAY);
            DrawText(menu_options[i], 60, 100 + i * 40, 20, BLACK);
        }
    }

    switch (*choice) {
        case 1:
            create_file(sm);
            break;
        case 2:
            display_memory_state(sm);
            break;
        case 3:
            display_file_metadata(sm);
            break;
        case 4:
            search_record(sm);
            break;
        case 5:
            insert_record(sm);
            break;
        case 6:
            delete_record(sm);
            break;
        case 7:
            defragment_file(sm);
            break;
        case 8:
            delete_file(sm);
            break;
        case 9:
            rename_file(sm);
            break;
        case 10:
            compact_memory(sm);
            break;
        case 11:
            clear_memory(sm);
            break;
        case 12:
            free_secondary_memory(sm);
            CloseWindow();
            break;
        default:
            printf("Invalid choice. Please try again.\n");
    }
}


// gcc -o main main.c filesystem.c -lraylib -lopengl32 -lm -lgdi32
