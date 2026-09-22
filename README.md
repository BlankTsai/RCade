# RCade

> A four-lane rhythm game built with **C++ and raylib**, featuring original music, BPM-driven note generation, timing-based judgment, and an ongoing performance profiling and optimization workflow.

RCade started as a university course project and has since been continuously refined as a personal software engineering project.
Beyond implementing a playable rhythm game, the project explores real-time input handling, music synchronization, game-state management, resource lifecycle management, performance measurement, and reproducible development practices.

---

## Overview

RCade is a desktop rhythm game developed independently using **C++** and **raylib**.

Players use the `A`, `S`, `D`, and `F` keys to interact with four lanes while notes move toward a judgment line according to the timing of the selected song.

The game currently contains five original tracks and includes a complete gameplay flow from the opening sequence to song selection, gameplay, scoring, and the final result screen.

### Main Features

* Four-lane rhythm gameplay
* `A / S / D / F` keyboard controls
* Five playable songs with different BPM values
* BPM-based note generation
* Timing-based `Perfect`, `Good`, `Miss`, and `Bad` judgments
* Real-time score calculation
* Music streaming through raylib
* Animated intro, menu, transitions, countdown, and result screen
* Command-line debug logging
* Frame-time profiling and CSV export
* Structured source modules for gameplay, rendering, input, audio, and song data

---

## Gameplay Flow

```text
Intro
  ↓
Main Menu
  ↓
Song Selection
  ↓
3 - 2 - 1 - GO
  ↓
Gameplay
  ↓
Score Screen
  ↓
Main Menu
```

During gameplay, notes are generated according to each song's BPM and travel toward the judgment line.

The game compares the player's input time with each note's target time and determines the corresponding judgment.

```text
Input
  ↓
Find nearest valid note in the selected lane
  ↓
Calculate timing difference
  ↓
Perfect / Good / Bad
```

Notes that pass the valid timing window without being hit are counted as `Miss`.

---

## Technical Architecture

The project separates major responsibilities into individual modules instead of implementing the entire game inside a single main loop.

```text
RCade
├── Game
│   ├── Game state
│   ├── Scene lifecycle
│   ├── Countdown
│   └── Game completion
│
├── Block
│   ├── Note generation
│   ├── Position calculation
│   └── Miss detection
│
├── Input
│   ├── Keyboard input
│   ├── Lane matching
│   └── Timing judgment
│
├── Audio
│   ├── Music loading
│   ├── Streaming
│   └── Resource lifecycle
│
├── Render
│   ├── Gameplay rendering
│   ├── Scene transitions
│   ├── UI
│   └── Visual feedback
│
└── Song Database
    ├── Song metadata
    ├── BPM
    └── Audio paths
```

This structure makes individual systems easier to analyze, modify, and optimize independently.

---

## Rhythm and Timing System

Each song stores metadata including its:

* Name
* BPM
* Audio file path
* Timing offset

The basic interval between notes is calculated from BPM:

```text
Beat Interval = 60 / BPM
```

Instead of placing a note directly at the judgment line, RCade calculates when the note needs to appear based on its target judgment time and travel time.

```text
Travel Time = Judge Line Position / Block Speed

Spawn Time = Target Time - Travel Time
```

The note position is then calculated continuously from elapsed time rather than relying solely on frame-by-frame position increments.

This allows note movement to remain based on game time and provides a foundation for further audio synchronization and latency calibration.

---

## Timing Judgment

For each key press, RCade searches for the closest active note in the corresponding lane.

The absolute difference between the current playback-relative time and the note's target time is used for judgment.

```text
| Current Time - Target Time |
```

Depending on the timing difference, the input is classified as:

* **Perfect**
* **Good**
* **Bad**
* **Miss**

This implementation separates note position from hit judgment and makes timing-window parameters independently adjustable.

---

## Audio System

RCade uses raylib's music streaming API for background music playback.

The audio lifecycle includes:

1. Initializing the audio device
2. Loading the selected music stream
3. Starting playback after the countdown
4. Updating the stream during gameplay
5. Detecting song completion
6. Stopping and unloading resources when necessary

Additional validity checks are used to avoid operating on unavailable or unloaded audio resources.

---

## Performance Profiling

After the original course project was completed, RCade was extended with a performance profiling workflow to investigate occasional frame-time instability.

Profiling can be enabled through a command-line argument:

```bash
music_game --profile output.csv
```

Detailed logging can also be enabled:

```bash
music_game --verbose
```

Both options can be combined:

```bash
music_game --verbose --profile output.csv
```

During active gameplay, RCade records complete frame durations and exports them to CSV when the program exits normally.

The profiler summarizes:

* Mean frame time
* 95th percentile (`p95`)
* 99th percentile (`p99`)
* Maximum frame time
* Frames exceeding 25 ms
* Frames exceeding 50 ms

Example output format:

```text
Gameplay frames=...
mean_ms=...
p95_ms=...
p99_ms=...
max_ms=...
over25ms=...
over50ms=...
```

### Why Percentiles?

Average frame time alone may hide occasional long frames.

For an interactive rhythm game, isolated latency spikes can affect perceived timing even when the average frame rate appears stable. Therefore, RCade also records percentile values and long-frame counts to provide a more informative view of frame-time consistency.

The current measurements include frame pacing and rendering completion, so they should be interpreted as **complete gameplay frame durations rather than pure CPU or GPU execution time**.

---

## Optimization Workflow

Optimization is treated as a measurement-driven process rather than assuming that a code change improves performance.

The current workflow is:

```text
Identify a potential problem
        ↓
Define reproducible conditions
        ↓
Collect frame-time data
        ↓
Compare distributions and long frames
        ↓
Modify the implementation
        ↓
Measure again
        ↓
Document results and remaining limitations
```

One investigated factor is high-frequency debug logging during gameplay.

RCade therefore supports separate normal and verbose logging modes so their behavior can be compared under similar conditions.

Further experiments and results are documented in:

* [`docs/OPTIMIZATION.md`](docs/OPTIMIZATION.md)
* [`CHANGELOG.md`](CHANGELOG.md)

---

## Project Structure

```text
RCade/
├── include/
│   ├── Audio.h
│   ├── Block.h
│   ├── Constants.h
│   ├── Game.h
│   ├── Input.h
│   ├── Render.h
│   └── SongDatabase.h
│
├── src/
│   ├── main.cpp
│   ├── Audio.cpp
│   ├── Block.cpp
│   ├── Game.cpp
│   ├── Input.cpp
│   ├── Render.cpp
│   └── SongDatabase.cpp
│
├── resources/
│   ├── Music
│   ├── Images
│   └── Sound effects
│
├── docs/
│   └── OPTIMIZATION.md
│
├── ASSETS.md
├── CHANGELOG.md
└── README.md
```

### Main Components

| Component          | Responsibility                                                 |
| ------------------ | -------------------------------------------------------------- |
| `main.cpp`         | Main loop, scene dispatch, profiling and application lifecycle |
| `Game.cpp`         | Gameplay state, countdown, song lifecycle and game completion  |
| `Block.cpp`        | Note spawning, movement and miss detection                     |
| `Input.cpp`        | Keyboard input, lane matching and hit judgment                 |
| `Audio.cpp`        | Music loading and streaming                                    |
| `Render.cpp`       | Gameplay graphics, menus, transitions and visual feedback      |
| `SongDatabase.cpp` | Song metadata and BPM configuration                            |

---

## Build Environment

RCade was developed for Windows using GCC/MinGW and raylib.

The current project has been successfully rebuilt with:

* **GCC 14.2.0**
* **raylib 5.5**
* Windows

A compatible GCC and raylib environment must be configured before compilation.

### Compile

From the project root:

```bash
g++ src/*.cpp -o music_game -I include -lraylib -lopengl32 -lgdi32 -lwinmm
```

Run the executable from the project root so that relative paths under `resources/` can be resolved correctly.

---

## Current Song List

| Song             | BPM |
| ---------------- | --: |
| LoveTheWorld     | 110 |
| Courage Delivery | 125 |
| Re:verse         | 130 |
| Fresh Afternoon  | 130 |
| Ryuko (Hard)     | 180 |

All five tracks included in the project were produced by the author.

---

## Current Limitations

RCade is still being refined and currently has several known limitations:

* Notes are generated from BPM rather than manually authored beatmaps.
* Note lanes are currently assigned randomly.
* Multiple lane inputs occurring within the same frame require further improvement.
* Audio offset, note travel time, and device/input latency are not yet fully separated.
* Performance measurements are preliminary and are not presented as proof that gameplay stuttering has already been eliminated.
* Build and distribution workflows can be further automated.

Documenting these limitations is part of the project's development process and provides clear targets for future experiments and improvements.

---

## Future Work

Planned areas for continued development include:

* Fixed and manually designed beatmaps
* Improved simultaneous multi-key input
* Input and audio latency calibration
* Separation of song offset and note travel timing
* More reproducible performance benchmarks
* Update/render architecture refinement
* Improved resource lifecycle management
* Pause and retry systems
* Game settings
* Improved result statistics
* Reproducible release builds

---

## Engineering Focus

Although RCade began as a game-development course project, continued development has shifted the focus toward broader software engineering topics.

Through this project, I have explored:

* Modular C++ program design
* Real-time application loops
* Time-based simulation
* Audio synchronization
* Interactive input processing
* Resource management
* Performance profiling
* Experimental comparison of software behavior
* Version control and change documentation
* Identifying and documenting limitations rather than assuming optimization results

This process has helped transform RCade from a completed course assignment into an ongoing engineering project in which implementation decisions can be measured, evaluated, and iteratively improved.

---

## Authorship and Assets

RCade is an independently developed project.

* Game programming and system implementation were completed by the author.
* The five included songs were composed and produced by the author.
* Image assets were generated with AI assistance.
* Additional asset information is documented in [`ASSETS.md`](ASSETS.md).
* Post-course AI tools have been used to assist with code analysis, documentation, and optimization planning. Changes and verification results are recorded through Git history and [`CHANGELOG.md`](CHANGELOG.md).

Some asset licensing information is still being reviewed. No open-source license is currently granted for this repository unless explicitly stated otherwise.

---

## Development History

RCade was originally created as a university final project and later revisited for further development.

Rather than replacing the original implementation, the repository preserves its baseline and records subsequent modifications so that changes can be compared against the earlier version.

See [`CHANGELOG.md`](CHANGELOG.md) for detailed development history.

---

## Author

**Blank Tsai**

Music:VioletBK
All Rights Reserved
