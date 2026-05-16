#include "GameBoy.h"
#include "Arduino.h" // Підключаємо наш "емулятор" функцій Arduino


// Підключаємо сам sketch
#include "../sketch.ino"

int main() {
    // Ініціалізуємо генератор випадкових чисел по дефолту
    randomSeed(millis());

    gb.begin(10); // Або яка там у тебе інтенсивність
    
    setup();

    while (!WindowShouldClose()) {
        loop();
        // На випадок, якщо в loop() немає delay, оновлюємо екран тут
        gb.drawDisplay(); 
    }

    CloseWindow();
    return 0;
}