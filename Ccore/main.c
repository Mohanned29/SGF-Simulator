#include "C:\raylib\raylib\src\raylib.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "filesystem.h"
#define BUFFER_SIZE 512

typedef enum {
    STATE_INITIALIZATION,
    STATE_MAIN_MENU,
    STATE_CREATE_FILE,
    STATE_SEARCH_RECORD,
    STATE_DISPLAY_MEMORY,
    STATE_EXIT,
    STATE_DISPLAY_FILE_METADATA,
    STATE_INSERT_NEW_RECORD,
    STATE_DELETE_RECORD,
    STATE_DEFRAGMENT_FILE,
    STATE_DELETE_FILE,
    STATE_RENAME_FILE,
    STATE_COMPACT_MEMORY,
    STATE_CLEAR_SECONDARY_MEMORY
} AppState;


#define MAX_INPUT_SIZE 128
void ShowInitializationScreen(int *initialized, SecondaryMemory *sm, int *total_blocks, int *block_size, AppState *state);
void ShowMainMenu(AppState *state);
void ShowCreateFileScreen(SecondaryMemory *sm, AppState *state);
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
                ShowCreateFileScreen(&sm,&state);
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

void TransitionWithText(Color bgColor, Color textColor, int frames, const char *text) {
    int frameCount = 0; 
    while (frameCount < frames) {
        BeginDrawing();
        ClearBackground(bgColor); 

        
        float alpha = (float)frameCount / frames;
        if (alpha > 0.5f) alpha = 1.0f - alpha; 
        alpha *= 2.0f; 
        
        
        Color blendedTextColor = (Color){
            textColor.r, textColor.g, textColor.b, (unsigned char)(255 * alpha)};
        
        
        DrawText(text, GetScreenWidth() / 2 - MeasureText(text, 20) / 2, 
                 GetScreenHeight() / 2, 40, blendedTextColor);

        EndDrawing();

        frameCount++;
    }
}

void TransitionEffect(Color color, int frames) {
    for (int i = 0; i < frames; i++) {
        
        float progress = (float)i / (frames - 1);

       
        ClearBackground(RAYWHITE); 
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(color, progress));

        
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

       
        int progressBarWidth = screenWidth / 2; 
        int progressBarHeight = 40;             
        int progressBarX = (screenWidth - progressBarWidth) / 2;
        int progressBarY = (screenHeight - progressBarHeight) / 2; 

        
        DrawRectangle(progressBarX, progressBarY, progressBarWidth, progressBarHeight, LIGHTGRAY);

        
        int progressWidth = progressBarWidth * progress;
        DrawRectangle(progressBarX, progressBarY, progressWidth, progressBarHeight, BLUE);

        
        char progressText[16];
        snprintf(progressText, sizeof(progressText), "%d%%", (int)(progress * 100));
        DrawText(progressText, progressBarX + progressBarWidth / 2 - MeasureText(progressText, 20) / 2, 
                 progressBarY - 30, 20, BLACK);

        
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

    static char error_message[100] = "\0";
    DrawCenteredText("Initialization Screen", 200, 50, DARKBLUE);

    DrawText("Enter total blocks:", 450, 300, 20, BLACK);
    DrawText("Enter block size (bytes):", 450, 390, 20, BLACK);

    DrawRectangle(450, 310 + padding, inputWidth, inputHeight, LIGHTGRAY);
    DrawText(blocks_input, 460, 320 + padding, 20, BLACK);

    DrawRectangle(450, 400 + padding, inputWidth, inputHeight, LIGHTGRAY);
    DrawText(size_input, 460, 410 + padding, 20, BLACK);

    // Handle input
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

            if (*total_blocks <= 0 || *block_size <= 0) {
                snprintf(error_message, sizeof(error_message), "Error: Both values must be positive integers.");
            } else if (*block_size < sizeof(Record)) {
                snprintf(error_message, sizeof(error_message), "Error: Block size (%d) too small for a record (%lu).", *block_size, sizeof(Record));
            } else {
                char *buffer = (char *)malloc(*total_blocks * *block_size);
                //buffer 
                initialize_secondary_memory(sm, *total_blocks, *block_size, buffer);
                
                *initialized = 1;
                *state = STATE_MAIN_MENU;
                TransitionEffect(WHITE, 60);
            }
        }
    } else {
        DrawRectangleRec(submit_button, BLUE);
        DrawText("Submit", 560, 480 + padding, 20, WHITE);
    }


    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){450, 310 + padding, inputWidth, inputHeight}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        active_input = 0;
    } else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){450, 400 + padding, inputWidth, inputHeight}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        active_input = 1;
    }

    if (strlen(error_message) > 0) {
        DrawText(error_message, 450, 550 + padding, 20, RED);
    }
}

void ShowMainMenu(AppState *state) {
    DrawCenteredText("Main Menu", 50, 40, DARKBLUE);

   
    const char *options[] = {
        "Create File",
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
        "Exit"
    };

    int totalOptions = sizeof(options) / sizeof(options[0]);

   
    int buttonWidth = 400;
    int buttonHeight = 40; 
    int spacing = 10;      
    int totalHeight = totalOptions * buttonHeight + (totalOptions - 1) * spacing;
    int startY = (GetScreenHeight() - totalHeight) / 2;

    for (int i = 0; i < totalOptions; i++) {
        Rectangle button = { (1200 - buttonWidth) / 2, startY + i * (buttonHeight + spacing), buttonWidth, buttonHeight };
        if (CheckCollisionPointRec(GetMousePosition(), button)) {
            DrawRectangleRec(button, DARKBLUE);
            DrawText(options[i], (1200 - MeasureText(options[i], 20)) / 2, startY + i * (buttonHeight + spacing) + 10, 20, WHITE);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                switch (i) {
                    case 0: *state = STATE_CREATE_FILE; break;
                    case 1: *state = STATE_DISPLAY_MEMORY; break;
                    case 2: *state = STATE_DISPLAY_FILE_METADATA; break;
                    case 3: *state = STATE_SEARCH_RECORD; break;
                    case 4: *state = STATE_INSERT_NEW_RECORD; break;
                    case 5: *state = STATE_DELETE_RECORD; break;
                    case 6: *state = STATE_DEFRAGMENT_FILE; break;
                    case 7: *state = STATE_DELETE_FILE; break;
                    case 8: *state = STATE_RENAME_FILE; break;
                    case 9: *state = STATE_COMPACT_MEMORY; break;
                    case 10: *state = STATE_CLEAR_SECONDARY_MEMORY; break;
                    case 11: *state = STATE_EXIT; break;
                    
                }
                TransitionEffect(WHITE, 60);
            }
        } else {
            DrawRectangleRec(button, BLUE);
            DrawText(options[i], (1200 - MeasureText(options[i], 20)) / 2, startY + i * (buttonHeight + spacing) + 10, 20, WHITE);
        }
    }
}

void ShowCreateFileScreen(SecondaryMemory *sm, AppState *state) {
    static char filename[MAX_FILENAME] = "";
    static char num_records_input[MAX_INPUT_SIZE] = "";
    static int active_input = 0; 

    static char buffer[BUFFER_SIZE] = {0};

    Color defaultColor = BLUE;
    Color selectedColor = DARKBLUE;
    static int global_org_choice = 1; 
    static int internal_org_choice = 1; 

    bool showSuccess = false;
    bool showError = false;

    ClearBackground(RAYWHITE);
    DrawText("Create New File", 450, 50, 30, BLACK);

  
    DrawText("File Name:", 300, 150, 20, BLACK);
    Rectangle filenameField = {300, 180, 300, 30};
    DrawRectangleRec(filenameField, (active_input == 1) ? LIGHTGRAY : LIGHTGRAY);
    DrawText(filename, 305, 185, 20, BLACK);

   
    DrawText("Number of Records:", 300, 230, 20, BLACK);
    Rectangle numRecordsField = {300, 260, 300, 30};
    DrawRectangleRec(numRecordsField, (active_input == 2) ? LIGHTGRAY : LIGHTGRAY);
    DrawText(num_records_input, 305, 265, 20, BLACK);

    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        if (CheckCollisionPointRec(mousePos, filenameField)) {
            active_input = 1;
        } else if (CheckCollisionPointRec(mousePos, numRecordsField)) {
            active_input = 2;
        } else {
            active_input = 0;
        }
    }

    if (active_input > 0) {
        int key = GetCharPressed();
        while (key > 0) { 
            if (key >= 32 && key <= 126) {
                if (active_input == 1 && strlen(filename) < MAX_FILENAME - 1) {
                    int len = strlen(filename);
                    filename[len] = (char)key;
                    filename[len + 1] = '\0';
                } else if (active_input == 2 && strlen(num_records_input) < MAX_INPUT_SIZE - 1) {
                    int len = strlen(num_records_input);
                    num_records_input[len] = (char)key;
                    num_records_input[len + 1] = '\0';
                }
            }
            key = GetCharPressed(); 
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (active_input == 1 && strlen(filename) > 0) {
                filename[strlen(filename) - 1] = '\0';
            } else if (active_input == 2 && strlen(num_records_input) > 0) {
                num_records_input[strlen(num_records_input) - 1] = '\0';
            }
        }
    }

  
    DrawText("Select Global Organization Mode:", 300, 310, 20, BLACK);
    Rectangle contButton = {300, 340, 140, 30};
    Rectangle chainButton = {450, 340, 140, 30};

    if (global_org_choice == 1) {
        DrawRectangleRec(contButton, selectedColor);
    } else {
        DrawRectangleRec(contButton, defaultColor);
    }
    DrawText("Contiguous", 315, 345, 20, WHITE);

    if (global_org_choice == 2) {
        DrawRectangleRec(chainButton, selectedColor);
    } else {
        DrawRectangleRec(chainButton, defaultColor);
    }
    DrawText("Chained", 465, 345, 20, WHITE);

  
    DrawText("Select Internal Organization Mode:", 300, 390, 20, BLACK);
    Rectangle unsortedButton = {300, 420, 140, 30};
    Rectangle sortedButton = {450, 420, 140, 30};

    if (internal_org_choice == 1) {
        DrawRectangleRec(unsortedButton, selectedColor);
    } else {
        DrawRectangleRec(unsortedButton, defaultColor);
    }
    DrawText("Unsorted", 315, 425, 20, WHITE);

    if (internal_org_choice == 2) {
        DrawRectangleRec(sortedButton, selectedColor);
    } else {
        DrawRectangleRec(sortedButton, defaultColor);
    }
    DrawText("Sorted", 465, 425, 20, WHITE);

  
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        if (CheckCollisionPointRec(mousePos, contButton)) {
            global_org_choice = 1;
        }
        if (CheckCollisionPointRec(mousePos, chainButton)) {
            global_org_choice = 2;
        }
        if (CheckCollisionPointRec(mousePos, unsortedButton)) {
            internal_org_choice = 1;
        }
        if (CheckCollisionPointRec(mousePos, sortedButton)) {
            internal_org_choice = 2;
        }
    }

   
    Rectangle submitButton = {350, 480, 100, 40};
    DrawRectangleRec(submitButton, GREEN);
    DrawText("Submit", 375, 490, 20, WHITE);

    if (CheckCollisionPointRec(GetMousePosition(), submitButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (filename[0] != '\0' && num_records_input[0] != '\0' && global_org_choice != 0 && internal_org_choice != 0) {
            create_file(sm, filename, atoi(num_records_input), global_org_choice, internal_org_choice, buffer);
            showSuccess = true;
            showError = false;
        } else {
            showError = true;
            showSuccess = false;
        }
    }


    if (showSuccess) {
        TransitionWithText(WHITE, GREEN, 60, buffer);
    } else if (showError) {
        DrawText("Please fill in all the fields", 450, 550, 20, RED);
    }

    
    if (IsKeyPressed(KEY_SPACE)) {
        *state = STATE_MAIN_MENU;
        TransitionEffect(WHITE, 60);
    }
}



void ShowSearchRecordScreen(AppState *state) {
    DrawCenteredText("Search Record Screen (Under Development)", GetScreenHeight() / 2, 20, BLACK);
     if (IsKeyPressed(KEY_SPACE)){
         *state = STATE_MAIN_MENU;
         TransitionEffect(WHITE, 60);
    } 
}

void ShowDisplayMemoryScreen(AppState *state) {
    DrawCenteredText("Display Memory State (Under Development)", GetScreenHeight() / 2, 20, BLACK);
    if (IsKeyPressed(KEY_SPACE)){
         *state = STATE_MAIN_MENU;
         TransitionEffect(WHITE, 60);
    }     
}
void ShowDisplayFileMetadataScreen(AppState *state) {
    DrawCenteredText("Display File Metadata (Under Development)", GetScreenHeight() / 2, 20, BLACK);
     if (IsKeyPressed(KEY_SPACE)){
         *state = STATE_MAIN_MENU;
         TransitionEffect(WHITE, 60);
    } 
}

void ShowInsertNewRecordScreen(AppState *state) {
    DrawCenteredText("Insert New Record (Under Development)", GetScreenHeight() / 2, 20, BLACK);
    if (IsKeyPressed(KEY_SPACE)){
         *state = STATE_MAIN_MENU;
         TransitionEffect(WHITE, 60);
    } 
}

void ShowDeleteRecordScreen(AppState *state) {
    DrawCenteredText("Delete Record (Under Development)", GetScreenHeight() / 2, 20, BLACK);
     if (IsKeyPressed(KEY_SPACE)){
         *state = STATE_MAIN_MENU;
         TransitionEffect(WHITE, 60);
    } 
}    

void ShowDefragmentFileScreen(AppState *state) {
    DrawCenteredText("Defragment File (Under Development)", GetScreenHeight() / 2, 20, BLACK);
     if (IsKeyPressed(KEY_SPACE)){
         *state = STATE_MAIN_MENU;
         TransitionEffect(WHITE, 60);
    } 
}

void ShowDeleteFileScreen(AppState *state) {
    DrawCenteredText("Delete File (Under Development)", GetScreenHeight() / 2, 20, BLACK);
    if (IsKeyPressed(KEY_SPACE)){
         *state = STATE_MAIN_MENU;
         TransitionEffect(WHITE, 60);
    } 
}

void ShowRenameFileScreen(AppState *state) {
    DrawCenteredText("Rename File (Under Development)", GetScreenHeight() / 2, 20, BLACK);
     if (IsKeyPressed(KEY_SPACE)){
         *state = STATE_MAIN_MENU;
         TransitionEffect(WHITE, 60);
    } }

void ShowCompactMemoryScreen(AppState *state) {
    DrawCenteredText("Compact Memory (Under Development)", GetScreenHeight() / 2, 20, BLACK);
     if (IsKeyPressed(KEY_SPACE)){
         *state = STATE_MAIN_MENU;
         TransitionEffect(WHITE, 60);
    } }

void ShowClearSecondaryMemoryScreen(AppState *state) {
    DrawCenteredText("Clear Secondary Memory (Under Development)", GetScreenHeight() / 2, 20, BLACK);
    if (IsKeyPressed(KEY_SPACE)){
         *state = STATE_MAIN_MENU;
         TransitionEffect(WHITE, 60);
    } 
}