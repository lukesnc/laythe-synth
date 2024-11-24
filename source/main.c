#include <fcntl.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "raylib.h"

#include "synth.h"
#include "wav.h"

#define SAMPLE_RATE (44100)
#define CHANNELS (1)
#define BIT_DEPTH (16)
#define MAX_VOICES (8)

// Current notes & midi
static uint8_t active_notes[MAX_VOICES] = {0};
static uint8_t active_note_count = 0;
static int8_t octave_shift = 0;

float freq_from_midi(const uint8_t note) {
    return 440.0f * powf(2.0f, (note - 69) / 12.0f);
}

float freq_with_cents(const float base_freq, const int8_t cents) {
    return base_freq * powf(2.0f, cents / 1200.0f);
}

// Recording
#define MAX_RECORDING_SIZE (120 * SAMPLE_RATE * (BIT_DEPTH / 8 * CHANNELS))

static bool recording = false;

void record_sample(const int16_t sample) {
    static uint8_t rec_buffer[MAX_RECORDING_SIZE];
    static uint32_t rec_playhead = 0;

    if (recording) {
        write_wav_sample(rec_buffer, rec_playhead, sample);
        rec_playhead++;
    } else {
        if (rec_playhead > 0) {
            // Make recordings dir
            struct stat st = {0};
            if (stat("recordings", &st) == -1) {
#ifdef _WIN32
                mkdir("recordings");
#else
                mkdir("recordings", 0700);
#endif
            }

            char filename[40];
            const struct tm *timenow;
            const time_t now = time(NULL);
            timenow = gmtime(&now);
            strftime(filename, sizeof(filename),
                     "recordings/laythe-%y%m%d_%H%M.wav", timenow);

            write_wav_file(filename, rec_buffer, rec_playhead);
            rec_playhead = 0;
        }
    }
}

// Audio stream callback
#define NUM_OSCILLATORS (2)

static Oscillator oscs[NUM_OSCILLATORS] = {
    {true, 1.0f, 0, "triangle", triangle_wave},
    {false, 1.0f, 0, "sine", sine_wave},
};

#define FILTER_CUTOFF_MAX (8000)

static bool filtering = false;
static float filter_cutoff = 1000.0f;

void audio_callback(void *buffer, uint32_t frames) {
    // Track phase for each voice
    static float phases[MAX_VOICES] = {0.0f};

    // TODO: stop using global amplitude
    const float amp = 10000.0f;

    // Put data in stream
    int16_t *d = (int16_t *)buffer;
    for (uint32_t frame = 0; frame < frames; frame++) {
        int32_t sample = 0;

        // Sum all active notes on all oscillators
        for (size_t n = 0; n < active_note_count; n++) {
            float freq = freq_from_midi(active_notes[n]);
            freq += (0.01f * n); // Apply slight detune for phasing

            for (size_t o = 0; o < NUM_OSCILLATORS; o++) {
                if (oscs[o].enabled) {
                    // FIXME: freq change shouldnt be affecting all voices
                    freq = freq_with_cents(freq, oscs[o].fine_tune);
                    sample += oscs[o].play(phases[n], amp * oscs[o].level);
                }
            }

            // Update phase
            phases[n] += freq / SAMPLE_RATE;
            if (phases[n] > 1.0f)
                phases[n] -= 1.0f;
        }

        if (filtering) {
            sample = lowpass(sample, filter_cutoff);
        }

        // Check bounds of signed 16-bit sample
        if (sample > INT16_MAX)
            sample = INT16_MAX;
        if (sample < INT16_MIN)
            sample = INT16_MIN;

        // Write to stream
        d[frame] = (int16_t)sample;

        // Check if recording
        record_sample((int16_t)sample);
    }
}

int main(int argc, char *argv[]) {
    // Parse args
    bool use_keyboard = false;
    for (int32_t i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--keyboard") == 0) {
            use_keyboard = 1;
        } else {
            fprintf(stderr, "usage: %s [--keyboard]\n", argv[0]);
            return 1;
        }
    }

    // Init midi
    if (!use_keyboard) {
    }

    // Raylib window init
    InitWindow(360, 240, "Laythe");
    SetTargetFPS(60);

    // Raylib audio init
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(4096);
    AudioStream stream = LoadAudioStream(SAMPLE_RATE, BIT_DEPTH, CHANNELS);
    SetAudioStreamCallback(stream, audio_callback);
    PlayAudioStream(stream);

    // Init wav lib (for recording)
    if (!wav_init(CHANNELS, SAMPLE_RATE, BIT_DEPTH))
        return 1;

    // Event loop
    while (!WindowShouldClose()) {
        // Get current notes (either keyboard or midi)
        if (use_keyboard) {
            for (uint8_t key = 0; key < 255; key++) {
                uint8_t note = 0;
                switch (key) { // clang-format off
                    case KEY_A: note = 50; break;
                    case KEY_W: note = 51; break;
                    case KEY_S: note = 52; break;
                    case KEY_E: note = 53; break;
                    case KEY_D: note = 54; break;
                    case KEY_F: note = 55; break;
                    case KEY_T: note = 56; break;
                    case KEY_G: note = 57; break;
                    case KEY_Y: note = 58; break;
                    case KEY_H: note = 59; break;
                    case KEY_U: note = 60; break;
                    case KEY_J: note = 61; break;
                    case KEY_K: note = 62; break;
                    case KEY_O: note = 63; break;
                    case KEY_L: note = 64; break;
                    case KEY_P: note = 65; break;
                    case KEY_SEMICOLON: note = 66; break;
                    default: continue;
                } // clang-format on

                note = note + (12 * octave_shift);

                // Add all depressed keys to active notes
                if (IsKeyDown(key)) {
                    for (uint8_t i = 0; i < MAX_VOICES; i++) {
                        // Note already on
                        if (active_notes[i] == note)
                            break;

                        if (active_notes[i] == 0) {
                            active_notes[i] = note;
                            active_note_count++;
                            break;
                        }
                    }
                }

                // Clear all avtive notes when any key released
                if (IsKeyReleased(key)) {
                    memset(active_notes, 0, sizeof(active_notes));
                    active_note_count = 0;
                }
            }
        } else {
            // Fetch midi notes
        }

        // ---- Draw -----
        BeginDrawing();

        const Vector2 mouse_pos = GetMousePosition();

        // Draw oscillator controls
        uint32_t posY = 10;
        for (uint32_t i = 0; i < NUM_OSCILLATORS; i++) {
            // Oscillator name
            DrawRectangle(10, posY, 60, 35, WHITE);
            char name[5] = "osc";
            char idx[2];
            sprintf(idx, "%d", i);
            strcat(name, idx);
            DrawText(name, 15, posY + 5, 20, BLACK);

            // Enabled button
            const Rectangle osc_enabled_rec = {80, posY, 35, 35};
            DrawRectangleRec(osc_enabled_rec, oscs[i].enabled ? GREEN : RED);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                CheckCollisionPointRec(mouse_pos, osc_enabled_rec)) {
                oscs[i].enabled = !oscs[i].enabled;
            }

            // Wavetable name
            const Rectangle osc_wavetable_rec = {125, posY, 100, 35};
            DrawRectangleRec(osc_wavetable_rec, WHITE);
            DrawText(oscs[i].wt_name, 130, posY + 5, 20, BLACK);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                CheckCollisionPointRec(mouse_pos, osc_wavetable_rec)) {
                cycle_wavetable(&oscs[i]);
            }

            // Oscillator level
            const Rectangle osc_level_rec = {235, posY, 50, 35};
            DrawRectangleRec(osc_level_rec, WHITE);
            char level[10];
            sprintf(level, "%0.2f", oscs[i].level);
            DrawText(level, 240, posY + 5, 20, BLACK);
            if (CheckCollisionPointRec(mouse_pos, osc_level_rec)) {
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    oscs[i].level += 0.01;
                    if (oscs[i].level > 1.0f)
                        oscs[i].level -= 1.0f;
                } else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                    // Reset with right click
                    oscs[i].level = 1.0f;
                }
            }

            // Oscillator fine tune
            const Rectangle osc_fine_rec = {295, posY, 50, 35};
            DrawRectangleRec(osc_fine_rec, WHITE);
            char fine[10];
            sprintf(fine, "%d", oscs[i].fine_tune);
            DrawText(fine, 300, posY + 5, 20, BLACK);
            if (CheckCollisionPointRec(mouse_pos, osc_fine_rec)) {
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    if (!(oscs[i].fine_tune >= 100))
                        oscs[i].fine_tune += 1;
                } else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                    if (!(oscs[i].fine_tune <= -100))
                        oscs[i].fine_tune -= 1;
                }
            }

            posY += 40;
        }

        // Draw recording button
        const Rectangle record_button_rec = {10, 105, 35, 35};
        DrawRectangleRec(record_button_rec, recording ? RED : MAROON);
        DrawText("R", 15, 110, 20, BLACK);

        // Toggle recording with R or on-screen button
        if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
             CheckCollisionPointRec(mouse_pos, record_button_rec)) ||
            IsKeyPressed(KEY_R)) {
            recording = !recording;
            printf(recording ? "Recording started\n" : "Recording stopped\n");
        }

        // Draw octave + / - buttons
        DrawRectangle(50, 105, 45, 35, WHITE);
        DrawText("oct", 55, 110, 20, BLACK);
        const Rectangle octave_plus_rec = {100, 105, 35, 35};
        DrawRectangleRec(octave_plus_rec, WHITE);
        DrawText("+", 105, 110, 20, BLACK);
        const Rectangle octave_minus_rec = {140, 105, 35, 35};
        DrawRectangleRec(octave_minus_rec, WHITE);
        DrawText("-", 145, 110, 20, BLACK);

        // Check octave shift
        if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
             CheckCollisionPointRec(mouse_pos, octave_minus_rec)) ||
            IsKeyPressed(KEY_MINUS)) {
            octave_shift--;
        } else if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                    CheckCollisionPointRec(mouse_pos, octave_plus_rec)) ||
                   IsKeyPressed(KEY_EQUAL)) {
            octave_shift++;
        }

        // Draw filter controls
        const Rectangle filter_rec = {10, 160, 100, 35};
        DrawRectangleRec(filter_rec, filtering ? YELLOW : ORANGE);
        DrawText(filtering ? "filter on" : "filter off", 15, 165, 20, BLACK);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            CheckCollisionPointRec(mouse_pos, filter_rec)) {
            filtering = !filtering;
        }

        const Rectangle filter_cutoff_rec = {120, 160, 100, 35};
        DrawRectangleRec(filter_cutoff_rec, WHITE);
        char cutoff[10];
        sprintf(cutoff, "%d Hz", (uint32_t)filter_cutoff);
        DrawText(cutoff, 125, 165, 20, BLACK);
        if (CheckCollisionPointRec(mouse_pos, filter_cutoff_rec)) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                if (!(filter_cutoff >= FILTER_CUTOFF_MAX)) {
                    filter_cutoff += 50;
                }
            } else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                if (!(filter_cutoff <= 0)) {
                    filter_cutoff -= 50;
                }
            }
        }

        EndDrawing();
    }

    // Cleanup
    if (recording) {
        recording = false;
        record_sample(0);
    }

    UnloadAudioStream(stream);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
