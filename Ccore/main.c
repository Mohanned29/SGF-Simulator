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
void ShowRenameFileScreen(AppState *state, SecondaryMemory *sm);
void ShowDeleteFileScreen(AppState *state, SecondaryMemory *sm);
void ShowDefragmentFileScreen(AppState *state, SecondaryMemory *sm);
void ShowClearSecondaryMemoryScreen(AppState *state, SecondaryMemory *sm);
void ShowCompactMemoryScreen(AppState *state, SecondaryMemory *sm);
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
            case STATE_CLEAR_SECONDARY_MEMORY:
                ShowClearSecondaryMemoryScreen(&state, &sm);
                break;
            case STATE_CREATE_FILE:
                ShowCreateFileScreen(&sm,&state);
                break;
            case STATE_COMPACT_MEMORY:
                ShowCompactMemoryScreen(&state, &sm);
                break;
            case STATE_SEARCH_RECORD:
                ShowSearchRecordScreen(&state, &sm);
                break;
            case STATE_DELETE_FILE:
                ShowDeleteFileScreen(&state, &sm);
                break;
            case STATE_RENAME_FILE:
                ShowRenameFileScreen(&state, &sm);
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
        DrawText(progressText, progressBarX + progressBarWidth / 2 - MeasureText(progressText, 20) / 2, progressBarY - 30, 20, BLACK);
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
    static int active_input = 0;
    static char error_msg[256] = "";
    static bool showResult = false;
    static bool showError = false;
    static GlobalOrganization global_org = CONTIGUOUS;
    static InternalOrganization internal_org = UNSORTED;
    DrawCenteredText("Create New File", 50, 40, DARKBLUE);

    DrawText("File Name:", 450, 150, 20, BLACK);
    Rectangle filenameBox = {450, 180, 300, 40};
    DrawRectangleRec(filenameBox, (active_input == 1) ? LIGHTGRAY : WHITE);
    DrawRectangleLinesEx(filenameBox, 2, BLUE);
    DrawText(filename, 460, 190, 20, BLACK);

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

    // Organization Buttons
    DrawText("Global Organization:", 450, 250, 20, BLACK);
    Rectangle contButton = {450, 280, 140, 40};
    Rectangle chainButton = {600, 280, 140, 40};
    
    DrawRectangleRec(contButton, global_org == CONTIGUOUS ? DARKBLUE : BLUE);
    DrawRectangleRec(chainButton, global_org == CHAINED ? DARKBLUE : BLUE);
    DrawText("Contiguous", 465, 290, 20, WHITE);
    DrawText("Chained", 625, 290, 20, WHITE);

    DrawText("Internal Organization:", 450, 350, 20, BLACK);
    Rectangle unsortedButton = {450, 380, 140, 40};
    Rectangle sortedButton = {600, 380, 140, 40};
    
    DrawRectangleRec(unsortedButton, internal_org == UNSORTED ? DARKBLUE : BLUE);
    DrawRectangleRec(sortedButton, internal_org == SORTED ? DARKBLUE : BLUE);
    DrawText("Unsorted", 465, 390, 20, WHITE);
    DrawText("Sorted", 625, 390, 20, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), contButton)) global_org = CONTIGUOUS;
        if (CheckCollisionPointRec(GetMousePosition(), chainButton)) global_org = CHAINED;
        if (CheckCollisionPointRec(GetMousePosition(), unsortedButton)) internal_org = UNSORTED;
        if (CheckCollisionPointRec(GetMousePosition(), sortedButton)) internal_org = SORTED;
    }

    Rectangle createBtn = {450, 460, 300, 50};
    bool btnHovered = CheckCollisionPointRec(GetMousePosition(), createBtn);
    DrawRectangleRec(createBtn, btnHovered ? DARKGREEN : GREEN);
    DrawText("Create File", 520, 475, 20, WHITE);

    if (btnHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (strlen(filename) > 0) {
            if (create_file(sm, filename, global_org, internal_org, error_msg)) {
                showResult = true;
                showError = false;
                filename[0] = '\0';
            } else {
                showError = true;
                showResult = false;
            }
        } else {
            showError = true;
            strcpy(error_msg, "Please enter a filename");
        }
    }

    // Show Results
    if (showResult) {
        DrawText("File created successfully!", 450, 540, 20, GREEN);
    } else if (showError) {
        DrawText(error_msg, 450, 540, 20, RED);
    }

    // Return to Main Menu
    if (IsKeyPressed(KEY_SPACE)) {
        *state = STATE_MAIN_MENU;
        TransitionEffect(WHITE, 60);
    }
}


void ShowSearchRecordScreen(AppState *state, SecondaryMemory *sm) {
    static char filename[MAX_FILENAME] = "";
    static char record_id_input[13] = "";
    static int active_input = 0;
    static bool showResult = false;
    static bool showError = false;
    static Record found_record;
    static char message[100] = "";
    static float searchAnimation = 0.0f;
    
    DrawCenteredText("Search Record", 50, 40, DARKBLUE);

    int inputWidth = 300;
    int inputHeight = 40;
    int startY = 150;
    
    DrawText("Enter File Name:", 450, startY, 20, BLACK);
    Rectangle filenameBox = {450, startY + 30, inputWidth, inputHeight};
    DrawRectangleRec(filenameBox, (active_input == 1) ? LIGHTGRAY : WHITE);
    DrawRectangleLinesEx(filenameBox, 2, BLUE);
    DrawText(filename, 460, startY + 40, 20, BLACK);

    // Record ID Input
    DrawText("Enter Matricule :", 450, startY + 100, 20, BLACK);
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

    if (active_input > 0) {
        int key = GetCharPressed();
        char *currentInput = (active_input == 1) ? filename : record_id_input;
        int maxLen = (active_input == 1) ? MAX_FILENAME - 1 : 12;
        
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

    Rectangle searchBtn = {450, startY + 200, inputWidth, 50};
    bool btnHovered = CheckCollisionPointRec(GetMousePosition(), searchBtn);
    DrawRectangleRec(searchBtn, btnHovered ? DARKBLUE : BLUE);
    DrawText("Search Record", 520, startY + 215, 20, WHITE);


    if (btnHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (strlen(filename) > 0 && strlen(record_id_input) > 0) {
            bool success;
            char *endptr;
            long matricule = strtol(record_id_input, &endptr, 10);
            char error_msg[100];
            Record* result = search_record(sm, filename, matricule, &success);
            
            if (success && result != NULL) {
                showResult = true;
                showError = false;
                found_record = *result;
                free(result);
            } else {
                showError = true;
                showResult = false;
                strcpy(message, error_msg);
            }
            searchAnimation = 0.0f;
        } else {
            showError = true;
            showResult = false;
            strcpy(message, "Please fill all fields!");
        }
    }

    if (showResult || showError) {
        searchAnimation += GetFrameTime() * 2;
        if (searchAnimation > 1.0f) searchAnimation = 1.0f;
        
        float alpha = searchAnimation;
        Rectangle resultBox = {350, startY + 280, 500, 100};
        DrawRectangleRec(resultBox, Fade(LIGHTGRAY, 0.3f));
        DrawRectangleLinesEx(resultBox, 2, BLUE);
        
        if (showResult) {
            char resultText[100];
            snprintf(resultText, sizeof(resultText), "Matricule: %d", found_record.id);
            DrawText(resultText, 370, startY + 300, 20, Fade(BLACK, alpha));
            DrawText(found_record.name, 370, startY + 330, 20, Fade(BLACK, alpha));
        } else {
            DrawText(message, 370, startY + 315, 20, Fade(RED, alpha));
        }
    }

    if (IsKeyPressed(KEY_SPACE)) {
        *state = STATE_MAIN_MENU;
        TransitionEffect(WHITE, 60);
    }
}

void ShowDisplayMemoryScreen(AppState *state, SecondaryMemory *sm) {

    static char memoryStateBuffer[BUFFER_SIZE];
    display_memory_state(sm, memoryStateBuffer);
    DrawCenteredText("Memory State", 50, 40, DARKBLUE);

    int blockSize = 50;
    int padding = 10;
    int blocksPerRow = 10;
    int startX = (GetScreenWidth() - (blocksPerRow * (blockSize + padding))) / 2;
    int startY = 150;
    
    Color fileColors[] = {
        RED, BLUE, PURPLE, ORANGE, BROWN,
        PINK, MAROON, DARKBLUE, DARKPURPLE, DARKBROWN
    };
    int numColors = sizeof(fileColors) / sizeof(fileColors[0]);
    
    Color* blockColors = (Color*)malloc(sm->total_blocks * sizeof(Color));
    for (int i = 0; i < sm->total_blocks; i++) {
        blockColors[i] = GREEN;
    }
    
    float records_per_block = (float)sm->block_size / sizeof(Record);
    char recordsInfo[32];
    snprintf(recordsInfo, sizeof(recordsInfo), "Records per block: %.2f", records_per_block);
    DrawText(recordsInfo, startX, startY - 140, 20, BLACK);
    
    int colorIndex = 0;
    int lastUsedBlock = -1;
    File* current = sm->file_list;
    
    while (current != NULL) {
        Color fileColor = fileColors[colorIndex % numColors];
        
        if (current->metadata.global_org == CONTIGUOUS) {
            for (int i = current->metadata.first_block_address; 
                 i < current->metadata.first_block_address + current->metadata.size_in_blocks; 
                 i++) {
                blockColors[i] = fileColor;
                lastUsedBlock = (i > lastUsedBlock) ? i : lastUsedBlock;
            }
        } else {
            int startBlock = (lastUsedBlock + 1);
            for (int i = 0; i < current->metadata.size_in_blocks; i++) {
                blockColors[startBlock + i] = fileColor;
                lastUsedBlock = startBlock + i;
            }
        }
        colorIndex++;
        current = current->next;
    }

    for (int i = 0; i < sm->total_blocks; i++) {
        int row = i / blocksPerRow;
        int col = i % blocksPerRow;
        int x = startX + col * (blockSize + padding);
        int y = startY + row * (blockSize + padding);
        DrawRectangle(x, y, blockSize, blockSize, blockColors[i]);
        char blockNum[5];
        snprintf(blockNum, sizeof(blockNum), "%d", i);
        int textWidth = MeasureText(blockNum, 20);
        DrawText(blockNum, x + (blockSize - textWidth)/2, y + blockSize/3, 20, WHITE);
    }

    DrawRectangle(startX, startY - 60, 20, 20, GREEN);
    DrawText("Free", startX + 30, startY - 60, 20, BLACK);

    current = sm->file_list;
    colorIndex = 0;
    int legendX = startX + 150;
    while (current != NULL && colorIndex < numColors) {
        DrawRectangle(legendX, startY - 60, 20, 20, fileColors[colorIndex]);
        DrawText(current->metadata.filename, legendX + 30, startY - 60, 20, BLACK);
        legendX += 150;
        colorIndex++;
        current = current->next;
    }
    
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
    
    free(blockColors);
    
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
    
    DrawText("Press SPACE to return to main menu", GetScreenWidth()/2 - MeasureText("Press SPACE to return to main menu", 20)/2,GetScreenHeight() - 50, 20, DARKGRAY);
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
    static float messageTimer = 0.0f;
    
    DrawCenteredText("Insert New Record", 50, 40, DARKBLUE);

    int inputWidth = 300;
    int inputHeight = 40;
    int startY = 150;

    DrawText("Enter File Name:", 450, startY, 20, BLACK);
    Rectangle filenameBox = {450, startY + 30, inputWidth, inputHeight};
    DrawRectangleRec(filenameBox, (active_input == 1) ? LIGHTGRAY : WHITE);
    DrawRectangleLinesEx(filenameBox, 2, BLUE);
    DrawText(filename, 460, startY + 40, 20, BLACK);

    DrawText("Enter Matricule:", 450, startY + 100, 20, BLACK);
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

    Rectangle insertBtn = {450, startY + 300, inputWidth, 50};
    bool btnHovered = CheckCollisionPointRec(GetMousePosition(), insertBtn);
    DrawRectangleRec(insertBtn, btnHovered ? DARKBLUE : BLUE);
    DrawText("Insert Record", 520, startY + 315, 20, WHITE);

    if (btnHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (strlen(filename) > 0 && strlen(record_id) > 0 && strlen(record_data) > 0) {
            bool success;
            char *endptr;
            long matricule = strtol(record_id, &endptr, 10);
            char error_msg[256];
            if (insert_record(sm, filename, matricule, record_data)) {
                showSuccess = true;
                showError = false;
                strcpy(message, "Record inserted successfully!");
                filename[0] = '\0';
                record_id[0] = '\0';
                record_data[0] = '\0';
            } else {
                showError = true;
                showSuccess = false;
                strcpy(message, error_msg);
            }
            messageTimer = 2.0f;
        } else {
            showError = true;
            showSuccess = false;
            strcpy(message, "Please fill all fields!");
            messageTimer = 2.0f;
        }
    }

    if (messageTimer > 0) {
        DrawText(message, 450, startY + 370, 20, showError ? RED : GREEN);
        messageTimer -= GetFrameTime();
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
    static bool is_physical = true;
    static float messageTimer = 0.0f;
    
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

    DrawText("Select Deletion Type:", 450, startY + 180, 20, BLACK);
    Rectangle physicalBtn = {450, startY + 210, inputWidth/2 - 5, 40};
    Rectangle logicalBtn = {450 + inputWidth/2 + 5, startY + 210, inputWidth/2 - 5, 40};
    
    DrawRectangleRec(physicalBtn, is_physical ? DARKBLUE : BLUE);
    DrawRectangleRec(logicalBtn, !is_physical ? DARKBLUE : BLUE);
    DrawText("Physical", 460, startY + 220, 20, WHITE);
    DrawText("Logical", 460 + inputWidth/2 + 5, startY + 220, 20, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), filenameBox)) active_input = 1;
        else if (CheckCollisionPointRec(GetMousePosition(), idBox)) active_input = 2;
        else if (CheckCollisionPointRec(GetMousePosition(), physicalBtn)) is_physical = true;
        else if (CheckCollisionPointRec(GetMousePosition(), logicalBtn)) is_physical = false;
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

    Rectangle deleteBtn = {450, startY + 270, inputWidth, 50};
    bool btnHovered = CheckCollisionPointRec(GetMousePosition(), deleteBtn);
    DrawRectangleRec(deleteBtn, btnHovered ? DARKBROWN : RED);
    DrawText("Delete Record", 520, startY + 285, 20, WHITE);

    if (btnHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (strlen(filename) > 0 && strlen(record_id_input) > 0) {
            char error_msg[256];
            char *endptr;
            long matricule = strtol(record_id_input, &endptr, 10);
            if (delete_record(sm, filename, matricule, is_physical, message)) {
                showResult = true;
                showError = false;
                filename[0] = '\0';
                record_id_input[0] = '\0';
            } else {
                showResult = false;
                showError = true;
            }
            strcpy(message, error_msg);
            messageTimer = 2.0f;
        } else {
            showError = true;
            strcpy(message, "Please fill all fields!");
            messageTimer = 2.0f;
        }
    }

    if (messageTimer > 0) {
        DrawText(message, 450, startY + 340, 20, showError ? RED : GREEN);
        messageTimer -= GetFrameTime();
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
    static float messageTimer = 0.0f;
    
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
            char error_msg[256];
            if (defragment_file(sm, filename, error_msg)) {
                showResult = true;
                showError = false;
                sprintf(message, "Defragmentation completed for file '%s'", filename);
                filename[0] = '\0';
            } else {
                showError = true;
                showResult = false;
                strcpy(message, error_msg);
            }
            messageTimer = 2.0f;
        } else {
            showError = true;
            showResult = false;
            strcpy(message, "Please enter a filename");
            messageTimer = 2.0f;
        }
    }

    if (messageTimer > 0) {
        DrawText(message, 450, startY + 180, 20, showError ? RED : GREEN);
        messageTimer -= GetFrameTime();
    }

    if (IsKeyPressed(KEY_SPACE)) {
        *state = STATE_MAIN_MENU;
        TransitionEffect(WHITE, 60);
    }
}



void ShowDeleteFileScreen(AppState *state, SecondaryMemory *sm) {
    static char filename[MAX_FILENAME] = "";
    static int active_input = 0;
    static bool showResult = false;
    static bool showError = false;
    static char message[256] = "";
    static float messageTimer = 0;
    DrawCenteredText("Delete File", 50, 40, DARKBLUE);
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

    Rectangle deleteBtn = {450, startY + 100, inputWidth, 50};
    bool btnHovered = CheckCollisionPointRec(GetMousePosition(), deleteBtn);
    DrawRectangleRec(deleteBtn, btnHovered ? DARKBROWN : RED);
    DrawText("Delete File", 520, startY + 115, 20, WHITE);
    
    if (btnHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (strlen(filename) > 0) {
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
            
            if (current != NULL) {
                if (remove(filename) == 0) {
                    if (current->metadata.global_org == CONTIGUOUS) {
                        for (int i = current->metadata.first_block_address; i < current->metadata.first_block_address + current->metadata.size_in_blocks; i++) {
                            sm->allocation_table[i] = 0;
                        }
                    } else {
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
                    showResult = true;
                    showError = false;
                    sprintf(message, "File '%s' deleted successfully", filename);
                    filename[0] = '\0';
                } else {
                    showError = true;
                    sprintf(message, "Error deleting file '%s'", filename);
                }
            } else {
                showError = true;
                sprintf(message, "File '%s' not found", filename);
            }
            messageTimer = 3.0f;
        }
    }

    if (messageTimer > 0) {
        DrawText(message, 450, startY + 180, 20, showError ? RED : GREEN);
        messageTimer -= GetFrameTime();
    }

    if (IsKeyPressed(KEY_SPACE)) {
        *state = STATE_MAIN_MENU;
        TransitionEffect(WHITE, 60);
    }
}


void ShowRenameFileScreen(AppState *state, SecondaryMemory *sm) {
    static char old_filename[MAX_FILENAME] = "";
    static char new_filename[MAX_FILENAME] = "";
    static int active_input = 0;
    static bool showResult = false;
    static bool showError = false;
    static char message[256] = "";
    
    DrawCenteredText("Rename File", 50, 40, DARKBLUE);
    
    int inputWidth = 300;
    int inputHeight = 40;
    int startY = 150;
    
    DrawText("Current File Name:", 450, startY, 20, BLACK);
    Rectangle oldFilenameBox = {450, startY + 30, inputWidth, inputHeight};
    DrawRectangleRec(oldFilenameBox, (active_input == 1) ? LIGHTGRAY : WHITE);
    DrawRectangleLinesEx(oldFilenameBox, 2, BLUE);
    DrawText(old_filename, 460, startY + 40, 20, BLACK);
    
    DrawText("New File Name:", 450, startY + 100, 20, BLACK);
    Rectangle newFilenameBox = {450, startY + 130, inputWidth, inputHeight};
    DrawRectangleRec(newFilenameBox, (active_input == 2) ? LIGHTGRAY : WHITE);
    DrawRectangleLinesEx(newFilenameBox, 2, BLUE);
    DrawText(new_filename, 460, startY + 140, 20, BLACK);
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), oldFilenameBox)) active_input = 1;
        else if (CheckCollisionPointRec(GetMousePosition(), newFilenameBox)) active_input = 2;
        else active_input = 0;
    }
    
    if (active_input > 0) {
        int key = GetCharPressed();
        char *currentInput = (active_input == 1) ? old_filename : new_filename;
        
        while (key > 0) {
            if ((key >= 32 && key <= 126) && strlen(currentInput) < MAX_FILENAME - 1) {
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
    
    Rectangle renameBtn = {450, startY + 200, inputWidth, 50};
    bool btnHovered = CheckCollisionPointRec(GetMousePosition(), renameBtn);
    DrawRectangleRec(renameBtn, btnHovered ? DARKBLUE : BLUE);
    DrawText("Rename File", 520, startY + 215, 20, WHITE);

    if (btnHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (strlen(old_filename) > 0 && strlen(new_filename) > 0) {
            char buffer[BUFFER_SIZE];
            File *file = find_file(sm, old_filename, buffer);
            
            if (file == NULL) {
                showError = true;
                sprintf(message, "File '%s' not found", old_filename);
            } else if (find_file(sm, new_filename, buffer) != NULL) {
                showError = true;
                sprintf(message, "File '%s' already exists", new_filename);
            } else {
                if (rename(old_filename, new_filename) == 0) {
                    strcpy(file->metadata.filename, new_filename);
                    showResult = true;
                    showError = false;
                    sprintf(message, "File renamed to '%s'", new_filename);
                    old_filename[0] = '\0';
                    new_filename[0] = '\0';
                } else {
                    showError = true;
                    sprintf(message, "Error renaming file '%s'", old_filename);
                }
            }
        } else {
            showError = true;
            strcpy(message, "Please fill both fields");
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


void ShowCompactMemoryScreen(AppState *state, SecondaryMemory *sm) {
    static bool showConfirmation = false;
    static bool showResult = false;
    static float messageTimer = 0;
    static bool animationStarted = false;
    static float animationProgress = 0;

    DrawCenteredText("Compact Memory", 50, 40, DARKBLUE);
    
    if (!showConfirmation && !showResult) {
        DrawCenteredText("This operation will reorganize memory blocks to eliminate fragmentation.", 150, 20, BLACK);
        
        Rectangle compactBtn = {450, 300, 300, 50};
        bool btnHovered = CheckCollisionPointRec(GetMousePosition(), compactBtn);
        DrawRectangleRec(compactBtn, btnHovered ? DARKBLUE : BLUE);
        DrawText("Compact Memory", 510, 315, 20, WHITE);
        
        if (btnHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showConfirmation = true;
        }
    } else if (showConfirmation) {
        DrawRectangle(350, 250, 500, 200, Fade(LIGHTGRAY, 0.9f));
        DrawRectangleLinesEx((Rectangle){350, 250, 500, 200}, 2, DARKGRAY);
        DrawText("Proceed with memory compaction?", 400, 280, 20, BLACK);

        Rectangle yesBtn = {400, 350, 150, 40};
        bool yesHovered = CheckCollisionPointRec(GetMousePosition(), yesBtn);
        DrawRectangleRec(yesBtn, yesHovered ? DARKBLUE : BLUE);
        DrawText("Yes", 460, 360, 20, WHITE);

        Rectangle noBtn = {600, 350, 150, 40};
        bool noHovered = CheckCollisionPointRec(GetMousePosition(), noBtn);
        DrawRectangleRec(noBtn, noHovered ? DARKGRAY : LIGHTGRAY);
        DrawText("No", 665, 360, 20, BLACK);
        
        if (yesHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            compact_memory(sm);
            showConfirmation = false;
            showResult = true;
            messageTimer = 2.0f;
            animationStarted = true;
        } else if (noHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showConfirmation = false;
        }
    }
    if (animationStarted) {
        animationProgress += GetFrameTime();
        if (animationProgress <= 1.0f) {
            DrawRectangle(400, 400, 400, 30, LIGHTGRAY);
            DrawRectangle(400, 400, (int)(400 * animationProgress), 30, BLUE);
            DrawText(TextFormat("%d%%", (int)(animationProgress * 100)), 
                    580, 405, 20, BLACK);
        } else {
            animationStarted = false;
            animationProgress = 0;
        }
    }
    
    if (showResult) {
        messageTimer -= GetFrameTime();
        if (messageTimer > 0) {
            DrawCenteredText("Memory compaction completed successfully!", 400, 25, GREEN);
        } else {
            showResult = false;
        }
    }

    if (IsKeyPressed(KEY_SPACE)) {
        *state = STATE_MAIN_MENU;
        TransitionEffect(WHITE, 60);
    }
}


void ShowClearSecondaryMemoryScreen(AppState *state, SecondaryMemory *sm) {
    static bool showConfirmation = false;
    static bool showResult = false;
    static float messageTimer = 0;

    DrawCenteredText("Clear Secondary Memory", 50, 40, DARKBLUE);
    
    if (!showConfirmation && !showResult) {
        DrawCenteredText("Warning: This action will delete all files and clear memory.", 200, 25, RED);
        DrawCenteredText("This action cannot be undone.", 250, 25, RED);
        
        Rectangle clearBtn = {450, 300, 300, 50};
        bool btnHovered = CheckCollisionPointRec(GetMousePosition(), clearBtn);
        DrawRectangleRec(clearBtn, btnHovered ? DARKBROWN : RED);
        DrawText("Clear Memory", 520, 315, 20, WHITE);
        
        if (btnHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showConfirmation = true;
        }
    } else if (showConfirmation) {
        DrawRectangle(350, 250, 500, 200, Fade(LIGHTGRAY, 0.9f));
        DrawRectangleLinesEx((Rectangle){350, 250, 500, 200}, 2, DARKGRAY);
        DrawText("Are you sure you want to clear memory?", 400, 280, 20, BLACK);
        
        Rectangle yesBtn = {400, 350, 150, 40};
        bool yesHovered = CheckCollisionPointRec(GetMousePosition(), yesBtn);
        DrawRectangleRec(yesBtn, yesHovered ? DARKBROWN : RED);
        DrawText("Yes", 460, 360, 20, WHITE);
        
        Rectangle noBtn = {600, 350, 150, 40};
        bool noHovered = CheckCollisionPointRec(GetMousePosition(), noBtn);
        DrawRectangleRec(noBtn, noHovered ? DARKBLUE : BLUE);
        DrawText("No", 665, 360, 20, WHITE);
        
        if (yesHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            clear_memory(sm);
            showConfirmation = false;
            showResult = true;
            messageTimer = 2.0f;
        } else if (noHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showConfirmation = false;
        }
    }
    
    if (showResult) {
        messageTimer -= GetFrameTime();
        if (messageTimer > 0) {
            DrawCenteredText("Memory cleared successfully!", 400, 25, GREEN);
        } else {
            showResult = false;
        }
    }
    
    if (IsKeyPressed(KEY_SPACE)) {
        *state = STATE_MAIN_MENU;
        TransitionEffect(WHITE, 60);
    }
}
