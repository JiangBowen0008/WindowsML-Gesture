#include "AudioRecorder.hpp"

/*-----------------------------------------------------------
					 Main Functions
-----------------------------------------------------------*/

vector<float> AudioRecorder::readAudio()
{
	vector<float> recording;
	return recording;
}

/*-----------------------------------------------------------
					Helper Functions 
-----------------------------------------------------------*/

int record(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void* userData)
{
	if (status)
		std::cout << "Stream overflow detected!" << std::endl;
	// Do something with the data in the "inputBuffer" buffer.
	float* inputPtr = (float*)inputBuffer;
	cout << *inputPtr << endl;
	return 0;
}

void AudioRecorder::printAPIs()
{
	std::map<int, std::string> apiMap;
	apiMap[RtAudio::MACOSX_CORE] = "OS-X Core Audio";
	apiMap[RtAudio::WINDOWS_ASIO] = "Windows ASIO";
	apiMap[RtAudio::WINDOWS_DS] = "Windows DirectSound";
	apiMap[RtAudio::WINDOWS_WASAPI] = "Windows WASAPI";
	apiMap[RtAudio::UNIX_JACK] = "Jack Client";
	apiMap[RtAudio::LINUX_ALSA] = "Linux ALSA";
	apiMap[RtAudio::LINUX_PULSE] = "Linux PulseAudio";
	apiMap[RtAudio::LINUX_OSS] = "Linux OSS";
	apiMap[RtAudio::RTAUDIO_DUMMY] = "RtAudio Dummy";

	std::vector< RtAudio::Api > apis;
	RtAudio::getCompiledApi(apis);

	std::cout << "\nRtAudio Version " << RtAudio::getVersion() << std::endl;

	std::cout << "\nCompiled APIs:\n";
	for (unsigned int i = 0; i < apis.size(); i++)
		std::cout << "  " << apiMap[apis[i]] << std::endl;
}

/*-----------------------------------------------------------
				Constructors & Destructors
-----------------------------------------------------------*/

AudioRecorder::AudioRecorder()
{
	/**
	 * Start the stream when constructing the recorder
	 */
	printAPIs();

	if (adc.getDeviceCount() == 0) {
		std::cout << "\nNo audio devices found!\n";
		exit(0);
	}
	parameters.deviceId = adc.getDefaultInputDevice();
	parameters.nChannels = 2;
	parameters.firstChannel = 0;

	try {
		adc.openStream(	NULL, 
						&parameters,
						RTAUDIO_FLOAT32,
						sampleRate,
						&bufferFrames,
						&record);
		adc.startStream();
	}
	catch (RtAudioError& e) {
		e.printMessage();
		exit(0);
	}
}

AudioRecorder::~AudioRecorder()
{
	/**
	 * Stop the stream on destruction
	 */
	try {
		// Stop the stream
		adc.stopStream();
	}
	catch (RtAudioError& e) {
		e.printMessage();
	}
	if (adc.isStreamOpen()) adc.closeStream();
}