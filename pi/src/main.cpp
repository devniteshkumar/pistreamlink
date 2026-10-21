#include "stream/gstreamer_process.hpp"

#include <exception>
#include <iostream>

int main(int argc, char** argv) {
    try {
        const auto config = stream::parse_args(argc, argv, false);
        std::cout << "Stream PC to Pi — RTP receiver\n"
                  << "Bind address: " << config.bind_address << '\n'
                  << "Port: " << config.port << '\n'
                  << "Expected video: " << config.width << 'x' << config.height
                  << " @ " << config.fps << " FPS\n"
                  << "Expected bitrate: " << config.bitrate_kbps << " kbps\n"
                  << "Starting GStreamer VP8/RTP pipeline; press Ctrl-C to stop.\n";
        return stream::run_gstreamer(stream::receiver_pipeline(config));
    } catch (const std::exception& error) {
        std::cerr << "pi_receiver: " << error.what() << '\n';
        return 2;
    }
}
