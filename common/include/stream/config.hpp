#pragma once

#include <charconv>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace stream {

struct Config {
    std::string host = "127.0.0.1";
    std::string bind_address = "0.0.0.0";
    std::string source = "test";
    std::string pipewire_target;
    std::string pipewire_format = "raw";
    std::uint16_t port = 5000;
    int width = 1920;
    int height = 1080;
    int fps = 60;
    int bitrate_kbps = 12000;
};

inline int parse_int(std::string_view value, std::string_view option) {
    int result{};
    const auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), result);
    if (ec != std::errc{} || ptr != value.data() + value.size()) {
        throw std::runtime_error("Invalid integer for " + std::string(option) + ": " + std::string(value));
    }
    return result;
}

inline std::uint16_t parse_port(std::string_view value) {
    const int port = parse_int(value, "--port");
    if (port < 1 || port > 65535) {
        throw std::runtime_error("--port must be between 1 and 65535");
    }
    return static_cast<std::uint16_t>(port);
}

inline void validate(const Config& config) {
    if (config.source != "test" && config.source != "pipewire") {
        throw std::runtime_error("--source must be either test or pipewire");
    }
    if (config.source == "pipewire" && config.pipewire_target.empty()) {
        throw std::runtime_error("--pipewire-target is required when --source=pipewire");
    }
    if (config.pipewire_format != "raw" && config.pipewire_format != "mjpeg") {
        throw std::runtime_error("--pipewire-format must be either raw or mjpeg");
    }
    if (config.width < 1 || config.width > 16384) {
        throw std::runtime_error("--width must be between 1 and 16384");
    }
    if (config.height < 1 || config.height > 16384) {
        throw std::runtime_error("--height must be between 1 and 16384");
    }
    if (config.fps < 1 || config.fps > 240) {
        throw std::runtime_error("--fps must be between 1 and 240");
    }
    if (config.bitrate_kbps < 1 || config.bitrate_kbps > 200000) {
        throw std::runtime_error("--bitrate must be between 1 and 200000 kbps");
    }
}

inline void print_common_help(const char* program, bool sender) {
    std::cout << "Usage: " << program
              << (sender ? " [--host IP_OR_HOSTNAME] [--source test|pipewire]"
                         " [--pipewire-target NODE] [--pipewire-format raw|mjpeg]" : " [--bind IP_ADDRESS]")
              << " [--port PORT] [--width PIXELS] [--height PIXELS]"
                 " [--fps FPS] [--bitrate KBPS] [--help]\n";
}

inline Config parse_args(int argc, char** argv, bool sender) {
    Config config;
    for (int i = 1; i < argc; ++i) {
        const std::string_view option(argv[i]);
        if (option == "--help" || option == "-h") {
            print_common_help(argv[0], sender);
            std::exit(0);
        }
        if (i + 1 >= argc) {
            throw std::runtime_error("Missing value for " + std::string(option));
        }
        const std::string_view value(argv[++i]);
        if (option == "--host" && sender) {
            config.host = value;
        } else if (option == "--source" && sender) {
            config.source = value;
        } else if (option == "--pipewire-target" && sender) {
            config.pipewire_target = value;
        } else if (option == "--pipewire-format" && sender) {
            config.pipewire_format = value;
        } else if (option == "--bind" && !sender) {
            config.bind_address = value;
        } else if (option == "--port") {
            config.port = parse_port(value);
        } else if (option == "--width") {
            config.width = parse_int(value, option);
        } else if (option == "--height") {
            config.height = parse_int(value, option);
        } else if (option == "--fps") {
            config.fps = parse_int(value, option);
        } else if (option == "--bitrate") {
            config.bitrate_kbps = parse_int(value, option);
        } else {
            throw std::runtime_error("Unknown option: " + std::string(option));
        }
    }
    validate(config);
    return config;
}

} // namespace stream
