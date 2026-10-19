#include "stream/config.hpp"

#include <exception>
#include <iostream>

int main(int argc, char** argv) {
    try {
        const auto config = stream::parse_args(argc, argv, true);
        std::cout << "Stream PC to Pi — sender skeleton\n"
                  << "Receiver: " << config.host << ':' << config.port << '\n'
                  << "Video: " << config.width << 'x' << config.height
                  << " @ " << config.fps << " FPS\n"
                  << "Target bitrate: " << config.bitrate_kbps << " kbps\n"
                  << "Status: configuration loaded; streaming is not implemented yet.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "pc_sender: " << error.what() << '\n';
        return 2;
    }
}
