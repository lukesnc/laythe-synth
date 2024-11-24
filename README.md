# Laythe Synthesizer

Laythe is a dual oscillator standalone polyphonic synthesizer, up to 8 voices.

## Features

Layhte uses a simple UI to control paramaters such as enabling/disabling oscillators, selecting wavetables, changing oscillator volume, and more. Layhte can be controlled with a USB MIDI controller or your computer's keyboard (by passing the `--keyboard` option).

### Recording

Laythe can toggle recording the current session by pressing R. Files are saved in WAV format to `recordings/` in this directory.

## Build and Run

Retrieve the following dependencies:

- [raylib](https://github.com/raysan5/raylib)

Clone this repository, then

```bash
make && ./laythe [--dev /dev/<midi_controller>] [--keyboard]
```
