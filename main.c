#include "C:\raylib\raylib\src\raylib.h"
#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include "filesystem.h"

// Define application states
typedef enum {
    STATE_INITIALIZATION,
    STATE_MAIN_MENU,
    STATE_CREATE_FILE,
    STATE_SEARCH_RECORD,
    STATE_DISPLAY_MEMORY,
    STATE_EXIT
} AppState;

// Function declarations
void ShowInitializationScreen(int *initialized, SecondaryMemory *sm, int *total_blocks, int *block_size, AppState *state);
void ShowMainMenu(AppState *state);
void ShowCreateFileScreen(AppState *state);
void ShowSearchRecordScreen(AppState *state);
void ShowDisplayMemoryScreen(AppState *state);

// Utility functions
void DrawCenteredText(const char *text, int y, int fontSize, Color color);
void TransitionEffect(Color color, int frames);

int main(void) {
    InitWindow(800, 600, "File System Simulator");
    SetTargetFPS(60);

    SecondaryMemory sm;
    int initialized = 0;
    int total_blocks = 0;
    int block_size = 0;

    AppState state = STATE_INITIALIZATION;

    while (!WindowShouldClose() && state != STATE_EXIT) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (state) {
            case STATE_INITIALIZATION:
                ShowInitializationScreen(&initialized, &sm, &total_blocks, &block_size, &state);
                break;
            case STATE_MAIN_MENU:
                ShowMainMenu(&state);
                break;
            case STATE_CREATE_FILE:
                ShowCreateFileScreen(&state);
                break;
            case STATE_SEARCH_RECORD:
                ShowSearchRecordScreen(&state);
                break;
            case STATE_DISPLAY_MEMORY:
                ShowDisplayMemoryScreen(&state);
                break;
            default:
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

// Centered text utility
void DrawCenteredText(const char *text, int y, int fontSize, Color color) {
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, (GetScreenWidth() - textWidth) / 2, y, fontSize, color);
}

// Screen transition effect
void TransitionEffect(Color color, int frames) {
    for (int i = 0; i < frames; i++) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(color, (float)i / frames));
        EndDrawing();
        BeginDrawing();
    }
}

// Initialization Screen
void ShowInitializationScreen(int *initialized, SecondaryMemory *sm, int *total_blocks, int *block_size, AppState *state) {
    static char blocks_input[10] = "\0";
    static char size_input[10] = "\0";
    static int active_input = 0;

    DrawCenteredText("Initialization Screen", 50, 30, DARKBLUE);
    DrawText("Enter total blocks:", 50, 120, 20, BLACK);
    DrawText("Enter block size (bytes):", 50, 180, 20, BLACK);

    // Input Fields
    DrawRectangle(300, 110, 200, 40, active_input == 0 ? LIGHTGRAY : GRAY);
    DrawText(blocks_input, 310, 120, 20, BLACK);

    DrawRectangle(300, 170, 200, 40, active_input == 1 ? LIGHTGRAY : GRAY);
    DrawText(size_input, 310, 180, 20, BLACK);

    if (active_input == 0 && IsKeyPressed(KEY_BACKSPACE) && strlen(blocks_input) > 0) {
        blocks_input[strlen(blocks_input) - 1] = '\0';
    } else if (active_input == 1 && IsKeyPressed(KEY_BACKSPACE) && strlen(size_input) > 0) {
        size_input[strlen(size_input) - 1] = '\0';
    } else {
        for (int i = 32; i < 128; i++) {
            if (IsKeyPressed(i)) {
                char str[2] = { (char)i, '\0' };
                if (active_input == 0) strcat(blocks_input, str);
                else strcat(size_input, str);
            }
        }
    }

    // Button logic
    Rectangle submit_button = { 300, 250, 200, 50 };
    if (CheckCollisionPointRec(GetMousePosition(), submit_button)) {
        DrawRectangleRec(submit_button, DARKBLUE);
        DrawText("Submit", 370, 265, 20, WHITE);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            *total_blocks = atoi(blocks_input);
            *block_size = atoi(size_input);
            if (*total_blocks > 0 && *block_size > 0) {
                initialize_secondary_memory(sm, *total_blocks, *block_size);
                *initialized = 1;
                *state = STATE_MAIN_MENU;
                TransitionEffect(BLACK, 60); // Transition effect
            }
        }
    } else {
        DrawRectangleRec(submit_button, BLUE);
        DrawText("Submit", 370, 265, 20, WHITE);
    }

    // Switching inputs
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){300, 110, 200, 40}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        active_input = 0;
    } else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){300, 170, 200, 40}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        active_input = 1;
    }
}

// Main Menu
void ShowMainMenu(AppState *state) {
    DrawCenteredText("Main Menu", 50, 30, DARKBLUE);

    const char *options[] = { "Create a File", "Search Record", "Display Memory State", "Exit" };
    for (int i = 0; i < 4; i++) {
        Rectangle button = { 200, 100 + i * 70, 400, 50 };
        if (CheckCollisionPointRec(GetMousePosition(), button)) {
            DrawRectangleRec(button, DARKBLUE);
            DrawText(options[i], 250, 115 + i * 70, 20, WHITE);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                switch (i) {
                    case 0: *state = STATE_CREATE_FILE; break;
                    case 1: *state = STATE_SEARCH_RECORD; break;
                    case 2: *state = STATE_DISPLAY_MEMORY; break;
                    case 3: *state = STATE_EXIT; break;
                }
                TransitionEffect(BLACK, 60);
            }
        } else {
            DrawRectangleRec(button, BLUE);
            DrawText(options[i], 250, 115 + i * 70, 20, WHITE);
        }
    }
}

// Other Screens
void ShowCreateFileScreen(AppState *state) {
    DrawCenteredText("Create a File Screen (Under Development)", 200, 20, BLACK);
    if (IsKeyPressed(KEY_BACKSPACE)) *state = STATE_MAIN_MENU;
}

void ShowSearchRecordScreen(AppState *state) {
    DrawCenteredText("Search Record Screen (Under Development)", 200, 20, BLACK);
    if (IsKeyPressed(KEY_BACKSPACE)) *state = STATE_MAIN_MENU;
}

void ShowDisplayMemoryScreen(AppState *state) {
    DrawCenteredText("Display Memory State (Under Development)", 200, 20, BLACK);
    if (IsKeyPressed(KEY_BACKSPACE)) *state = STATE_MAIN_MENU;
}


// gcc -o main main.c filesystem.c -lraylib -lopengl32 -lm -lgdi32