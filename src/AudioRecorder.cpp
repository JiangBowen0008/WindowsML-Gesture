#include "AudioRecorder.hpp"

/*-----------------------------------------------------------
					 Main Functions
-----------------------------------------------------------*/

MatrixXf AudioRecorder::readAudio()
{
	assert(readRdy());			// only allow read when it is ready
	
	// read from buffer
	//DBOUT << "Buffer size: " << getBufferSize() << endl;
	
	MatrixXf clip(NUM_CHANNEL, NFFT);

	clip << buffer_[0],
			buffer_[1],
			buffer_[2],
			buffer_[3];

	// Removing one step of frames from the buffer
	buffer_.pop_front();
	
	return clip;
}

/*-----------------------------------------------------------
					Helper Functions 
-----------------------------------------------------------*/

int AudioRecorder::record(	void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
							double streamTime, RtAudioStreamStatus status, void* userData)
{
	if (status)
		std::cout << "Stream overflow detected!" << std::endl;
	// Do something with the data in the "inputBuffer" buffer.
	float* inputPtr = (float*)inputBuffer;
	AudioRecorder* recorderPtr = (AudioRecorder*)userData;
	recorderPtr->writeBuffer(inputPtr);
	//cout << *inputPtr << endl;
	return 0;
}

int AudioRecorder::playback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void* userData)
{
	if (status)
		std::cout << "Stream underflow detected!" << std::endl;
	// Do something with the data in the "inputBuffer" buffer.
	AudioRecorder* recorderPtr = (AudioRecorder*)userData;
	
	memcpy( outputBuffer, 
			recorderPtr->getOutputBuffer(),
			recorderPtr->getOutputBufferSize());
	float* a = (float*)outputBuffer;
	recorderPtr->isPlaybackOn = true;
	//cout << *inputPtr << endl;
	return 0;
}

void AudioRecorder::writeBuffer(float* signal)
{
	Eigen::Map<StepSignal, Eigen::Unaligned, Eigen::OuterStride<NUM_CHANNEL>> input(signal, NUM_CHANNEL, bufferFrames);
	//input.setConstant(1);

	//DBOUT << "sum:\t" << input.sum() << endl;

	buffer_.emplace_back(input);
	if (getBufferSize() >= bufferSize)
	{
		buffer_.pop_front();
	}
	
	logger << input.transpose() << endl;
	//bufferCounter_++;
	//cout << bufferCounter_ << endl;
}

inline int AudioRecorder::getBufferSize() const
{
	return buffer_.size();
}

bool AudioRecorder::readRdy() const
{
	bool result = getBufferSize() >= NUM_STEP_PER_FFT;
	//result &= bufferCounter_ >= step;
	return result;
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

	DBOUT << "\nRtAudio Version " << RtAudio::getVersion() << std::endl;

	DBOUT << "\nCompiled APIs:\n";
	for (unsigned int i = 0; i < apis.size(); i++)
		DBOUT << "  " << apiMap[apis[i]] << std::endl;
}

/*-----------------------------------------------------------
				Constructors & Destructors
-----------------------------------------------------------*/

AudioRecorder::AudioRecorder() :
	buffer_(0)
{
	printAPIs();

	if (adc.getDeviceCount() == 0 || dac.getDeviceCount() == 0) {
		std::cout << "\nNo audio devices found!\n";
		exit(0);
	}
	DBOUT << "<----------------Available Devices--------------------->" << endl;
	for (size_t i = 0; i < dac.getDeviceCount(); ++i)
	{
		auto info = dac.getDeviceInfo(i);
		DBOUT << "Device " << i << " Name: " << info.name << endl;
		DBOUT << "Device " << i << " isDefaultOutput: " << info.isDefaultOutput << endl;
		DBOUT << "Device " << i << " isDefaultInput: " << info.isDefaultInput << endl;
		DBOUT << "Device " << i << " preferredFs: " << info.preferredSampleRate << endl;
		DBOUT << endl;
	}
	DBOUT << "<------------------------------------------------------>" << endl;

	startPlaying();
	startRecording();

	// open the logger
	logger.open("logAudio.txt", std::ios_base::out);
}

void AudioRecorder::startRecording()
{
	/**
	 * Start the stream when constructing the recorder
	 */
	while (!isPlaybackOn) Sleep(1000);

	RtAudio::StreamParameters parameters;
	parameters.deviceId = adc.getDefaultInputDevice();
	parameters.nChannels = NUM_CHANNEL;
	parameters.firstChannel = 0;
	
	try {
		adc.openStream(NULL,
			&parameters,
			RTAUDIO_FLOAT32,
			sampleRate,
			&bufferFrames,
			&record,
			this);
		adc.startStream();
	}
	catch (RtAudioError& e) {
		e.printMessage();
		exit(0);
	}
}

void AudioRecorder::initPlaybackBuffer()
{
	// initialize the playback buffer
	bool flag = true;
	for (size_t i = 0; i < PLAYBACK_BUFFER_SIZE; ++i)
	{
		flag = !flag;
		float write = (flag) ? 1 : -1;

		playbackBuffer_[2 * i] = write;
		playbackBuffer_[2 * i + 1] = write;
	}
}

void AudioRecorder::startPlaying()
{
	initPlaybackBuffer();

	RtAudio::StreamParameters parameters;
	parameters.deviceId = dac.getDefaultOutputDevice();
	DBOUT << "Using deviceID: " << parameters.deviceId  << " for playback." << endl;
	parameters.nChannels = NUM_CHANNEL;
	parameters.firstChannel = 0;

	RtAudio::StreamOptions opts;
	opts.flags = RTAUDIO_SCHEDULE_REALTIME;
	opts.streamName = "RtPlayback";

	unsigned int outputBufferSize = getOutputBufferSize();
	unsigned int tempBufferSize = PLAYBACK_BUFFER_SIZE;
	
	try {
		dac.openStream(&parameters,
			NULL,
			RTAUDIO_FLOAT32,
			2*FC,
			&tempBufferSize,
			&playback,
			(void*)this,
			&opts);
		dac.startStream();
	}
	catch (RtAudioError& e) {
		DBOUT << "right here" << endl;
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
		dac.stopStream();
	}
	catch (RtAudioError& e) {
		e.printMessage();
	}
	if (adc.isStreamOpen()) adc.closeStream();
	if (dac.isStreamOpen()) dac.closeStream();
}