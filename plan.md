# Stream PC to Pi --- Project Plan

## 1. Goal

Build a low-latency C++ application that streams the PC's desktop to a
Raspberry Pi, which displays it on an HDMI-connected screen.

### Initial scope (MVP)

-   Capture the full desktop on the PC.
-   Encode and transmit video to the Raspberry Pi.
-   Receive, decode, and display the stream fullscreen on the Pi.
-   Target 1920×1080 at 60 FPS, with a practical balance of sharpness
    and motion.
-   Aim for under 50 ms end-to-end latency; this is a performance goal,
    not a guarantee.
-   No audio in the initial version.
-   No input forwarding in the initial version.
-   Add selected-window capture and keyboard/mouse forwarding in later
    phases.

## 2. Current Environment

### PC

-   Fedora Linux 44, x86_64.
-   Wayland desktop session.
-   Linux is the first implementation target.
-   Future platform targets: Windows and macOS.

### Raspberry Pi

-   Raspberry Pi 4 Model B.
-   Arch Linux ARM.
-   HDMI-connected display.
-   Approximately 1.78 GiB of memory available in the observed setup.

### Current connectivity

-   The PC and Pi can already reach one another on the same network.
-   SSH currently works over that existing network.
-   Use the existing network for the first prototype.
-   A direct Ethernet cable between PC and Pi is a possible later
    optimization.
-   Keep the Pi powered separately. Do not connect the PC and Pi with a
    USB-A-to-USB-A cable: both ports are host ports, and this is not a
    suitable host-to-host connection.

## 3. Proposed Technology

-   **Language:** C++ (use C++20 unless implementation constraints
    suggest otherwise).
-   **Build system:** CMake.
-   **Media pipeline:** GStreamer, subject to verifying package
    availability on Fedora and Arch Linux ARM.
-   **PC capture:** PipeWire through the Wayland desktop portal, since
    the PC runs Wayland.
-   **Video codec:** H.264 as the initial candidate for compatibility
    and low-latency streaming.
-   **Transport:** RTP over UDP as the initial candidate; keep transport
    details isolated so they can be changed if testing shows a better
    fit.
-   **Pi playback:** GStreamer receive/decode/display pipeline,
    fullscreen on HDMI.
-   **Acceleration:** Check which hardware encoding/decoding paths and
    GStreamer plugins are actually available on the PC and Pi. Do not
    assume hardware acceleration is available until tested.

These are starting choices, not immutable requirements. Validate them
with a small end-to-end prototype before building abstractions around
them.

## 4. High-Level Architecture

``` text
PC (Fedora / Wayland)
  Desktop capture (PipeWire / portal)
       |
       v
  Low-latency H.264 encoder
       |
       v
  Configurable video transport (initially RTP/UDP)
       |
       | Existing LAN/Wi-Fi for MVP
       | Direct Ethernet can be added later
       v
Raspberry Pi 4 (Arch Linux ARM)
  Receive stream
       |
       v
  H.264 decoder
       |
       v
  Fullscreen HDMI display
```

Keep capture, encoding, transport, decoding, and display concerns
separate. Network configuration must not be embedded into capture or
codec logic.

## 5. Networking Plan

### Phase 1: Existing network

1.  Keep the current Wi-Fi/LAN and SSH setup unchanged.
2.  Find the Pi's reachable IP address or hostname.
3.  Configure the sender with the receiver's address and port using
    command-line options or a configuration file.
4.  Confirm that the receiver can be reached and that video packets
    arrive.
5.  Bind or route the stream deliberately where practical, rather than
    relying on an assumed interface when multiple network interfaces are
    active.

### Phase 2: Optional direct Ethernet

1.  Connect the PC's Ethernet port to the Pi's Ethernet port using a
    normal Ethernet cable. If the PC has no Ethernet port, use a
    compatible USB-to-Ethernet adapter.
2.  Keep Wi-Fi enabled for SSH and internet access.
3.  Assign addresses on a dedicated, non-conflicting subnet, for
    example:
    -   PC Ethernet: `192.168.50.1/24`
    -   Pi Ethernet: `192.168.50.2/24`
4.  Do not add a default gateway to this dedicated link unless a
    specific routing need arises.
5.  Change the stream destination to the Pi's direct-link address and
    verify the chosen interface is used.
6.  Compare frame rate, packet loss, jitter, and end-to-end latency
    against the existing network.

The intended result is a network/configuration change, not a rewrite of
the media pipeline. Direct Ethernet is optional until measurements show
that it is useful.

## 6. Repository and Module Direction

Suggested starting layout; refine only as implementation needs become
clear:

``` text
stream-pc-to-pi/
├── CMakeLists.txt
├── README.md
├── plan.md
├── config/
│   └── stream.example.conf
├── common/
│   └── include/stream/
│       └── config.hpp
├── pc/
│   ├── CMakeLists.txt
│   └── src/
│       ├── main.cpp
│       ├── capture.cpp
│       ├── encoder.cpp
│       └── sender.cpp
└── pi/
    ├── CMakeLists.txt
    └── src/
        ├── main.cpp
        ├── receiver.cpp
        └── display.cpp
```

Prefer GStreamer's APIs and pipeline facilities over prematurely
implementing codec internals. Keep platform-specific capture behind an
interface so a Windows or macOS capture backend can be added later.

## 7. Milestones

### Milestone 0 --- Environment checks

-   [ ] Confirm PC and Pi network addresses and current SSH workflow.
-   [ ] Check whether the PC has a built-in Ethernet port (needed only
    for the optional direct-link phase).
-   [ ] Check GStreamer and required plugin availability on Fedora 44
    and Arch Linux ARM.
-   [ ] Check the Pi's available decode/display paths.

**Exit criteria:** Dependencies and basic connectivity are understood
before implementation choices are locked in.

### Milestone 1 --- Build skeleton

-   [ ] Create a CMake-based C++ project.
-   [ ] Build a minimal executable on the PC.
-   [ ] Build a minimal receiver executable on the Pi.
-   [ ] Add configurable host, port, resolution, frame rate, and bitrate
    settings where applicable.

**Exit criteria:** Both programs compile and start on their target
devices.

### Milestone 2 --- Transport proof of concept

-   [ ] Send a generated test pattern or test video from PC to Pi over
    the existing network.
-   [ ] Receive and display it on the Pi.
-   [ ] Verify stability and reconnect/error behavior.

**Exit criteria:** A continuous video stream reaches the HDMI display
without desktop capture.

### Milestone 3 --- Desktop capture

-   [ ] Capture the full desktop using the supported Wayland
    portal/PipeWire flow.
-   [ ] Handle permission prompts and capture-session errors.
-   [ ] Verify frame dimensions and frame timing.

**Exit criteria:** The PC can produce a reliable stream of desktop
frames.

### Milestone 4 --- Encode and stream

-   [ ] Integrate low-latency H.264 encoding.
-   [ ] Send encoded frames over the chosen transport.
-   [ ] Keep queues short and avoid buffering stale frames.
-   [ ] Avoid B-frames and lookahead where the selected encoder exposes
    these controls and latency testing supports doing so.

**Exit criteria:** Live desktop video is visible on the Pi.

### Milestone 5 --- Performance and reliability

-   [ ] Test 1080p at 60 FPS.
-   [ ] Measure end-to-end latency instead of relying on visual
    estimates.
-   [ ] Track dropped frames, jitter, CPU load, and memory usage.
-   [ ] Tune bitrate, encoder settings, queue sizes, and decoder path.
-   [ ] Prefer hardware acceleration where it is available and
    beneficial.

**Exit criteria:** Document actual performance and limitations; treat
\<50 ms as a target to validate, not a promise.

### Milestone 6 --- Optional direct Ethernet

-   [ ] Configure a dedicated PC-to-Pi Ethernet subnet.
-   [ ] Preserve Wi-Fi and the existing SSH workflow.
-   [ ] Select the direct-link address for video.
-   [ ] Repeat the performance measurements and compare results.

**Exit criteria:** Direct Ethernet can be enabled through configuration
and its measured benefit is known.

### Later milestones --- Outside the initial scope

-   [ ] Selected-window capture.
-   [ ] Keyboard and mouse input forwarding from Pi to PC.
-   [ ] Windows capture backend.
-   [ ] macOS capture backend.
-   [ ] Audio, only if added as a separate planned feature.

## 8. Latency and Quality Principles

-   60 FPS gives a frame interval of about 16.7 ms; capture, encoding,
    network transfer, decoding, and display all contribute to total
    latency.
-   Keep queues bounded and prefer dropping old frames over building up
    delay.
-   Tune quality and bitrate based on real tests and network capacity.
-   Use low-latency encoder settings and avoid unnecessary buffering.
-   Verify decoder and display behavior on the Pi; do not assume a
    particular hardware acceleration path.
-   Measure latency with a repeatable method and document the method
    alongside results.

## 9. Design Decisions to Preserve

1.  **Network is configurable:** destination IP, port, and any
    interface-specific behavior must be configurable.
2.  **Media pipeline is transport-independent:** changing from Wi-Fi/LAN
    to direct Ethernet should not require rewriting capture, encoding,
    or playback.
3.  **Capture is backend-based:** start with Wayland/PipeWire on Fedora;
    add other operating-system backends later.
4.  **Keep the MVP narrow:** full desktop video only; no audio or input
    forwarding yet.
5.  **Validate before optimizing:** get a simple end-to-end stream
    working, then measure and improve it.
6.  **Never use USB-A-to-USB-A as a PC-to-Pi data connection.**

## 10. Immediate Next Steps

1.  Confirm how the Pi is addressed for SSH (IP address or hostname).
2.  Check whether the PC has an Ethernet port; this only affects the
    optional direct-Ethernet milestone.
3.  Inspect available GStreamer packages/plugins on both devices.
4.  Build the smallest possible CMake project and test-pattern stream
    over the current network.
