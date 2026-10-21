#include "stream/gstreamer_process.hpp"

#include <exception>
#include <iostream>

int main(int argc, char** argv) {
    try {
        const auto config = stream::parse_args(argc, argv, true);
        std::cout << "Stream PC to Pi — test-pattern sender\n"
                  << "Receiver: " << config.host << ':' << config.port << '\n'
                  << "Video: " << config.width << 'x' << config.height
                  << " @ " << config.fps << " FPS\n"
                  << "Target bitrate: " << config.bitrate_kbps << " kbps\n"
                  << "Starting GStreamer VP8/RTP pipeline; press Ctrl-C to stop.\n";
        return stream::run_gstreamer(stream::sender_pipeline(config));
    } catch (const std::exception& error) {
        std::cerr << "pc_sender: " << error.what() << '\n';
        return 2;
    }
}
