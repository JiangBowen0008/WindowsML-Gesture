#ifndef AUDIO_RECORDER_HPP
#define AUDIO_RECORDER_HPP

#include "RtAudio.h"
#include "utils.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <map>
#include <functional>

#define NUM_CHANNEL 2
#define NFFT 8192
#define STEP 2048
#define NUM_STEP_PER_FFT 4

#define PLAYBACK_BUFFER_SIZE 19000

#define _DEBUG_AUDIO

// debug streaming
#ifdef _DEBUG_AUDIO
#undef DBOUT
#define DBOUT cout // or any other ostream
#else
#undef DBOUT
#define DBOUT 0 && cout
#endif

typedef Matrix<float, NUM_CHANNEL, STEP> StepSignal;

class AudioRecorder
{
public:
	AudioRecorder();
	~AudioRecorder();
	MatrixXf readAudio();
	bool readRdy() const;
	inline int getBufferSize() const;

private:
	unsigned int sampleRate = FS;
	unsigned int bufferFrames = STEP; // 256 sample frames
	unsigned int nfft = NFFT;
	unsigned int step = STEP;
	bool isPlaybackOn = false;

	size_t bufferSize = 10;

	deque<StepSignal> buffer_;		// We use a thread-safe container here
									// to deal with the asynchronous IO.
									// It comes with some performance loss.

	

	float playbackBuffer_[PLAYBACK_BUFFER_SIZE * NUM_CHANNEL];
	//int bufferCounter_ = 0;

	RtAudio adc, dac;

	// logging
	ofstream logger;

protected:
	void printAPIs();

	static int record(	void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
						double streamTime, RtAudioStreamStatus status, void* userData);
	static int playback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
						double streamTime, RtAudioStreamStatus status, void* userData);

	void startRecording();
	void startPlaying();

	void writeBuffer(float*);

	void initPlaybackBuffer();
	float* getOutputBuffer() { return playbackBuffer_; }
	size_t getOutputBufferSize() { return sizeof(playbackBuffer_); }
};

#endif