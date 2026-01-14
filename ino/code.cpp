#include <Arduino.h>
#include <SPI.h>
#include <math.h>

const float TIME_WAIT_MIN = 0.2f;
const float TIME_WAIT_MAX = 3.0f;
const float DEADZONE      = 0.03f;

const uint16_t MICROSTEP        = 3200;
const uint16_t HALF_TURN_STEPS  = MICROSTEP / 2;

const uint16_t PULSE_US = 8; // µm

const uint8_t PUL_PINS[3] = {0, 1, 2};
const uint8_t DIR_PIN      = 3
const uint8_t ADC_CS_PIN = 7;

SPISettings adcSpiSettings(100000, MSBFIRST, SPI_MODE0); // 100kHz, mode 0

uint16_t readAdc(uint8_t ch = 0)
{
    ch &= 0x07;

    uint8_t command = 0x80 | (ch << 4);

    digitalWrite(ADC_CS_PIN, LOW);
    SPI.beginTransaction(adcSpiSettings);

    uint8_t r0 = SPI.transfer(0x01);      // start bit
    uint8_t r1 = SPI.transfer(command);   // single-ended, channel
    uint8_t r2 = SPI.transfer(0x00);      // dummy，读数据

    SPI.endTransaction();
    digitalWrite(ADC_CS_PIN, HIGH);

    uint16_t val = ((r1 & 0x03) << 8) | r2;

    Serial.print("ADC raw bytes: ");
    Serial.print(r0); Serial.print(" ");
    Serial.print(r1); Serial.print(" ");
    Serial.println(r2);

    return val;
}

float logMap(uint16_t val, uint16_t vmin = 0, uint16_t vmax = 1023)
{
    float x = (float)(val - vmin) / (float)(vmax - vmin);
    if (x < 0.0f) x = 0.0f;
    if (x > 1.0f) x = 1.0f;

    if (x < DEADZONE) {
        return 0.0f;
    }

    x = (x - DEADZONE) / (1.0f - DEADZONE);

    float ratio = TIME_WAIT_MAX / TIME_WAIT_MIN;
    float result = TIME_WAIT_MIN * powf(ratio, x);

    return result;
}

void setup()
{
    Serial.begin(115200);
    while (!Serial) {
        ; // 等串口连接（USB CDC）
    }

    Serial.println("SAMD21 stepper control starting...");

    // Initiation GPIO
    for (uint8_t i = 0; i < 3; ++i) {
        pinMode(PUL_PINS[i], OUTPUT);
        digitalWrite(PUL_PINS[i], LOW);
    }

    pinMode(DIR_PIN, OUTPUT);
    digitalWrite(DIR_PIN, HIGH);

    // Initiation SPI
    pinMode(ADC_CS_PIN, OUTPUT);
    digitalWrite(ADC_CS_PIN, HIGH);
}

void loop()
{
    static uint32_t i = 0;

    uint8_t pulIndex = i % 3;
    uint8_t pulPin   = PUL_PINS[pulIndex];

    Serial.print("TOUR (half turn steps): ");
    Serial.println(HALF_TURN_STEPS);

    for (uint16_t step = 0; step < HALF_TURN_STEPS; ++step) {
        digitalWrite(pulPin, HIGH);      // front montant
        delayMicroseconds(PULSE_US);
        digitalWrite(pulPin, LOW);       // front descendant
        delayMicroseconds(PULSE_US);
    }

    uint16_t raw = readAdc(0);
    Serial.print("ADC value: ");
    Serial.println(raw);

    float timeWait = logMap(raw);
    Serial.print("Time wait (s): ");
    Serial.println(timeWait, 4);

    float rotationTime = (PULSE_US * 1e-6f) * HALF_TURN_STEPS * 2.0f;
    Serial.print("Rotation time (s): ");
    Serial.println(rotationTime, 6);

    if (timeWait > 0.0f) {
        delay((unsigned long)(timeWait * 1000.0f));// milli-second
    }
    ++i;
}
