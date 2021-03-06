/*************************************************************************************
 * MIT License                                                                       *
 *                                                                                   *
 * Copyright (c) 2018 TiWinDeTea                                                     *
 *                                                                                   *
 * Permission is hereby granted, free of charge, to any person obtaining a copy      *
 * of this software and associated documentation files (the "Software"), to deal     *
 * in the Software without restriction, including without limitation the rights      *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell         *
 * copies of the Software, and to permit persons to whom the Software is             *
 * furnished to do so, subject to the following conditions:                          *
 *                                                                                   *
 * The above copyright notice and this permission notice shall be included in all    *
 * copies or substantial portions of the Software.                                   *
 *                                                                                   *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR        *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,          *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE       *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER            *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,     *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     *
 * SOFTWARE.                                                                         *
 *                                                                                   *
 *************************************************************************************/

#include "sound_sender.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <sound_sender.hpp>

inline std::optional<sound_sender> sound_sender::try_build() {
	try {
		sound_sender s{};
		return {std::move(s)};
	} catch (const std::runtime_error&) {
		return {};
	}
}

inline sound_sender::sound_sender() {

	// Request the default capture device with a half-second buffer
	input_device = alcCaptureOpenDevice(nullptr, cst::frequency, AL_FORMAT_MONO16, cst::frequency / 2);
	if (!input_device) {
		throw std::runtime_error("No microphone found.");
	}
	alcCaptureStart(input_device);
}

inline sound_sender::sound_sender(sound_sender&& other) noexcept {
	input_device = other.input_device;
	other.input_device = nullptr;
}

inline sound_sender& sound_sender::operator=(sound_sender&& other) noexcept {
	input_device = other.input_device;
	other.input_device = nullptr;
	return *this;
}

inline sound_sender::~sound_sender() {
	if (input_device) {
		alcCaptureStop(input_device);
		alcCaptureCloseDevice(input_device);
	}
}

inline void sound_sender::  send_sample_to(const breep::tcp::network& net, const breep::tcp::peer& peer) noexcept {
	net.send_object_to(peer, buffer);
}

inline bool sound_sender::update_sample() noexcept {
	ALCint samplesIn = 0; // How many samples were captured

	// Poll for captured audio
	alcGetIntegerv(input_device, ALC_CAPTURE_SAMPLES, 1, &samplesIn);

	if (samplesIn > cst::cap_size) {
		// Grab the sound
		alcCaptureSamples(input_device, buffer.data(), cst::cap_size);
		return true;
	}

	return false;
}
