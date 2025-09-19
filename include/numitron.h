#ifndef _NUMITRON_H_
#define _NUMITRON_H_

#include <Arduino.h>

#define NUMITRON_DATA_PIN 13
#define NUMITRON_CLOCK_PIN 14
#define NUMITRON_LATCH_PIN 16

#define NUMITRON_PWM_PIN 15

#define NUMITRON_DIGIT_COUNT 6

#define SEGMENT_A 0x80
#define SEGMENT_B 0x01
#define SEGMENT_C 0x20
#define SEGMENT_D 0x10
#define SEGMENT_E 0x08
#define SEGMENT_F 0x04
#define SEGMENT_G 0x02

const uint8_t DIG_SEG[10] = {
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F,             // 0
    SEGMENT_B | SEGMENT_C,                                                             // 1
    SEGMENT_A | SEGMENT_B | SEGMENT_D | SEGMENT_E | SEGMENT_G,                         // 2
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_G,                         // 3
    SEGMENT_B | SEGMENT_C | SEGMENT_F | SEGMENT_G,                                     // 4
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G,                         // 5
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G,             // 6
    SEGMENT_A | SEGMENT_B | SEGMENT_C,                                                 // 7
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G, // 8
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G              // 9
};

void numitron_init();
void numitron_setBrightness(int brightness);
void numitron_display(const uint8_t *digs);

#endif