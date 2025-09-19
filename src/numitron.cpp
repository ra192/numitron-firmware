#include <numitron.h>

void numitron_init()
{
    pinMode(NUMITRON_DATA_PIN, OUTPUT);
    pinMode(NUMITRON_CLOCK_PIN, OUTPUT);
    pinMode(NUMITRON_LATCH_PIN, OUTPUT);

    pinMode(NUMITRON_PWM_PIN, OUTPUT);
}


void numitron_setBrightness(int brightness)
{
    analogWrite(NUMITRON_PWM_PIN, brightness);
}

void numitron_display(const uint8_t *digs)
{
    digitalWrite(NUMITRON_LATCH_PIN, LOW);

    for (int i = 0; i < NUMITRON_DIGIT_COUNT; i++)
    {
        uint8_t segments = DIG_SEG[digs[i]];

        for (int j = 0; j < 8; j++)
        {
            // Write the bit to the data pin
            digitalWrite(NUMITRON_DATA_PIN, (segments >> j) & 0x01);

            // Pulse the clock pin
            digitalWrite(NUMITRON_CLOCK_PIN, HIGH);
            digitalWrite(NUMITRON_CLOCK_PIN, LOW);
        }
    }

    digitalWrite(NUMITRON_LATCH_PIN, HIGH);
}