# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Rompler_2.0 is a JUCE-based VST3/AU/Standalone audio plugin (rompler/sampler synth) by Recluse Audio. It loads WAV files and plays them via MIDI input with ADSR envelope control. Built with C++20 and CMake 3.24.1+, targeting Windows (VST3), macOS (VST3 + AU), and Linux.

- **Plugin formats**: VST3 (Windows/macOS), AU (macOS), Standalone
- **Audio I/O**: Stereo output, MIDI input, 3 polyphonic voices
- **Bundle ID**: `com.recluse-audio.rompler` | **Plugin Code**: `Romp`

## Build Commands

### Using Python Helper Scripts (recommended)

```bash
python HELPER_SCRIPTS/build_vst3.py [--config Debug|Release]
python HELPER_SCRIPTS/build_standalone.py [--config Debug|Release]
python HELPER_SCRIPTS/build_au.py [--config Debug|Release]          # macOS only
python HELPER_SCRIPTS/rebuild_all.py --clean --config Release       # full clean rebuild
```

Scripts auto-detect CMake, call `regenSource.py` before building, and play audio feedback on Windows.

### Manual CMake

**Windows:**
```bash
cmake -S . -B BUILD
cmake --build BUILD --target Rompler_VST3 --config Release
cmake --build BUILD --target Rompler_All --config Release
```

**macOS x86_64:**
```bash
cmake -S . -B BUILD -G Xcode -DCMAKE_OSX_ARCHITECTURES=x86_64
cmake --build BUILD --target Rompler_All --config Release
```

**macOS ARM64:**
```bash
cmake -S . -B BUILD -G Xcode -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build BUILD --target Rompler_All --config Release
```

Build artifacts land in `BUILD/Rompler_artefacts/{Debug,Release}/{VST3,AU,Standalone}/`.

## Tests

Tests use Catch2 v3.1.0 (auto-fetched via FetchContent when `BUILD_TESTS=ON`).

```bash
python HELPER_SCRIPTS/build_tests.py [--config Debug|Release]
```

Or manually:
```bash
cmake -S . -B BUILD -DBUILD_TESTS=ON
cmake --build BUILD --target Tests --config Release
./BUILD/Tests
```

Test files are in `TESTS/` — currently covering plugin name, MIDI capabilities, editor presence, parameter ranges, and ADSR parameter existence.

## Source Management

**When adding or removing `.cpp`/`.h` files**, run `regenSource.py` to regenerate `CMAKE/SOURCES.cmake` and `CMAKE/TESTS.cmake`:
```bash
python HELPER_SCRIPTS/regenSource.py
```

**To bump the version** (increments patch in `VERSION.txt` and regenerates `SOURCE/Util/Version.h`):
```bash
python HELPER_SCRIPTS/update_version.py
```

## Architecture

### Signal Flow
MIDI input → `RomplerAudioProcessor::processBlock()` → JUCE `Synthesiser` → `RomplerVoice::renderNextBlock()` (reads WAV sample + applies pitch shift + ADSR envelope) → stereo output buffer

### Key Classes

| Class | File | Role |
|---|---|---|
| `RomplerAudioProcessor` | `SOURCE/Processor/PluginProcessor` | Main `AudioProcessor`; owns the JUCE `Synthesiser` (3 voices), APVTS with ADSR params, file loading, state save/restore |
| `RomplerVoice` | `SOURCE/Processor/PluginProcessor` | Renders individual sampled notes with pitch-shifting via resampling and ADSR envelope |
| `RomplerSound` | `SOURCE/Processor/RomplerSound.h` | Sample data container; stores loaded WAV buffer and MIDI note mapping |
| `RomplerAudioProcessorEditor` | `SOURCE/Components/PluginEditor` | Main UI window; hosts all UI components |
| `ADSRComponent` | `SOURCE/Components/ADSRComponent` | Attack/Decay/Sustain/Release sliders bound to APVTS |
| `WaveThumbnail` | `SOURCE/Components/WaveThumbnail` | Waveform display with playhead; handles drag-and-drop WAV file loading |
| `RompMenu` | `SOURCE/Components/RompMenu` | Sample browser using `Database` to navigate artist → category → sample |
| `Database` | `SOURCE/ROMPLE_DATA/Database.h` | Scans `C:/ProgramData/Recluse-Audio/Rompler/` for artist/category/sample structure |
| `PulsarUIFeel` | `SOURCE/LookNFeel/PulsarUIFeel` | Custom `LookAndFeel` for consistent plugin theming |

### Parameters (APVTS)
ATTACK, DECAY, SUSTAIN, RELEASE — all normalized 0.0–1.0, defined in `PluginProcessor::createParameters()`.

### Sample Data Path
The `Database` class hard-codes `C:/ProgramData/Recluse-Audio/Rompler/` as the root for sample libraries. This path is Windows-specific and may need adjustment for macOS/Linux builds.

### Dependencies
- **JUCE** — submodule at `SUBMODULES/JUCE`; modules used: `juce_audio_utils`, `juce_audio_processors`
- **Catch2 v3.1.0** — fetched at configure time when `BUILD_TESTS=ON`
- **Assets** — `ASSETS/LOGO_SVG.svg` and `ASSETS/Spyder_Sound.svg` embedded via `juce_add_binary_data`
