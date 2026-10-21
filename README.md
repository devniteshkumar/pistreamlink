# Stream PC to Pi — Milestone 2

C++20/CMake prototype for the Fedora PC sender and Arch Linux ARM receiver.
The sender emits a GStreamer test pattern over VP8/RTP/UDP, and the receiver
decodes it and displays it through the platform's GStreamer video sink.
VP8 is used for this proof of concept because H.264 plugins are not available
in the current development environment; the pipeline boundary is isolated so
H.264 can be enabled after the target plugins are verified.

## Dependencies

- CMake 3.20+
- A C++20 compiler (GCC 10+ or a recent Clang)
- `make` or another CMake-supported build tool
- GStreamer 1.x runtime with `videotestsrc`, `vp8enc`, `rtpvp8pay`,
  `udpsink`, `udpsrc`, `rtpvp8depay`, `vp8dec`, and a video sink

## Build

Run on either device from the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Both executables are built. Run the sender on the PC:

```sh
./build/pc_sender --host PI_IP_OR_HOSTNAME --port 5000 \
  --width 1920 --height 1080 --fps 60 --bitrate 12000
```

Run the receiver on the Pi:

```sh
./build/pi_receiver --bind 0.0.0.0 --port 5000 \
  --width 1920 --height 1080 --fps 60 --bitrate 12000
```

`--host` is the destination for the PC sender. `--bind` is the local address
the receiver binds to; `0.0.0.0` means all IPv4 interfaces. Both programs
run until GStreamer exits or Ctrl-C is pressed.

## Milestone 2 acceptance checks

- `cmake` configures successfully on both machines.
- `cmake --build build -j` succeeds on both machines.
- The receiver displays the sender's SMPTE test pattern over the existing LAN.
- `--help` displays usage; invalid values produce an error and exit status 2.

Start the receiver first:

```sh
./build/pi_receiver --bind 0.0.0.0 --port 5000
```

Then start the sender on the PC:

```sh
./build/pc_sender --host PI_IP_OR_HOSTNAME --port 5000
```
