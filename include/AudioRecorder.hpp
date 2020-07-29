#ifndef AUDIO_RECORDER_HPP
#define AUDIO_RECORDER_HPP

#define __WINDOWS_DS__

#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <map>

using namespace std;

class AudioRecorder
{
public:
	AudioRecorder();
	~AudioRecorder();
	vector<float> readAudio();
private:
	RtAudio::StreamParameters parameters;
	unsigned int sampleRate = 48000;
	unsigned int bufferFrames = 256; // 256 sample frames
	RtAudio adc;
	void printAPIs();

	//int record(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
	//	double streamTime, RtAudioStreamStatus status, void* userData);
};

#endif