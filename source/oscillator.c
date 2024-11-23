#include "oscillator.h"
#include <math.h>

// Wavetable algos
int16_t sin_wave(const float phase, const int32_t amp) {
    return (int16_t)(amp * sinf(2.0f * M_PI * phase));
}

int16_t triangle_wave(const float phase, const int32_t amp) {
    const float t = 2.0f * fabs(2.0f * (phase - floorf(phase + 0.5f))) - 1.0f;
    return (int16_t)(amp * t);
}

int32_t envelope(const float attack, const float decay, const float sustain,
                 const float release) {}

// Envelope generators
// TODO
