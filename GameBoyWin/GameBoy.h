#ifndef GameBoy_h
#define GameBoy_h

#include "raylib.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

typedef unsigned char byte;
typedef bool boolean;

// Коди кнопок
#define UP          1
#define DOWN        2
#define LEFT        3
#define RIGHT       4
#define BTN_START   5
#define BTN_SELECT  6

#define SCORE 1
#define COLLISION 2

class GameBoy {
private:
    Texture2D bgTexture;
    bool textureLoaded = false;
    bool debugBlinkState = false; 

    // ========================================================
    // 1. ІДЕАЛЬНЕ КАЛІБРУВАННЯ МАТРИЦІ (Під GameBoy.png 420x720)
    // ========================================================
    int startX   = 151; // Точний центр першого лівого діода
    int startY   = 50;  // Точний центр першого верхнього діода
    int ledSize  = 17;  // Розмір діода
    int spacingX = ledSize+5;  // Точний крок між діодами
    int spacingY = spacingX;  
    int gapY     = 2;   // Щілина між двома матрицями

    // ========================================================
    // 2. ІДЕАЛЬНЕ КАЛІБРУВАННЯ КНОПОК
    // ========================================================
    int btnSpaceY = 144;
    int btnUpX    = 119, btnUpY    = 456;
    int btnDownX  = btnUpX, btnDownY  = btnUpY+btnSpaceY;
    int btnLeftX  = 49,  btnLeftY  = btnUpY+(btnSpaceY/2);
    int btnRightX = 190, btnRightY = btnLeftY;
    
    // Праві кнопки дій
    int btnSelectX = 350, btnSelectY = btnUpY; 
    int btnStartX  = 279, btnStartY  = btnLeftY; 
    
    int piezoX = 65, piezoY = 340; // Динамік
    int btnRadius = 22; 
    // ========================================================

    bool isPressedUp()     { return IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || (IsMouseButtonDown(0) && CheckCollisionPointCircle(GetMousePosition(), {(float)btnUpX, (float)btnUpY}, btnRadius)); }
    bool isPressedDown()   { return IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S) || (IsMouseButtonDown(0) && CheckCollisionPointCircle(GetMousePosition(), {(float)btnDownX, (float)btnDownY}, btnRadius)); }
    bool isPressedLeft()   { return IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) || (IsMouseButtonDown(0) && CheckCollisionPointCircle(GetMousePosition(), {(float)btnLeftX, (float)btnLeftY}, btnRadius)); }
    bool isPressedRight()  { return IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D) || (IsMouseButtonDown(0) && CheckCollisionPointCircle(GetMousePosition(), {(float)btnRightX, (float)btnRightY}, btnRadius)); }
    bool isPressedStart()  { return IsKeyDown(KEY_Z) || (IsMouseButtonDown(0) && CheckCollisionPointCircle(GetMousePosition(), {(float)btnStartX, (float)btnStartY}, btnRadius)); }
    bool isPressedSelect() { return IsKeyDown(KEY_X) || (IsMouseButtonDown(0) && CheckCollisionPointCircle(GetMousePosition(), {(float)btnSelectX, (float)btnSelectY}, btnRadius)); }

    void drawButtonEffect(int centerX, int centerY, bool isPressed) {
        bool isHovered = CheckCollisionPointCircle(GetMousePosition(), {(float)centerX, (float)centerY}, btnRadius);
        if (isPressed || debugBlinkState) {
            DrawCircle(centerX, centerY, btnRadius, Fade(RED, 0.35f));
            DrawCircleLines(centerX, centerY, (float)btnRadius, RED);
        } else if (isHovered) {
            DrawCircleLines(centerX, centerY, (float)btnRadius, Fade(LIGHTGRAY, 0.6f));
        }
    }

public:
    int maxDevices = 2;
    const int addr = 1;
    bool display[8][16];
    byte block[4][4][4];

    GameBoy() {
        clearDisplay();
        std::memset(block, 0, sizeof(block));
    }

    ~GameBoy() { if (textureLoaded) UnloadTexture(bgTexture); }

    void begin(int Intensity) {
        if (!IsWindowReady()) {
            InitWindow(420, 648, "GameBoy Emulator");
            SetTargetFPS(60);

            bgTexture = LoadTexture("imgs/GameBoy.png");
            if (bgTexture.id != 0) {
                textureLoaded = true;
            }
        }
        clearDisplay();
    }

    void debug() {
        std::cout << "[DEBUG] Запуск перевірки пристрою...\n";
        for (int i = 0; i < 6; i++) {
            debugBlinkState = (i % 2 == 0); 
            
            for (int x = 0; x < 8; x++)
                for (int y = 0; y < 16; y++)
                    display[x][y] = debugBlinkState;

            if (debugBlinkState) sound(1000); 

            double startTime = GetTime();
            while ((GetTime() - startTime) < 0.25) {
                if (WindowShouldClose()) std::exit(0); 
                drawDisplay();
            }
        }
        debugBlinkState = false;
        clearDisplay();
        std::cout << "[DEBUG] Перевірку завершено.\n";
    }

    void shutdown(bool status) {}
    void sound(int melody) { std::cout << "BEEP: " << melody << " Hz\n"; }
    void setIntensity(int intensity) {}
    void testMatrix(short int delaytime) {}
    void clearDisplay() { std::memset(display, 0, sizeof(display)); }

    void setLed(int row, int col, boolean state) { if (row >= 0 && row < 8 && col >= 0 && col < 16) display[row][col] = state; }
    void memDisplay(short int x, short int y) { if (x >= 0 && x < 8 && y >= 0 && y < 16) display[x][y] = true; }
    void drawPoint(int x, int y) { if (x >= 0 && x < 8 && y >= 0 && y < 16) display[x][y] = true; }
    void wipePoint(int x, int y) { if (x >= 0 && x < 8 && y >= 0 && y < 16) display[x][y] = false; }
    
    bool checkCollision(int x, int y) {
        if (x < 0 || x >= 8 || y < 0 || y >= 16) return true;
        return display[x][y];
    }
    bool chekState(int x, int y) {
        if (x >= 0 && x < 8 && y >= 0 && y < 16) return display[x][y];
        return false;
    }
    bool isFree(int x, int y) { return !checkCollision(x, y); }

    void clearLine(byte num_line) {
        if (num_line >= 0 && num_line < 16) for (int x = 0; x < 8; x++) display[x][num_line] = false;
    }
    void gravity(int vector_name) {
        for (int y = 14; y >= 0; y--) for (int x = 0; x < 8; x++) display[x][y + 1] = display[x][y];
        for (int x = 0; x < 8; x++) display[x][0] = false;
    }
    int moveX(int start_x, int start_y, int left_x, int right_x, int move_var) {
        wipePoint(start_x, start_y);
        int new_x = start_x + move_var;
        if (!checkCollision(new_x, start_y)) { drawPoint(new_x, start_y); return new_x; }
        drawPoint(start_x, start_y);
        return start_x;
    }
    int fullLine() {
        int clearedLines = 0;
        for (int y = 0; y < 16; y++) {
            bool full = true;
            for (int x = 0; x < 8; x++) { if (!display[x][y]) { full = false; break; } }
            if (full) { clearLine(y); clearedLines++; }
        }
        return clearedLines;
    }

    int getKey() {
        if (isPressedUp())     return UP;
        if (isPressedDown())   return DOWN;
        if (isPressedLeft())   return LEFT;
        if (isPressedRight())  return RIGHT;
        if (isPressedStart())  return BTN_START;
        if (isPressedSelect()) return BTN_SELECT;
        return 0;
    }

    void drawDisplay() {
        if (WindowShouldClose()) std::exit(0); 

        BeginDrawing();

        if (textureLoaded) {
            DrawTexture(bgTexture, 0, 0, WHITE);
        } else {
            ClearBackground(BLACK);
            DrawText("ERROR: GameBoy.png not found!", 10, 10, 20, RED);
        }

        // ===============================================================
        // Інструмент розробника: Клікніть ПКМ, щоб побачити координати
        // ===============================================================
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            std::cout << "\n>>> Координати миші: X = " << GetMouseX() << ", Y = " << GetMouseY() << " <<<\n\n";
        }

        if (debugBlinkState) {
            DrawCircleLines(piezoX, piezoY, 15.0f, RED);
            DrawCircleLines(piezoX, piezoY, 25.0f, Fade(RED, 0.5f));
            DrawCircleLines(piezoX, piezoY, 35.0f, Fade(RED, 0.2f));
        }

        drawButtonEffect(btnUpX, btnUpY, isPressedUp());
        drawButtonEffect(btnDownX, btnDownY, isPressedDown());
        drawButtonEffect(btnLeftX, btnLeftY, isPressedLeft());
        drawButtonEffect(btnRightX, btnRightY, isPressedRight());
        drawButtonEffect(btnStartX, btnStartY, isPressedStart());
        drawButtonEffect(btnSelectX, btnSelectY, isPressedSelect());

        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 16; y++) {
                if (display[x][y]) {
                    int posX = startX + (x * spacingX);
                    int posY = startY + (y * spacingY);
                    
                    // Щілина між екранами
                    if (y >= 8) posY += gapY;

                    DrawCircle(posX, posY, (ledSize / 2) + 2, Fade(RED, 0.4f));
                    DrawCircle(posX, posY, (ledSize / 2), RED);
                }
            }
        }

        EndDrawing();
    }
};


#endif // GameBoy_h