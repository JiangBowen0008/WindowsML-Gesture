// WindowsML-Gesture.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <vector>
#include "GestureNet.hpp"
#include "FeatureExtractor.hpp"
#include "AudioRecorder.hpp"


using namespace std;

int main(int argc, char* argv[]) {
	// Load model with a path to the .pb file. 
	// An optional std::vector<uint8_t> parameter can be used to supply Tensorflow with
	// session options. The vector must represent a serialized ConfigProto which can be 
	// generated manually in python. See create_config_options.py.
	// Example:
	// const std::vector<uint8_t> ModelConfigOptions = { 0x32, 0xb, 0x9, 0x9a, 0x99, 0x99, 0x99, 0x99, 0x99, 0xb9, 0x3f, 0x20, 0x1 };
	// Model model("../model.pb", ModelConfigOptions);

	// string modelName(argv[1]);
	string modelName = "VadNet_9.pb";
	AudioRecorder recorder;
	cout << "Using " << modelName << endl;
	GestureNet net(modelName, "input", "softmax");
	FeatureExtractor featExtractor;

	float totalTime = 0.0f;
	size_t numTrials = 1000;
	vector<float> data(14);
	std::iota(data.begin(), data.end(), fmod(rand(), 100));

	for (int i = 0; i < 1000; ++i)
	{
		
		//vector<float> rawData(8192);
		//std::iota(data.begin(), data.end(), fmod(rand(), 5));
		//auto feat = featExtractor.extractFeature(rawData);

		auto start = chrono::high_resolution_clock::now();
		
		// cout << feat;
		net.getPred(data);
		auto end = chrono::high_resolution_clock::now();
		totalTime += chrono::duration_cast<chrono::duration<float>>(end - start).count();
	}

	cout << "Average time: " << totalTime / numTrials << endl;

}
