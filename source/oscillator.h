#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <stdbool.h>
#include <stdint.h>

// Wavetable callback fn type
typedef int16_t (*WtCallback)(float, int32_t);

// Oscillator
typedef struct {
    bool enabled;
    float level;
    WtCallback play;
} Oscillator;

// Wavetable algos
int16_t sin_wave(const float phase, const int32_t amp);
int16_t triangle_wave(const float phase, const int32_t amp);

#endif // OSCILLATOR_H
