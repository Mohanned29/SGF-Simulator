

/*        DONE BY JINX          */

#include "C:\raylib\raylib\src\raylib.h"
#include <stdio.h>
#include <string.h>

#define MAX_TEXT_LENGTH 32

// Define the structure to store data (if needed)
typedef struct {
    int value;
    char input_text[MAX_TEXT_LENGTH];
    bool is_typing; // Flag to check if the user is typing
} GameData;

int main(void)
{
    // Initialize Raylib window
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Raylib Custom UI Example");

    // Game data
    GameData data = {0, "0", false};

    // Set up font
    Font font = GetFontDefault();

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        Vector2 mousePosition = GetMousePosition();

        // Handle mouse click events
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // Handle click for input text box
            if (CheckCollisionPointRec(mousePosition, (Rectangle){100, 100, 200, 40})) {
                data.is_typing = true; // Activate typing mode
            } else {
                data.is_typing = false; // Deactivate typing mode
            }

            // Handle button click (simple increment button)
            if (CheckCollisionPointRec(mousePosition, (Rectangle){100, 200, 200, 50})) {
                data.value++;
                snprintf(data.input_text, sizeof(data.input_text), "%d", data.value);
            }
        }

        // Handle keyboard input (if typing is active)
        if (data.is_typing) {
            for (int key = 32; key <= 126; key++) {  // ASCII printable characters range
                if (IsKeyPressed(key)) {
                    // Append the character to the input text
                    int len = strlen(data.input_text);
                    if (len < MAX_TEXT_LENGTH - 1) {
                        data.input_text[len] = (char)key;
                        data.input_text[len + 1] = '\0'; // Null-terminate the string
                    }
                }
            }

            // Handle backspace key
            if (IsKeyPressed(KEY_BACKSPACE)) {
                int len = strlen(data.input_text);
                if (len > 0) {
                    data.input_text[len - 1] = '\0'; // Remove last character
                }
            }
        }

        // Drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw the input box (text area)
        DrawRectangle(100, 100, 200, 40, LIGHTGRAY);
        DrawText(data.input_text, 110, 110, 20, BLACK);

        // Draw the "Click Me" button manually
        DrawRectangle(100, 200, 200, 50, BLUE);
        DrawText("Click Me", 150, 215, 20, WHITE);

        // Draw current value
        DrawText("Current Value:", 100, 150, 20, DARKGRAY);
        DrawText(data.input_text, 230, 150, 20, BLACK);

        EndDrawing();
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context

    return 0;
}

// gcc -o main main.c filesystem.c -lraylib -lopengl32 -lm -lgdi32