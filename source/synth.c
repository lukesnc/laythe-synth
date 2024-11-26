#include "synth.h"

#include <math.h>

float freq_from_midi(const uint8_t note) {
    return 440.0f * powf(2.0f, (note - 69) / 12.0f);
}

float freq_with_cents(const float base_freq, const int8_t cents) {
    return base_freq * powf(2.0f, cents / 1200.0f);
}

void cycle_wavetable(Oscillator *osc) {
    if (osc->play == sine_wave) {
        osc->play = triangle_wave;
        osc->wt_name = "triangle";
    } else if (osc->play == triangle_wave) {
        osc->play = saw_wave;
        osc->wt_name = "saw";
    } else if (osc->play == saw_wave) {
        osc->play = square_wave;
        osc->wt_name = "square";
    } else if (osc->play == square_wave) {
        osc->play = pulse_wave;
        osc->wt_name = "pulse";
    } else {
        osc->play = sine_wave;
        osc->wt_name = "sine";
    }
}

int16_t sine_wave(const float phase, const int32_t amp) {
    const float x = sinf(2.0f * M_PI * phase);
    return (int16_t)(amp * x);
}

int16_t triangle_wave(const float phase, const int32_t amp) {
    const float x = 4.0f * fabs(phase - floorf(phase + 0.75f) + 0.25f) - 1.0f;
    return (int16_t)(amp * x);
}

int16_t saw_wave(const float phase, const int32_t amp) {
    const float x = 2.0f * (phase - floorf(phase + 0.5f));
    return (int16_t)(amp * x);
}

int16_t square_wave(const float phase, const int32_t amp) {
    const float x = 4.0f * floorf(phase) - 2.0f * floorf(2.0f * phase) + 1.0f;
    return (int16_t)(amp * x);
}

int16_t pulse_wave(const float phase, const int32_t amp) {
    const float duty = 0.9f;
    const float x = (fmodf(phase, 1.0f) < duty) ? 1.0f : -1.0f;
    return (int16_t)(amp * x);
}

int16_t lowpass(const int16_t sample, const float cutoff) {
    static float prev = 0.0f;
    const float a = 2.0f * M_PI * cutoff / (44100.0f + 2.0f * M_PI * cutoff);
    const float y = a * sample + (1.0f - a) * prev;
    prev = y;
    return (int16_t)y;
}
