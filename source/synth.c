#include "synth.h"

#include <math.h>

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

int16_t lowpass(const int16_t sample, const float cutoff) {
    static int16_t prev = 0;
    const int16_t y = (sample + prev) / 2;
    prev = sample;
    return y;
}

/*int32_t envelope(const float attack, const float decay, const float sustain,*/
/*                 const float release) {}*/

// Envelope generators
// TODO
