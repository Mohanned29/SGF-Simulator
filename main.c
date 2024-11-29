#include "C:\raylib\raylib\src\raylib.h"
#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "filesystem.h"

typedef enum {
    STATE_INITIALIZATION,
    STATE_MAIN_MENU,
    STATE_CREATE_FILE,
    STATE_SEARCH_RECORD,
    STATE_DISPLAY_MEMORY,
    STATE_EXIT
} AppState;

void ShowInitializationScreen(int *initialized, SecondaryMemory *sm, int *total_blocks, int *block_size, AppState *state);
void ShowMainMenu(AppState *state);
void ShowCreateFileScreen(AppState *state);
void ShowSearchRecordScreen(AppState *state);
void ShowDisplayMemoryScreen(AppState *state);
void DrawCenteredText(const char *text, int y, int fontSize, Color color);
void TransitionEffect(Color color, int frames);

int main(void) {
    InitWindow(1200, 800, "File System Simulator");
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

void DrawCenteredText(const char *text, int y, int fontSize, Color color) {
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, (1200 - textWidth) / 2, y, fontSize, color);
}

void TransitionEffect(Color color, int frames) {
    for (int i = 0; i < frames; i++) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(color, (float)i / frames));
        EndDrawing();
        BeginDrawing();
    }
}

void ShowInitializationScreen(int *initialized, SecondaryMemory *sm, int *total_blocks, int *block_size, AppState *state) {
    static char blocks_input[10] = "\0";
    static char size_input[10] = "\0";
    static int active_input = 0;

    int inputWidth = 300;
    int inputHeight = 50;
    int buttonHeight = 60;
    int padding = 20;

    DrawCenteredText("Initialization Screen", 200, 50, DARKBLUE);

    DrawText("Enter total blocks:", 450, 300, 20, BLACK);
    DrawText("Enter block size (bytes):", 450, 390, 20, BLACK);

    DrawRectangle(450, 310 + padding, inputWidth, inputHeight, LIGHTGRAY);
    DrawText(blocks_input, 460, 320 + padding, 20, BLACK);

    DrawRectangle(450, 400 + padding, inputWidth, inputHeight, LIGHTGRAY);
    DrawText(size_input, 460, 410 + padding, 20, BLACK);

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

    Rectangle submit_button = { 450, 460 + padding, inputWidth, buttonHeight };
    if (CheckCollisionPointRec(GetMousePosition(), submit_button)) {
        DrawRectangleRec(submit_button, DARKBLUE);
        DrawText("Submit", 560, 480 + padding, 20, WHITE);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            *total_blocks = atoi(blocks_input);
            *block_size = atoi(size_input);
            if (*total_blocks > 0 && *block_size > 0) {
                initialize_secondary_memory(sm, *total_blocks, *block_size);
                *initialized = 1;
                *state = STATE_MAIN_MENU;
                TransitionEffect(BLACK, 60);
            }
        }
    } else {
        DrawRectangleRec(submit_button, BLUE);
        DrawText("Submit", 560, 480 + padding, 20, WHITE);
    }

    static float progress = 0.0f;
    if (*initialized == 0 && *total_blocks == 0 && *block_size == 0) {

        progress += 0.01f;
        if (progress > 1.0f) progress = 0.0f;

        int progressBarX = 450;
        int progressBarY = 540 + padding;
        int progressBarWidth = 300;
        int progressBarHeight = 20;

        DrawRectangle(progressBarX, progressBarY, progressBarWidth, progressBarHeight, LIGHTGRAY);

        int progressWidth = progressBarWidth * progress;
        DrawRectangle(progressBarX, progressBarY, progressWidth, progressBarHeight, BLUE);
    }

    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){450, 310 + padding, inputWidth, inputHeight}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        active_input = 0;
    } else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){450, 400 + padding, inputWidth, inputHeight}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        active_input = 1;
    }
}


void ShowMainMenu(AppState *state) {
    DrawCenteredText("Main Menu", 50, 40, DARKBLUE);

    const char *options[] = { "Create a File", "Search Record", "Display Memory State", "Exit" };
    int buttonWidth = 400;
    int buttonHeight = 60;
    int spacing = 20;

    for (int i = 0; i < 4; i++) {
        Rectangle button = { (1200 - buttonWidth) / 2, 100 + i * (buttonHeight + spacing), buttonWidth, buttonHeight };
        if (CheckCollisionPointRec(GetMousePosition(), button)) {
            DrawRectangleRec(button, DARKBLUE);
            DrawText(options[i], (1200 - MeasureText(options[i], 20)) / 2, 115 + i * (buttonHeight + spacing), 20, WHITE);
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
            DrawText(options[i], (1200 - MeasureText(options[i], 20)) / 2, 115 + i * (buttonHeight + spacing), 20, WHITE);
        }
    }
}

void ShowCreateFileScreen(AppState *state) {
    DrawCenteredText("Create a File Screen (Under Development)", GetScreenHeight() / 2, 20, BLACK);
    if (IsKeyPressed(KEY_BACKSPACE)) *state = STATE_MAIN_MENU;
}

void ShowSearchRecordScreen(AppState *state) {
    DrawCenteredText("Search Record Screen (Under Development)", GetScreenHeight() / 2, 20, BLACK);
    if (IsKeyPressed(KEY_BACKSPACE)) *state = STATE_MAIN_MENU;
}

void ShowDisplayMemoryScreen(AppState *state) {
    DrawCenteredText("Display Memory State (Under Development)", GetScreenHeight() / 2, 20, BLACK);
    if (IsKeyPressed(KEY_BACKSPACE)) *state = STATE_MAIN_MENU;
}
