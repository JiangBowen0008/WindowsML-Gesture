#ifndef AUDIO_RECORDER_HPP
#define AUDIO_RECORDER_HPP

#include "RtAudio.h"
#include "utils.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <map>
#include <functional>


class AudioRecorder
{
public:
	AudioRecorder();
	~AudioRecorder();
	vector<float> readAudio();
	bool readRdy();

private:
	RtAudio::StreamParameters parameters;
	unsigned int sampleRate = 48000;
	unsigned int bufferFrames = 0; // 256 sample frames
	unsigned int nfft = 8192;
	unsigned int step = 2048;

	deque<float> buffer_;
	size_t bufferSize = 81920;
	size_t bufferCounter_ = 0;


	RtAudio adc;

protected:
	void printAPIs();

	static int record(	void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
						double streamTime, RtAudioStreamStatus status, void* userData);


	void writeBuffer(float);
};

#endif