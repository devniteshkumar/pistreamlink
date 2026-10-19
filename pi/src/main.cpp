#include "stream/config.hpp"

#include <exception>
#include <iostream>

int main(int argc, char** argv) {
    try {
        const auto config = stream::parse_args(argc, argv, false);
        std::cout << "Stream PC to Pi — receiver skeleton\n"
                  << "Bind address: " << config.bind_address << '\n'
                  << "Port: " << config.port << '\n'
                  << "Expected video: " << config.width << 'x' << config.height
                  << " @ " << config.fps << " FPS\n"
                  << "Expected bitrate: " << config.bitrate_kbps << " kbps\n"
                  << "Status: configuration loaded; receiving/display is not implemented yet.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "pi_receiver: " << error.what() << '\n';
        return 2;
    }
}
