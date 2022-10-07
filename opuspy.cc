#include <iostream>
#include "opusenc.h"
#include "opusfile.h"
#include "opus.h"
#include <vector>
#include <cmath>
#include <stdexcept>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

template<typename T>
py::array_t<T> MakeNpArray(std::vector<ssize_t> shape, T* data) {

    std::vector<ssize_t> strides(shape.size());
    size_t v = sizeof(T);
    size_t i = shape.size();
    while (i--) {
        strides[i] = v;
        v *= shape[i];
    }
    py::capsule free_when_done(data, [](void* f) {
        auto* foo = reinterpret_cast<T*>(f);
        delete[] foo;
    });
    return py::array_t<T>(shape, strides, data, free_when_done);
}



void OpusWrite(const std::string& path, const py::array_t<int16_t>& waveform_tc, const int sample_rate, const int bitrate=OPUS_AUTO, const int signal_type = 0, const int encoder_complexity = 10) {
    if (waveform_tc.ndim() != 2) {
        throw py::value_error("waveform_tc must have exactly 2 dimension: [time, channels].");
    }
    if (waveform_tc.shape(1) > 8 || waveform_tc.shape(1) < 1) {
        throw py::value_error("waveform_tc must have at least 1 channel, and no more than 8.");
    }
    if ((bitrate < 500 or bitrate > 512000) && bitrate != OPUS_BITRATE_MAX && bitrate != OPUS_AUTO) {
        throw py::value_error("Invalid bitrate, must be at least 512 and at most 512k bits/s.");
    }
    if (sample_rate < 8000 or sample_rate > 48000) {
        throw py::value_error("Invalid sample_rate, must be at least 8k and at most 48k.");
    }
    if (encoder_complexity > 10 || encoder_complexity < 0) {
        throw py::value_error("Invalid encoder_complexity, must be in range [0, 10] inclusive. The higher, the better quality at the given bitrate, but uses more CPU.");
    }
    opus_int32 opus_signal_type;
    switch (signal_type) {
        case 0:
            opus_signal_type = OPUS_AUTO;
            break;
        case 1:
            opus_signal_type = OPUS_SIGNAL_MUSIC;
            break;
        case 2:
            opus_signal_type = OPUS_SIGNAL_VOICE;
            break;
        default:
            throw py::value_error("Invalid signal type, must be 0 (auto), 1 (music) or 2 (voice).");
    }

    OggOpusComments* comments = ope_comments_create();
    //  ope_comments_add(comments, "hello", "world");
    int error;
    // We set family == 1, and channels based on waveform.
    OggOpusEnc* enc = ope_encoder_create_file(
            path.data(), comments, sample_rate, waveform_tc.shape(1), 0, &error);
    if (error != 0) {
        throw py::value_error("Unexpected error, is the provided path valid?");
    }

    if (ope_encoder_ctl(enc, OPUS_SET_BITRATE_REQUEST, bitrate) != 0) {
        throw py::value_error("This should not happen. Could not set bitrate...");
    }


    if (ope_encoder_ctl(enc, OPUS_SET_SIGNAL_REQUEST, opus_signal_type) != 0) {
        throw py::value_error("This should not happen. Could not set signal type...");
    }
    if (ope_encoder_ctl(enc, OPUS_SET_COMPLEXITY_REQUEST, encoder_complexity) != 0) {
        throw py::value_error("This should not happen. Could not set encoder complexity...");
    }

    // OK, now we are all configured. Let's write!
    if (ope_encoder_write(enc, waveform_tc.data(), waveform_tc.shape(0)) != 0) {
        throw py::value_error("Could not write audio data.");
    }
    if (ope_encoder_drain(enc) != 0) {
        throw py::value_error("Could not finalize write.");
    }

    // Cleanup.
    ope_encoder_destroy(enc);
    ope_comments_destroy(comments);
}

std::tuple<py::array_t<opus_int16>, int> OpusRead(const std::string& path) {
    int error;
    OggOpusFile* file = op_open_file(path.data(), &error);
    if (error != 0) {
        throw py::value_error("Could not open opus file.");
    }
    const ssize_t num_chans = op_channel_count(file, -1);
    const ssize_t num_samples = op_pcm_total(file, -1) / num_chans;

    const OpusHead* meta = op_head(file, -1); // unowned
    const int sample_rate = meta->input_sample_rate;

    auto* data = static_cast<opus_int16 *>(malloc(sizeof(opus_int16) * num_chans * num_samples));
    auto waveform_tc = MakeNpArray<opus_int16>({num_samples, num_chans}, data);
    size_t num_read = 0;

    while (true) {
        int chunk = op_read(file, data + num_read*num_chans, num_samples-num_read*num_chans, nullptr);
        if (chunk < 0) {
            throw py::value_error("Could not read opus file.");
        }
        if (chunk == 0) {
            break;
        }
        num_read += chunk;
        if (num_read > num_samples) {
            throw py::value_error("Read too much???");
        }
    }

    if (num_read < num_samples-10) {
        std::cout << num_read << " " << num_samples << " " << num_chans;
        throw py::value_error("Could not complete read...");
    }
    op_free(file);
    return std::make_tuple(std::move(waveform_tc), sample_rate);
}

//int main(int argc, char *argv[])
//{
//    int err;
//    const int sample_rate = 48000;
//    const int wave_hz = 330;
//    const opus_int16 max_ampl = std::numeric_limits<opus_int16>::max() / 2;
//    OggOpusComments* a = ope_comments_create();
//    OggOpusEnc* file = ope_encoder_create_file(
//            "hello.opus", a, sample_rate, 1, 0, &err);
//    if (ope_encoder_ctl(file, OPUS_SET_BITRATE_REQUEST, 10000) != 0) {
//        throw std::invalid_argument("Invalid bitrate.");
//    }
//
//
//    std::vector<int16_t> wave;
//    for (int i = 0; i < sample_rate*11; i++) {
//        double ampl = max_ampl * sin(static_cast<double>(i)/sample_rate*2*M_PI*wave_hz);
//        wave.push_back(static_cast<opus_int16>(ampl));
//    }
//
//    ope_encoder_write(file, wave.data(), wave.size());
//    ope_encoder_drain(file);
//    ope_encoder_destroy(file);
//}



PYBIND11_MODULE(opuspy, m) {

    m.def("write", &OpusWrite, py::arg("path"), py::arg("waveform_tc"), py::arg("sample_rate"), py::arg("bitrate")=OPUS_AUTO, py::arg("signal_type")=0, py::arg("encoder_complexity")=10,
          "Saves the waveform_tc as the opus-encoded file at the specified path. The waveform must be a numpy array of np.int16 type, and shape [samples (time axis), channels]. Recommended sample rate is 48000. You can specify the bitrate in bits/s, as well as encoder_complexity (in range [0, 10] inclusive, the higher the better quality at given bitrate, but more CPU usage, 10 is recommended). Finally, there is signal_type option, that can help to improve quality for specific audio, types (0 = AUTO (default), 1 = MUSIC, 2 = SPEECH).");
    m.def("read", &OpusRead, py::arg("path"), "Returns the waveform_tc as the int16 np.array of shape [samples, channels] and the original sample rate. NOTE: the waveform returned is ALWAYS at 48khz as this is how opus stores any waveform, the sample rate returned is just the original sample rate of encoded audio that you might witch to resample the returned waveform to.");
}