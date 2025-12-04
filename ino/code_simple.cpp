#include <Arduino.h>

const uint16_t MICROSTEP        = 3200;
const uint16_t HALF_TURN_STEPS  = MICROSTEP / 2;

const uint16_t PULSE_US = 50; // µm
const uint8_t PUL_PINS = 2;

void setup()
{
    // Initiation GPIO
    pinMode(PUL_PINS[i], OUTPUT);
    digitalWrite(PUL_PINS[i], LOW);
}

void loop()
{
    for (uint16_t step = 0; step < HALF_TURN_STEPS; ++step) {
        digitalWrite(PUL_PINS, HIGH);      // front montant
        delayMicroseconds(PULSE_US);
        digitalWrite(PUL_PINS, LOW);       // front descendant
        delayMicroseconds(PULSE_US);
    }

    delay(1000);
    ++i;
}
