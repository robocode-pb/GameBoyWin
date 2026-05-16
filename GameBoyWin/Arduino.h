// Arduino.h
#ifndef Arduino_h
#define Arduino_h

#include <cmath>
#include <cstdlib>
#include "GameBoy.h" // Для доступу до gb, delay, millis

// --- Математичні приколи Arduino ---
// Важливо: оформлюємо як inline або макроси, щоб не було помилок лінкувальника

inline long map(long x, long in_min, long in_max, long out_max) {
    // Класична формула Arduino map
    return (x - in_min) * (out_max - in_min) / (in_max - in_min) + in_min; 
}

// Заміна для random() в Arduino
inline long random(long max) {
    if (max <= 0) return 0;
    return std::rand() % max;
}

inline long random(long min, long max) {
    if (min >= max) return min;
    return min + (std::rand() % (max - min));
}

inline void randomSeed(unsigned long seed) {
    std::srand(seed);
}

// Запобігаємо конфліктам з pow
#define pow(base, exponent) std::pow(base, exponent)


inline void delay(unsigned long ms) {
    double startTime = GetTime();
    double targetTime = ms / 1000.0;
    extern GameBoy gb; 
    while ((GetTime() - startTime) < targetTime) {
        if (WindowShouldClose()) std::exit(0); 
        gb.drawDisplay();
    }
}

inline unsigned long millis() { return (unsigned long)(GetTime() * 1000.0); }


#endif // Arduino_h