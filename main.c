

/*        DONE BY JINX          */

#include "C:\raylib\raylib\src\raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "filesystem.h"

#define MAX_TEXT_LENGTH 32

typedef struct {
    int total_blocks;
    int block_size;
    int choice;
    bool is_typing_total_blocks;
    bool is_typing_block_size;
    char input_text[MAX_TEXT_LENGTH];
} UIState;

void DrawButton(Rectangle rect, const char *text, Color color) {
    DrawRectangleRec(rect, color);
    DrawText(text, rect.x + (rect.width - MeasureText(text, 20)) / 2, rect.y + (rect.height - 20) / 2, 20, WHITE);
}

void DrawInputBox(Rectangle rect, const char *text) {
    DrawRectangleRec(rect, LIGHTGRAY);
    DrawText(text, rect.x + 10, rect.y + 10, 20, BLACK);
}

void DrawTextCentered(const char *text, int yPos) {
    int width = MeasureText(text, 20);
    DrawText(text, (GetScreenWidth() - width) / 2, yPos, 20, BLACK);
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "File System Simulator");

    UIState ui = {0, 0, 0, false, false, ""};
    SecondaryMemory sm;
    int initialized = 0;

    Font font = GetFontDefault();

    while (!WindowShouldClose()) {
        Vector2 mousePosition = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

            if (CheckCollisionPointRec(mousePosition, (Rectangle){100, 50, 200, 50})) {
                ui.choice = 1;
            }

            if (CheckCollisionPointRec(mousePosition, (Rectangle){100, 120, 200, 50})) {
                ui.choice = 2;
            }

            if (CheckCollisionPointRec(mousePosition, (Rectangle){100, 190, 200, 50})) {
                ui.choice = 3;
            }

            if (CheckCollisionPointRec(mousePosition, (Rectangle){100, 260, 200, 50})) {
                ui.choice = 4;
            }

            if (CheckCollisionPointRec(mousePosition, (Rectangle){100, 330, 200, 50})) {
                ui.choice = 5;
            }

            if (CheckCollisionPointRec(mousePosition, (Rectangle){100, 400, 200, 50})) {
                ui.choice = 6;
            }

            if (CheckCollisionPointRec(mousePosition, (Rectangle){100, 470, 200, 50})) {
                ui.choice = 7;
            }

            if (CheckCollisionPointRec(mousePosition, (Rectangle){100, 540, 200, 50})) {
                ui.choice = 8;
            }

            if (CheckCollisionPointRec(mousePosition, (Rectangle){100, 610, 200, 50})) {
                ui.choice = 9;
            }

            if (CheckCollisionPointRec(mousePosition, (Rectangle){100, 680, 200, 50})) {
                ui.choice = 10;
            }

            if (CheckCollisionPointRec(mousePosition, (Rectangle){100, 750, 200, 50})) {
                ui.choice = 11;
            }

            if (CheckCollisionPointRec(mousePosition, (Rectangle){100, 820, 200, 50})) {
                ui.choice = 12;
            }

            if (CheckCollisionPointRec(mousePosition, (Rectangle){100, 890, 200, 50})) {
                ui.choice = 13;
            }
        }

        if (ui.is_typing_total_blocks || ui.is_typing_block_size) {
            for (int key = 32; key <= 126; key++) {
                if (IsKeyPressed(key)) {
                    int len = strlen(ui.input_text);
                    if (len < MAX_TEXT_LENGTH - 1) {
                        ui.input_text[len] = (char)key;
                        ui.input_text[len + 1] = '\0';
                    }
                }
            }
            if (IsKeyPressed(KEY_BACKSPACE)) {
                int len = strlen(ui.input_text);
                if (len > 0) {
                    ui.input_text[len - 1] = '\0';
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTextCentered("File System Simulator", 20);

        DrawButton((Rectangle){100, 50, 200, 50}, "Initialize Memory", BLUE);
        DrawButton((Rectangle){100, 120, 200, 50}, "Create a File", BLUE);
        DrawButton((Rectangle){100, 190, 200, 50}, "Display Memory State", BLUE);
        DrawButton((Rectangle){100, 260, 200, 50}, "Display File Metadata", BLUE);
        DrawButton((Rectangle){100, 330, 200, 50}, "Search Record by ID", BLUE);
        DrawButton((Rectangle){100, 400, 200, 50}, "Insert a Record", BLUE);
        DrawButton((Rectangle){100, 470, 200, 50}, "Delete a Record", BLUE);
        DrawButton((Rectangle){100, 540, 200, 50}, "Defragment a File", BLUE);
        DrawButton((Rectangle){100, 610, 200, 50}, "Delete a File", BLUE);
        DrawButton((Rectangle){100, 680, 200, 50}, "Rename a File", BLUE);
        DrawButton((Rectangle){100, 750, 200, 50}, "Compact Memory", BLUE);
        DrawButton((Rectangle){100, 820, 200, 50}, "Clear Memory", BLUE);
        DrawButton((Rectangle){100, 890, 200, 50}, "Quit", RED);

        switch (ui.choice) {
            case 1: {
                int total_blocks = atoi(ui.input_text);
                int block_size = atoi(ui.input_text);
                if (total_blocks > 0 && block_size > 0) {
                    initialize_secondary_memory(&sm, total_blocks, block_size);
                    initialized = 1;
                } else {
                    DrawTextCentered("Invalid input for total blocks or block size.", 460);
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
                CloseWindow();
                break;
            default:
                DrawTextCentered("Invalid choice. Please try again.", 460);
                break;
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

// gcc -o main main.c filesystem.c -lraylib -lopengl32 -lm -lgdi32