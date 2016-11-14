# Visualizer Demo for Davis 240B Neuromorphic Camera

This program provides a simple visualizer for the asynchronous output of polarity events from the Davis 240B neuromorphic camera

## Requirements

- [libcaer][1]
- cmake >= 2.6
- gcc >= 4.9 or clang >= 3.6
- libusb >= 1.0.17
- [OpenCV 3.1.0][2]

[1]: https://github.com/inilabs/libcaer
[2]: http://opencv.org/

## Installation

Provided makefile is currently configured with the assumption pkg-config is installed and the path to opencv.pc is included in your PKG_CONFIG_PATH

## Usage

Once you have all the dependencies installed and correct path variables set, compile this program using 'make' with the provided makefile.  Run the generated 'main' executable with the Davis 240B camera connected.  Issues initializing program may be solved by running the executable as root.

## Credits

Cody Barnson

## License

    Visualizer Demo for Davis 240B Neuromorphic Camera
    Copyright (C) 2016  Cody Barnson

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.