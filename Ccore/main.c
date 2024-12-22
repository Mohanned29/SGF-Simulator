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
void ShowSearchRecordScreen(AppState *state, SecondaryMemory *sm);
void ShowDisplayMemoryScreen(AppState *state, SecondaryMemory *sm);
void ShowInsertNewRecordScreen(AppState *state, SecondaryMemory *sm);
void DrawCenteredText(const char *text, int y, int fontSize, Color color);
void ShowDisplayFileMetadataScreen(AppState *state, SecondaryMemory *sm);
void ShowDeleteRecordScreen(AppState *state, SecondaryMemory *sm);
void ShowDefragmentFileScreen(AppState *state, SecondaryMemory *sm);
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
            case STATE_DISPLAY_FILE_METADATA:
                ShowDisplayFileMetadataScreen(&state, &sm);
                break;
            case STATE_MAIN_MENU:
                ShowMainMenu(&state);
                break;
            case STATE_CREATE_FILE:
                ShowCreateFileScreen(&sm,&state);
                break;
            case STATE_SEARCH_RECORD:
                ShowSearchRecordScreen(&state, &sm);
                break;
            case STATE_DISPLAY_MEMORY:
                ShowDisplayMemoryScreen(&state, &sm);
                break;
            case STATE_INSERT_NEW_RECORD:
                ShowInsertNewRecordScreen(&state, &sm);
                break;
            case STATE_DELETE_RECORD:
                ShowDeleteRecordScreen(&state, &sm);
                break;
            case STATE_DEFRAGMENT_FILE:
                ShowDefragmentFileScreen(&state, &sm);
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

void ShowSearchRecordScreen(AppState *state, SecondaryMemory *sm) {
    static char filename[MAX_FILENAME] = "";
    static char record_id_input[10] = "";
    static int active_input = 0;
    static bool showResult = false;
    static bool showError = false;
    static Record found_record;
    static char message[100] = "";
    static float searchAnimation = 0.0f;
    
    // Title
    DrawCenteredText("Search Record", 50, 40, DARKBLUE);
    
    // Input Fields
    int inputWidth = 300;
    int inputHeight = 40;
    int startY = 150;
    
    // Filename Input
    DrawText("Enter File Name:", 450, startY, 20, BLACK);
    Rectangle filenameBox = {450, startY + 30, inputWidth, inputHeight};
    DrawRectangleRec(filenameBox, (active_input == 1) ? LIGHTGRAY : WHITE);
    DrawRectangleLinesEx(filenameBox, 2, BLUE);
    DrawText(filename, 460, startY + 40, 20, BLACK);
    
    // Record ID Input
    DrawText("Enter Record ID:", 450, startY + 100, 20, BLACK);
    Rectangle idBox = {450, startY + 130, inputWidth, inputHeight};
    DrawRectangleRec(idBox, (active_input == 2) ? LIGHTGRAY : WHITE);
    DrawRectangleLinesEx(idBox, 2, BLUE);
    DrawText(record_id_input, 460, startY + 140, 20, BLACK);
    
    // Handle Input Selection
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), filenameBox)) active_input = 1;
        else if (CheckCollisionPointRec(GetMousePosition(), idBox)) active_input = 2;
        else active_input = 0;
    }
    
    // Handle Text Input
    if (active_input > 0) {
        int key = GetCharPressed();
        char *currentInput = (active_input == 1) ? filename : record_id_input;
        int maxLen = (active_input == 1) ? MAX_FILENAME - 1 : 9;
        
        while (key > 0) {
            if ((key >= 32 && key <= 126) && strlen(currentInput) < maxLen) {
                int len = strlen(currentInput);
                currentInput[len] = (char)key;
                currentInput[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(currentInput) > 0) {
            currentInput[strlen(currentInput) - 1] = '\0';
        }
    }
    
    // Search Button
    Rectangle searchBtn = {450, startY + 200, inputWidth, 50};
    bool btnHovered = CheckCollisionPointRec(GetMousePosition(), searchBtn);
    DrawRectangleRec(searchBtn, btnHovered ? DARKBLUE : BLUE);
    DrawText("Search Record", 520, startY + 215, 20, WHITE);
    
    // Handle Search
    if (btnHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        char buffer[BUFFER_SIZE];
        File *file = find_file(sm, filename, buffer);
        
        if (file == NULL) {
            showError = true;
            showResult = false;
            strcpy(message, "File not found!");
        } else {
            FILE *fp = fopen(filename, "rb");
            if (fp == NULL) {
                showError = true;
                showResult = false;
                strcpy(message, "Error opening file!");
            } else {
                int record_id = atoi(record_id_input);
                fseek(fp, (record_id - 1) * sizeof(Record), SEEK_SET);
                if (fread(&found_record, sizeof(Record), 1, fp) == 1) {
                    showResult = true;
                    showError = false;
                } else {
                    showError = true;
                    showResult = false;
                    strcpy(message, "Record not found!");
                }
                fclose(fp);
            }
        }
        searchAnimation = 0.0f;
    }
    
    // Animate Search Result
    if (showResult || showError) {
        searchAnimation += GetFrameTime() * 2;
        if (searchAnimation > 1.0f) searchAnimation = 1.0f;
        
        float alpha = searchAnimation;
        Rectangle resultBox = {350, startY + 280, 500, 100};
        DrawRectangleRec(resultBox, Fade(LIGHTGRAY, 0.3f));
        DrawRectangleLinesEx(resultBox, 2, BLUE);
        
        if (showResult) {
            char resultText[100];
            snprintf(resultText, sizeof(resultText), "Record ID: %d", found_record.id);
            DrawText(resultText, 370, startY + 300, 20, Fade(BLACK, alpha));
            DrawText(found_record.data, 370, startY + 330, 20, Fade(BLACK, alpha));
        } else {
            DrawText(message, 370, startY + 315, 20, Fade(RED, alpha));
        }
    }
    
    // Return to Main Menu
    if (IsKeyPressed(KEY_SPACE)) {
        *state = STATE_MAIN_MENU;
        TransitionEffect(WHITE, 60);
    }
}


void ShowDisplayMemoryScreen(AppState *state, SecondaryMemory *sm) {
    DrawCenteredText("Memory State", 50, 40, DARKBLUE);
    
    //calculate display parameters
    int blockSize = 50;
    int padding = 10;
    int blocksPerRow = 10;
    int startX = (GetScreenWidth() - (blocksPerRow * (blockSize + padding))) / 2;
    int startY = 150;
    
    //display block information
    for (int i = 0; i < sm->total_blocks; i++) {
        int row = i / blocksPerRow;
        int col = i % blocksPerRow;
        int x = startX + col * (blockSize + padding);
        int y = startY + row * (blockSize + padding);
        
        //drawing block rectangle
        Color blockColor = sm->allocation_table[i] == 0 ? GREEN : RED;
        DrawRectangle(x, y, blockSize, blockSize, blockColor);
        
        //drawing block number
        char blockNum[5];
        snprintf(blockNum, sizeof(blockNum), "%d", i);
        int textWidth = MeasureText(blockNum, 20);
        DrawText(blockNum, x + (blockSize - textWidth)/2, y + blockSize/3, 20, WHITE);
    }
    
    DrawRectangle(startX, startY - 60, 20, 20, GREEN);
    DrawText("Free", startX + 30, startY - 60, 20, BLACK);
    DrawRectangle(startX + 150, startY - 60, 20, 20, RED);
    DrawText("Occupied", startX + 180, startY - 60, 20, BLACK);

    int occupiedBlocks = 0;
    for (int i = 0; i < sm->total_blocks; i++) {
        if (sm->allocation_table[i] == 1) occupiedBlocks++;
    }
    
    char stats[100];
    snprintf(stats, sizeof(stats), "Total Blocks: %d | Occupied: %d | Free: %d", 
             sm->total_blocks, occupiedBlocks, sm->total_blocks - occupiedBlocks);
    DrawText(stats, startX, startY - 100, 20, BLACK);

    DrawText("Press SPACE to return to main menu",
             GetScreenWidth()/2 - MeasureText("Press SPACE to return to main menu", 20)/2, 
             GetScreenHeight() - 50, 20, DARKGRAY);
    
    if (IsKeyPressed(KEY_SPACE)) {
        *state = STATE_MAIN_MENU;
        TransitionEffect(WHITE, 60);
    }
}

void ShowDisplayFileMetadataScreen(AppState *state, SecondaryMemory *sm) {
    DrawCenteredText("File Metadata", 50, 40, DARKBLUE);
    
    const int startY = 120;
    const int rowHeight = 40;
    const int colWidth = 160;
    const int startX = 100;
    
    DrawText("Filename", startX, startY, 20, BLACK);
    DrawText("Size(Blocks)", startX + colWidth, startY, 20, BLACK);
    DrawText("Size(Records)", startX + colWidth * 2, startY, 20, BLACK);
    DrawText("First Block", startX + colWidth * 3, startY, 20, BLACK);
    DrawText("Global Org", startX + colWidth * 4, startY, 20, BLACK);
    DrawText("Internal Org", startX + colWidth * 5, startY, 20, BLACK);

    DrawLine(startX, startY + 30, startX + colWidth * 6 - 60, startY + 30, DARKGRAY);

    File *current = sm->file_list;
    int row = 0;
    
    while (current != NULL && row < 12) {  //limit to prevent overflow (balak hna)
        int yPos = startY + 50 + (row * rowHeight);
        
        DrawText(current->metadata.filename, 
                startX, yPos, 18, DARKGRAY);
        
        char blocks[10], records[10], first_block[10];
        snprintf(blocks, sizeof(blocks), "%d", current->metadata.size_in_blocks);
        snprintf(records, sizeof(records), "%d", current->metadata.size_in_records);
        snprintf(first_block, sizeof(first_block), "%d", current->metadata.first_block_address);
        
        DrawText(blocks, 
                startX + colWidth, yPos, 18, DARKGRAY);
        DrawText(records, 
                startX + colWidth * 2, yPos, 18, DARKGRAY);
        DrawText(first_block, 
                startX + colWidth * 3, yPos, 18, DARKGRAY);
        DrawText(current->metadata.global_org == CONTIGUOUS ? "Contiguous" : "Chained", 
                startX + colWidth * 4, yPos, 18, DARKGRAY);
        DrawText(current->metadata.internal_org == SORTED ? "Sorted" : "Unsorted", 
                startX + colWidth * 5, yPos, 18, DARKGRAY);
        
        current = current->next;
        row++;
    }
    
    DrawRectangleLinesEx((Rectangle){startX - 10, startY - 10, 
                                   colWidth * 6 - 40, 
                                   rowHeight * (row + 1) + 30}, 2, BLUE);
    
    DrawText("Press SPACE to return to main menu", 
             GetScreenWidth()/2 - MeasureText("Press SPACE to return to main menu", 20)/2, 
             GetScreenHeight() - 50, 20, DARKGRAY);
    
    if (IsKeyPressed(KEY_SPACE)) {
        *state = STATE_MAIN_MENU;
        TransitionEffect(WHITE, 60);
    }
}


void ShowInsertNewRecordScreen(AppState *state, SecondaryMemory *sm) {
    static char filename[MAX_FILENAME] = "";
    static char record_id[32] = "";
    static char record_data[256] = "";
    static int active_input = 0;
    static bool showError = false;
    static bool showSuccess = false;
    static char message[256] = "";

    DrawCenteredText("Insert New Record", 50, 40, DARKBLUE);

    int inputWidth = 300;
    int inputHeight = 40;
    int startY = 150;

    DrawText("Enter File Name:", 450, startY, 20, BLACK);
    Rectangle filenameBox = {450, startY + 30, inputWidth, inputHeight};
    DrawRectangleRec(filenameBox, (active_input == 1) ? LIGHTGRAY : WHITE);
    DrawRectangleLinesEx(filenameBox, 2, BLUE);
    DrawText(filename, 460, startY + 40, 20, BLACK);

    DrawText("Enter Record ID:", 450, startY + 100, 20, BLACK);
    Rectangle idBox = {450, startY + 130, inputWidth, inputHeight};
    DrawRectangleRec(idBox, (active_input == 2) ? LIGHTGRAY : WHITE);
    DrawRectangleLinesEx(idBox, 2, BLUE);
    DrawText(record_id, 460, startY + 140, 20, BLACK);

    DrawText("Enter Record Data:", 450, startY + 200, 20, BLACK);
    Rectangle dataBox = {450, startY + 230, inputWidth, inputHeight};
    DrawRectangleRec(dataBox, (active_input == 3) ? LIGHTGRAY : WHITE);
    DrawRectangleLinesEx(dataBox, 2, BLUE);
    DrawText(record_data, 460, startY + 240, 20, BLACK);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), filenameBox)) active_input = 1;
        else if (CheckCollisionPointRec(GetMousePosition(), idBox)) active_input = 2;
        else if (CheckCollisionPointRec(GetMousePosition(), dataBox)) active_input = 3;
        else active_input = 0;
    }

    if (active_input > 0) {
        int key = GetCharPressed();
        char *currentInput = (active_input == 1) ? filename : 
                           (active_input == 2) ? record_id : record_data;
        int maxLen = (active_input == 1) ? MAX_FILENAME - 1 : 
                    (active_input == 2) ? 31 : 255;
        
        while (key > 0) {
            if ((key >= 32 && key <= 126) && strlen(currentInput) < maxLen) {
                int len = strlen(currentInput);
                currentInput[len] = (char)key;
                currentInput[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(currentInput) > 0) {
            currentInput[strlen(currentInput) - 1] = '\0';
        }
    }

    Rectangle submitBtn = {450, startY + 300, inputWidth, 50};
    bool btnHovered = CheckCollisionPointRec(GetMousePosition(), submitBtn);
    DrawRectangleRec(submitBtn, btnHovered ? DARKBLUE : BLUE);
    DrawText("Insert Record", 520, startY + 315, 20, WHITE);

    if (btnHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (strlen(filename) > 0 && strlen(record_id) > 0 && strlen(record_data) > 0) {
            char buffer[BUFFER_SIZE];
            File *file = find_file(sm, filename, buffer);
            
            if (file != NULL) {
                FILE *fp = fopen(filename, "ab");
                if (fp != NULL) {
                    Record new_record;
                    new_record.id = atoi(record_id);
                    strncpy(new_record.data, record_data, 255);
                    new_record.data[255] = '\0';
                    
                    if (fwrite(&new_record, sizeof(Record), 1, fp) == 1) {
                        file->metadata.size_in_records++;
                        update_memory_allocation(sm, file);
                        showSuccess = true;
                        showError = false;
                        strcpy(message, "Record inserted successfully!");

                        filename[0] = '\0';
                        record_id[0] = '\0';
                        record_data[0] = '\0';
                    } else {
                        showError = true;
                        showSuccess = false;
                        strcpy(message, "Error writing record to file.");
                    }
                    fclose(fp);
                } else {
                    showError = true;
                    showSuccess = false;
                    strcpy(message, "Error opening file.");
                }
            } else {
                showError = true;
                showSuccess = false;
                strcpy(message, "File not found.");
            }
        } else {
            showError = true;
            showSuccess = false;
            strcpy(message, "Please fill all fields.");
        }
    }

    if (showSuccess) {
        DrawText(message, 450, startY + 370, 20, GREEN);
    } else if (showError) {
        DrawText(message, 450, startY + 370, 20, RED);
    }
    

    if (IsKeyPressed(KEY_SPACE)) {
        *state = STATE_MAIN_MENU;
        TransitionEffect(WHITE, 60);
    }
}


void ShowDeleteRecordScreen(AppState *state, SecondaryMemory *sm) {
    static char filename[MAX_FILENAME] = "";
    static char record_id_input[32] = "";
    static int active_input = 0;
    static bool showResult = false;
    static bool showError = false;
    static char message[256] = "";

    DrawCenteredText("Delete Record", 50, 40, DARKBLUE);
    
    int inputWidth = 300;
    int inputHeight = 40;
    int startY = 150;

    DrawText("Enter File Name:", 450, startY, 20, BLACK);
    Rectangle filenameBox = {450, startY + 30, inputWidth, inputHeight};
    DrawRectangleRec(filenameBox, (active_input == 1) ? LIGHTGRAY : WHITE);
    DrawRectangleLinesEx(filenameBox, 2, BLUE);
    DrawText(filename, 460, startY + 40, 20, BLACK);

    DrawText("Enter Record ID:", 450, startY + 100, 20, BLACK);
    Rectangle idBox = {450, startY + 130, inputWidth, inputHeight};
    DrawRectangleRec(idBox, (active_input == 2) ? LIGHTGRAY : WHITE);
    DrawRectangleLinesEx(idBox, 2, BLUE);
    DrawText(record_id_input, 460, startY + 140, 20, BLACK);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), filenameBox)) active_input = 1;
        else if (CheckCollisionPointRec(GetMousePosition(), idBox)) active_input = 2;
        else active_input = 0;
    }
    if (active_input > 0) {
        int key = GetCharPressed();
        char *currentInput = (active_input == 1) ? filename : record_id_input;
        int maxLen = (active_input == 1) ? MAX_FILENAME - 1 : 31;
        
        while (key > 0) {
            if ((key >= 32 && key <= 126) && strlen(currentInput) < maxLen) {
                int len = strlen(currentInput);
                currentInput[len] = (char)key;
                currentInput[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(currentInput) > 0) {
            currentInput[strlen(currentInput) - 1] = '\0';
        }
    }
    
    // Delete Button
    Rectangle deleteBtn = {450, startY + 200, inputWidth, 50};
    bool btnHovered = CheckCollisionPointRec(GetMousePosition(), deleteBtn);
    DrawRectangleRec(deleteBtn, btnHovered ? DARKBROWN : RED);
    DrawText("Delete Record", 520, startY + 215, 20, WHITE);
    
    // Handle Delete Operation
    if (btnHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (strlen(filename) > 0 && strlen(record_id_input) > 0) {
            char buffer[BUFFER_SIZE];
            File *file = find_file(sm, filename, buffer);
            
            if (file == NULL) {
                showError = true;
                strcpy(message, "File not found!");
            } else {
                FILE *fp = fopen(filename, "rb");
                if (fp == NULL) {
                    showError = true;
                    strcpy(message, "Error opening file!");
                } else {
                    Record *records = malloc(sizeof(Record) * file->metadata.size_in_records);
                    int count = 0;
                    int found = 0;
                    int record_id = atoi(record_id_input);
                    
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
                        if (fp != NULL) {
                            fwrite(records, sizeof(Record), count, fp);
                            fclose(fp);
                            file->metadata.size_in_records--;
                            showResult = true;
                            showError = false;
                            sprintf(message, "Record with ID %d deleted successfully!", record_id);

                            filename[0] = '\0';
                            record_id_input[0] = '\0';
                        } else {
                            showError = true;
                            strcpy(message, "Error writing to file!");
                        }
                    } else {
                        showError = true;
                        sprintf(message, "Record with ID %d not found!", record_id);
                    }
                    free(records);
                }
            }
        } else {
            showError = true;
            strcpy(message, "Please fill all fields!");
        }
    }

    if (showResult || showError) {
        DrawText(message, 450, startY + 280, 20, showError ? RED : GREEN);
    }

    if (IsKeyPressed(KEY_SPACE)) {
        *state = STATE_MAIN_MENU;
        TransitionEffect(WHITE, 60);
    }
}


void ShowDefragmentFileScreen(AppState *state, SecondaryMemory *sm) {
    static char filename[MAX_FILENAME] = "";
    static int active_input = 0;
    static bool showResult = false;
    static bool showError = false;
    static char message[256] = "";
    
    DrawCenteredText("Defragment File", 50, 40, DARKBLUE);
    
    int inputWidth = 300;
    int inputHeight = 40;
    int startY = 200;
    
    DrawText("Enter File Name:", 450, startY, 20, BLACK);
    Rectangle filenameBox = {450, startY + 30, inputWidth, inputHeight};
    DrawRectangleRec(filenameBox, (active_input == 1) ? LIGHTGRAY : WHITE);
    DrawRectangleLinesEx(filenameBox, 2, BLUE);
    DrawText(filename, 460, startY + 40, 20, BLACK);
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), filenameBox)) {
            active_input = 1;
        } else {
            active_input = 0;
        }
    }
    

    if (active_input == 1) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32 && key <= 126) && strlen(filename) < MAX_FILENAME - 1) {
                int len = strlen(filename);
                filename[len] = (char)key;
                filename[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(filename) > 0) {
            filename[strlen(filename) - 1] = '\0';
        }
    }

    Rectangle defragBtn = {450, startY + 100, inputWidth, 50};
    bool btnHovered = CheckCollisionPointRec(GetMousePosition(), defragBtn);
    DrawRectangleRec(defragBtn, btnHovered ? DARKBLUE : BLUE);
    DrawText("Defragment File", 500, startY + 115, 20, WHITE);
    
    if (btnHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (strlen(filename) > 0) {
            char buffer[BUFFER_SIZE];
            File *file = find_file(sm, filename, buffer);
            
            if (file != NULL) {
                showResult = true;
                showError = false;
                sprintf(message, "Defragmentation completed for file '%s'", filename);
                filename[0] = '\0';
            } else {
                showError = true;
                showResult = false;
                sprintf(message, "File '%s' not found", filename);
            }
        } else {
            showError = true;
            showResult = false;
            strcpy(message, "Please enter a filename");
        }
    }

    if (showResult || showError) {
        DrawText(message, 450, startY + 180, 20, showError ? RED : GREEN);
    }
    if (IsKeyPressed(KEY_SPACE)) {
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