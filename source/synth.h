#ifndef SYNTH_H
#define SYNTH_H

#include <stdbool.h>
#include <stdint.h>

// Wavetable callback fn type
typedef int16_t (*WtCallback)(float, int32_t);

// Oscillator
typedef struct Oscillator {
    bool enabled;
    float level;
    int8_t fine_tune;
    char *wt_name;
    WtCallback play;
} Oscillator;

void cycle_wavetable(Oscillator *osc);

// Wavetable algorithms
int16_t sine_wave(const float phase, const int32_t amp);
int16_t triangle_wave(const float phase, const int32_t amp);
int16_t saw_wave(const float phase, const int32_t amp);
int16_t square_wave(const float phase, const int32_t amp);
int16_t pulse_wave(const float phase, const int32_t amp);

// Filters
int16_t lowpass(const int16_t sample, const float cutoff);

// Helper funcs
float freq_from_midi(const uint8_t note);
float freq_with_cents(const float base_freq, const int8_t cents);

#endif // SYNTH_H
