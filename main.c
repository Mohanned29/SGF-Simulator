#include "filesystem.h"
#include "raylib.h"

#define MAX_INPUT 32

int main() {
    // Initialize variables
    SecondaryMemory sm;
    int initialized = 0;
    int total_blocks = 0;
    int block_size = 0;
    char input_text[MAX_INPUT] = "\0";
    int current_selection = 0;

    // Initialize Raylib
    InitWindow(800, 600, "File System Simulator");
    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {
        // Handle input and GUI interactions
        if (IsKeyPressed(KEY_ENTER)) {
            // Handle specific actions based on the button clicked or text input
            switch (current_selection) {
                case 1: {
                    // Initialize Secondary Memory
                    if (total_blocks > 0 && block_size > 0) {
                        initialize_secondary_memory(&sm, total_blocks, block_size);
                        if (block_size >= sizeof(Record)) {
                            initialized = 1;
                        }
                    }
                    break;
                }
                case 2: create_file(&sm); break;
                case 3: display_memory_state(&sm); break;
                case 4: display_file_metadata(&sm); break;
                case 5: search_record(&sm); break;
                case 6: insert_record(&sm); break;
                case 7: delete_record(&sm); break;
                case 8: defragment_file(&sm); break;
                case 9: delete_file(&sm); break;
                case 10: rename_file(&sm); break;
                case 11: compact_memory(&sm); break;
                case 12: clear_memory(&sm); break;
                case 13: {
                    if (initialized) {
                        free_secondary_memory(&sm);
                    }
                    CloseWindow();  // Close the window and exit the program
                    break;
                }
                default:
                    break;
            }
        }

        // Draw everything
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw menu
        DrawText("========== File System Simulator ==========", 250, 20, 20, DARKGRAY);

        // Draw buttons using Raylib's GuiButton
        if (GuiButton((Rectangle){50, 100, 200, 40}, "Initialize Secondary Memory")) {
            current_selection = 1;
        }
        if (GuiButton((Rectangle){50, 150, 200, 40}, "Create a File")) {
            current_selection = 2;
        }
        if (GuiButton((Rectangle){50, 200, 200, 40}, "Display Memory State")) {
            current_selection = 3;
        }
        if (GuiButton((Rectangle){50, 250, 200, 40}, "Display File Metadata")) {
            current_selection = 4;
        }
        if (GuiButton((Rectangle){50, 300, 200, 40}, "Search a Record by ID")) {
            current_selection = 5;
        }
        if (GuiButton((Rectangle){50, 350, 200, 40}, "Insert a New Record")) {
            current_selection = 6;
        }
        if (GuiButton((Rectangle){50, 400, 200, 40}, "Delete a Record")) {
            current_selection = 7;
        }
        if (GuiButton((Rectangle){50, 450, 200, 40}, "Defragment a File")) {
            current_selection = 8;
        }
        if (GuiButton((Rectangle){50, 500, 200, 40}, "Delete a File")) {
            current_selection = 9;
        }
        if (GuiButton((Rectangle){50, 550, 200, 40}, "Rename a File")) {
            current_selection = 10;
        }
        if (GuiButton((Rectangle){300, 100, 200, 40}, "Compact Memory")) {
            current_selection = 11;
        }
        if (GuiButton((Rectangle){300, 150, 200, 40}, "Clear Secondary Memory")) {
            current_selection = 12;
        }
        if (GuiButton((Rectangle){300, 200, 200, 40}, "Quit")) {
            current_selection = 13;
        }

        // Input box for total blocks and block size (only shown when initializing memory)
        if (current_selection == 1 && !initialized) {
            DrawText("Enter total number of blocks:", 300, 250, 20, DARKGRAY);

            // Manual TextBox with Raylib's TextBox function
            Rectangle inputBoxRec = {300, 300, 200, 40};
            DrawRectangleLines(inputBoxRec.x, inputBoxRec.y, inputBoxRec.width, inputBoxRec.height, DARKGRAY);
            DrawText(input_text, inputBoxRec.x + 5, inputBoxRec.y + 10, 20, DARKGRAY);
            if (IsKeyPressed(KEY_ENTER)) {
                total_blocks = atoi(input_text);
            }

            DrawText("Enter block size (in bytes):", 300, 400, 20, DARKGRAY);

            // Manual TextBox with Raylib's TextBox function
            inputBoxRec.y = 450;
            DrawRectangleLines(inputBoxRec.x, inputBoxRec.y, inputBoxRec.width, inputBoxRec.height, DARKGRAY);
            DrawText(input_text, inputBoxRec.x + 5, inputBoxRec.y + 10, 20, DARKGRAY);
            if (IsKeyPressed(KEY_ENTER)) {
                block_size = atoi(input_text);
            }
        }

        // Display current action message
        if (current_selection == 1 && initialized) {
            DrawText("Secondary Memory Initialized!", 300, 550, 20, GREEN);
        }

        // Draw the Raylib interface
        EndDrawing();
    }

    // Close Raylib and clean up
    CloseWindow();

    return 0;
}
