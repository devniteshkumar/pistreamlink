# Stream PC to Pi — Milestone 1

C++20/CMake skeleton for the Fedora PC sender and Arch Linux ARM receiver.
This milestone does not yet send or receive video and does not require GStreamer.

## Dependencies

- CMake 3.20+
- A C++20 compiler (GCC 10+ or a recent Clang)
- `make` or another CMake-supported build tool

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
the future receiver will bind to; `0.0.0.0` means all IPv4 interfaces.
These options only print configuration in Milestone 1; they do not open sockets.

## Milestone 1 acceptance checks

- `cmake` configures successfully on both machines.
- `cmake --build build -j` succeeds on both machines.
- Both executables start, print their settings, and exit successfully.
- `--help` displays usage; invalid values produce an error and exit status 2.
