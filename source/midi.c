#include "midi.h"

#include <math.h>

float freq_from_midi(const uint8_t note) {
    return 440.0f * powf(2.0f, (note - 69) / 12.0f);
}

float freq_with_cents(const float base_freq, const int8_t cents) {
    return base_freq * powf(2.0f, cents / 1200.0f);
}
