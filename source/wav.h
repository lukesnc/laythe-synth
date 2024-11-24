// Copyright (c) 2024 Luke Simone
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef WAV_H
#define WAV_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Lib context
static uint16_t channels = 1;
static uint32_t sample_rate = 8000;
static uint16_t bit_depth = 16;

#pragma pack(push, 1)
typedef struct {
  char riff[4];
  uint32_t file_size;
  char description[4];
  char fmt[4];
  uint32_t chunk_size;
  uint16_t format;
  uint16_t channels;
  uint32_t sample_rate;
  uint32_t bytes_sec;
  uint16_t bytes_samp;
  uint16_t bits_samp;
  char data_header[4];
  uint32_t data_size;
} WavHeader;
#pragma pack(pop)

static inline bool is_valid_bit_depth(const uint16_t n) {
  return n == 8 || n == 16 || n == 24 || n == 32;
}

// Not providing support for higher channel number than 2 right now as it breaks
// write_wav_sample()
static inline bool is_valid_num_channels(const uint16_t n) {
  return n == 1 || n == 2;
}

static inline bool is_valid_sample_rate(const uint32_t n) { return n > 0; }

// Do this first to set global options
bool wav_init(const uint16_t __channels, const uint32_t __sample_rate,
              const uint16_t __bit_depth) {
  if (!is_valid_num_channels(__channels)) {
    fprintf(stderr, "wav_init error: number of channels should be 1 or 2\n");
    return false;
  }
  if (!is_valid_sample_rate(__sample_rate)) {
    fprintf(stderr,
            "wav_init error: sample rate must be between 1 Hz and 4.3 GHz\n");
    return false;
  }
  if (!is_valid_bit_depth(__bit_depth)) {
    fprintf(stderr,
            "wav_init error: possible bit depth values are 8,16,24,32\n");
    return false;
  }

  channels = __channels;
  sample_rate = __sample_rate;
  bit_depth = __bit_depth;

  return true;
}

// Write one audio sample into buffer[pos]
void write_wav_sample(uint8_t *buffer, const uint32_t pos,
                      const int32_t sample) {
  switch (bit_depth) {
  case 32:
    switch (channels) {
    case 2:
      // Left channel
      buffer[pos * 8] = (sample & 0xFF);           // Low byte
      buffer[pos * 8 + 1] = (sample >> 8) & 0xFF;  // Second byte
      buffer[pos * 8 + 2] = (sample >> 16) & 0xFF; // Third byte
      buffer[pos * 8 + 3] = (sample >> 24) & 0xFF; // High byte

      // Right channel
      buffer[pos * 8 + 4] = (sample & 0xFF);       // Low byte
      buffer[pos * 8 + 5] = (sample >> 8) & 0xFF;  // Second byte
      buffer[pos * 8 + 6] = (sample >> 16) & 0xFF; // Third byte
      buffer[pos * 8 + 7] = (sample >> 24) & 0xFF; // High byte
      break;
    case 1:
      buffer[pos * 4] = (sample & 0xFF);           // Low byte
      buffer[pos * 4 + 1] = (sample >> 8) & 0xFF;  // Second byte
      buffer[pos * 4 + 2] = (sample >> 16) & 0xFF; // Third byte
      buffer[pos * 4 + 3] = (sample >> 24) & 0xFF; // High byte
      break;
    }
    break;
  case 24:
    switch (channels) {
    case 2:
      // Left channel
      buffer[pos * 6] = (sample & 0xFF);           // Low byte
      buffer[pos * 6 + 1] = (sample >> 8) & 0xFF;  // Middle byte
      buffer[pos * 6 + 2] = (sample >> 16) & 0xFF; // High byte

      // Right channel
      buffer[pos * 6 + 3] = (sample & 0xFF);       // Low byte
      buffer[pos * 6 + 4] = (sample >> 8) & 0xFF;  // Middle byte
      buffer[pos * 6 + 5] = (sample >> 16) & 0xFF; // High byte
      break;
    case 1:
      buffer[pos * 3] = (sample & 0xFF);           // Low byte
      buffer[pos * 3 + 1] = (sample >> 8) & 0xFF;  // Middle byte
      buffer[pos * 3 + 2] = (sample >> 16) & 0xFF; // High byte
      break;
    }
    break;
  case 16:
    switch (channels) {
    case 2:
      // Left channel
      buffer[pos * 4] = (sample & 0xFF);          // Low byte
      buffer[pos * 4 + 1] = (sample >> 8) & 0xFF; // High byte

      // Right channel
      buffer[pos * 4 + 2] = (sample & 0xFF);      // Low byte
      buffer[pos * 4 + 3] = (sample >> 8) & 0xFF; // High byte
      break;
    case 1:
      buffer[pos * 2] = (sample & 0xFF);          // Low byte
      buffer[pos * 2 + 1] = (sample >> 8) & 0xFF; // High byte
      break;
    }
    break;
  case 8:
    switch (channels) {
    case 2:
      buffer[pos * 2] = (int8_t)sample;     // Left channel
      buffer[pos * 2 + 1] = (int8_t)sample; // Right channel
      break;
    case 1:
      buffer[pos] = (int8_t)sample; // Single channel (mono)
      break;
    }
    break;
  }
}

void build_wav_header(WavHeader *header, const uint32_t samples) {
  memcpy(header->riff, "RIFF", 4);
  memcpy(header->description, "WAVE", 4);
  memcpy(header->fmt, "fmt ", 4);
  header->chunk_size = 16;
  header->format = 1;
  header->channels = channels;
  header->sample_rate = sample_rate;
  header->bytes_sec = sample_rate * channels * bit_depth / 8;
  header->bytes_samp = bit_depth / 8 * channels;
  header->bits_samp = bit_depth;
  memcpy(header->data_header, "data", 4);
  header->data_size = samples * header->bytes_samp;
  header->file_size = header->data_size + sizeof(WavHeader);
}

// Save audio buffer to {filename}.wav given buffer length in samples
bool write_wav_file(const char *filename, const uint8_t *buffer,
                    const uint32_t samples) {
  // Build header
  WavHeader header;
  build_wav_header(&header, samples);

  // Write file
  FILE *f = fopen(filename, "wb");
  if (f == NULL) {
    perror("error opening file");
    return false;
  }

  fwrite(&header, 1, sizeof(WavHeader), f);
  fwrite(buffer, 1, header.data_size, f);
  fclose(f);
  printf("Wrote %.2f MB to %s\n", (double)header.data_size / (1024 * 1024),
         filename);
  return true;
}

uint32_t samples_from_seconds(const uint32_t seconds) {
  return seconds * sample_rate;
}

size_t bytes_sample() { return bit_depth / 8 * channels; }

size_t bytes_from_seconds(const uint32_t seconds) {
  return seconds * sample_rate * (bit_depth / 8 * channels);
}

size_t bytes_from_samples(const uint32_t samples) {
  return samples * (bit_depth / 8 * channels);
}

#endif // WAV_H
