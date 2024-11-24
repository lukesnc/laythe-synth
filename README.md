# Laythe Synthesizer

Laythe is a dual oscillator standalone polyphonic synthesizer, up to 8 voices.

## Features

Layhte uses a simple UI to control paramaters such as enabling/disabling oscillators, selecting wavetables, changing oscillator volume, and more. Layhte can be controlled with a USB MIDI controller or your computer's keyboard (by passing the `--keyboard` option).

### Recording

Laythe can toggle recording the current session by pressing R. Files are saved in WAV format to `recordings/` in this directory.

## Build and Run

Clone this repositroy and retrieve the following dependencies:

- [raylib](https://github.com/raysan5/raylib)

To cross compile for Windows from WSL:

```bash
make WSL= && ./laythe.exe [--dev /dev/<midi_controller>] [--keyboard]
```

To compile natively for Linux:

```bash
make && ./laythe [--dev /dev/<midi_controller>] [--keyboard]
```
