#pragma once

#include <string>
#include <vector>

struct Endpoint {
    const std::string name;
    const std::string path;

    const std::string author() const {
        return path.substr(0, path.find('/'));
    }

    static const std::vector<Endpoint> all;
};

const std::vector<Endpoint> Endpoint::all = {
    {"TA Variations", "TechAudio/ldm_variations"},
    {"TA Style Transfer", "TechAudio/ldm_styletransfer"},
    {"TA Diffusers", "TechAudio/diffusers_HARP_test"},
    {"Pitch shifting", "hugggof/pitch_shifter"},
    {"Harmonic/percussive source separation", "hugggof/harmonic_percussive"},
    {"Music audio generation", "descript/vampnet"},
    {"Convert Instrumental Music into 8-bit Chiptune", "hugggof/nesquik"},
    {"Music audio generation", "hugggof/MusicGen"},
    {"Pitch-preserving timbre-removal", "cwitkowitz/timbre-trap"}
};
