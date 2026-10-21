#pragma once

#include "stream/config.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace stream {

inline int run_gstreamer(const std::vector<std::string>& arguments) {
    if (arguments.empty()) {
        throw std::invalid_argument("GStreamer command cannot be empty");
    }

    std::vector<char*> argv;
    argv.reserve(arguments.size() + 1);
    for (const auto& argument : arguments) {
        argv.push_back(const_cast<char*>(argument.c_str()));
    }
    argv.push_back(nullptr);

    const pid_t child = fork();
    if (child < 0) {
        throw std::runtime_error("Unable to start GStreamer: " + std::string(std::strerror(errno)));
    }
    if (child == 0) {
        execvp(argv.front(), argv.data());
        _exit(127);
    }

    int status = 0;
    if (waitpid(child, &status, 0) < 0) {
        throw std::runtime_error("Unable to wait for GStreamer: " + std::string(std::strerror(errno)));
    }
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status)) {
        throw std::runtime_error("GStreamer terminated by signal " +
                                 std::to_string(WTERMSIG(status)));
    }
    throw std::runtime_error("GStreamer ended unexpectedly");
}

inline std::vector<std::string> sender_pipeline(const Config& config) {
    return {
        "gst-launch-1.0",
        "-e",
        "videotestsrc",
        "is-live=true",
        "pattern=smpte",
        "!",
        "video/x-raw,width=" + std::to_string(config.width) +
            ",height=" + std::to_string(config.height) +
            ",framerate=" + std::to_string(config.fps) + "/1",
        "!",
        "videoconvert",
        "!",
        "vp8enc",
        "deadline=1",
        "target-bitrate=" + std::to_string(config.bitrate_kbps * 1000),
        "!",
        "rtpvp8pay",
        "pt=96",
        "!",
        "udpsink",
        "host=" + config.host,
        "port=" + std::to_string(config.port),
        "sync=false",
        "async=false",
    };
}

inline std::vector<std::string> receiver_pipeline(const Config& config) {
    return {
        "gst-launch-1.0",
        "-e",
        "udpsrc",
        "address=" + config.bind_address,
        "port=" + std::to_string(config.port),
        "caps=application/x-rtp,media=video,encoding-name=VP8,payload=96,clock-rate=90000",
        "!",
        "rtpvp8depay",
        "!",
        "vp8dec",
        "!",
        "videoconvert",
        "!",
        "autovideosink",
        "sync=false",
    };
}

} // namespace stream
