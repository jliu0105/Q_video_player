# QVideoPlayer: Customizable Video Player
QVideoPlayer is a versatile video player that allows users to play and pause videos, adjust playback speed, and more. Developed using C++, Qt, QML, and FFmpeg, the application leverages QString, QDebug, QScopedPointer, and QThread for module development and UI building. FFmpeg is utilized for decoding, multiplexing, and resizing video and audio streams, as well as applying video filters. To prevent the GUI thread from being blocked, heavy computations are executed in a separate thread.
## Table of Contents
[Features](#features) <br>
[Requirement](#requirement)<br>
[Installation](#requirement)<br>
[Usage](#usage)<br>
[Contribution](#contribution)<br>


## Features
* __Video Playback:__ Play and pause video files with ease.
* __Playback Speed Control:__ Adjust the playback speed according to your preference.
* __FFmpeg Integration:__ Decode, multiplex, and resize video and audio streams, and apply video filters.
* __Separate Computation Thread:__ Prevent the GUI thread from being blocked by running heavy computations in a separate thread.
* __QML-based UI:__ Modern and intuitive user interface built with QML.

## Requirement
* C++ Compiler (e.g., GCC, Clang)
* Qt 5.15 or higher
* QML
* FFmpeg

## Installation

### clone the repo
```bash
git clone https://github.com/jliu0105/Q_video_player.git
```

### Create a build directory and navigate to it:
```bash
mkdir build && cd build
```

### Run qmake to generate a Makefile:
```bash
qmake ../QVideoPlayer.pro
```
### Build the project
```bash
make
```

## Usage
* Open a video file by clicking the "Open" button or using the menu.
* Use the playback controls to play, pause, or stop the video.
* Adjust the playback speed using the speed control slider.
* Use the seek bar to navigate through the video.

## Contribution
We welcome contributions from the community! If you're interested in contributing to this project, please follow these steps:

1. Fork the repository
2. Create a new branch for your changes
3. Commit your changes and push them to your fork
4. Create a pull request for your changes
