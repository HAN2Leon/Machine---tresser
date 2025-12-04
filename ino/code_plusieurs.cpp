#include <Arduino.h>

const uint16_t MICROSTEP        = 3200;
const uint16_t HALF_TURN_STEPS  = MICROSTEP / 2;

const uint16_t PULSE_US = 50; // µm
const uint8_t PUL_PINS[3] = {0, 1, 2};

void setup()
{
    // Initiation GPIO
    for (uint8_t i = 0; i < 3; ++i) {
        pinMode(PUL_PINS[i], OUTPUT);
        digitalWrite(PUL_PINS[i], LOW);
    }
}

void loop()
{
    static uint32_t i = 0;
    uint8_t pulIndex = i % 3;

    for (uint16_t step = 0; step < HALF_TURN_STEPS; ++step) {
        digitalWrite(PUL_PINS[pulIndex], HIGH);      // front montant
        delayMicroseconds(PULSE_US);
        digitalWrite(PUL_PINS[pulIndex], LOW);       // front descendant
        delayMicroseconds(PULSE_US);
    }

    delay(1000);
    ++i;
}
