#ifndef MIDI_H
#define MIDI_H

#include <stdint.h>

float freq_from_midi(const uint8_t note);
float freq_with_cents(const float base_freq, const int8_t cents);

#endif // MIDI_H
