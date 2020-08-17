// WindowsML-Gesture.cpp : Defines the entry point for the application.
//

#include "utils.hpp"
//#include "GestureNetTF.hpp"
//#include "GestureNetONNX.hpp"
#include "GestureNetTorch.hpp"
#include "FeatureExtractor.hpp"
#include "AudioRecorder.hpp"

int main(int argc, char* argv[]) {
	// Load model with a path to the .pb file. 
	// An optional std::vector<uint8_t> parameter can be used to supply Tensorflow with
	// session options. The vector must represent a serialized ConfigProto which can be 
	// generated manually in python. See create_config_options.py.
	// Example:
	// const std::vector<uint8_t> ModelConfigOptions = { 0x32, 0xb, 0x9, 0x9a, 0x99, 0x99, 0x99, 0x99, 0x99, 0xb9, 0x3f, 0x20, 0x1 };
	// Model model("../model.pb", ModelConfigOptions);

	// string modelName(argv[1]);
	string modelName = "..\\..\\assets\\test.pt";
	AudioRecorder recorder;
	cout << "Using " << modelName << endl;
	//GestureNetTF net(modelName, "input", "softmax");
	GestureNetTorch net(modelName, modelName);
	FeatureExtractor featExtractor;
	unordered_map<int, string> gestDict = {
		{0, "."},
		{1, "\nPred: Left Swing\n"},
		{2, "\nPred: Right Swing\n"},
		{3, "\nPred: Double Click\n"}
	};


	float totalTime = 0.0f;

	size_t numTrials = 100;
	vector<float> data(11, 0.0f);
	// std::iota(data.begin(), data.end(), fmod(rand(), 100));

	int i = 0;
	while (i < numTrials)
	{
		
		MatrixXf rawData;
		//std::iota(data.begin(), data.end(), fmod(rand(), 5));
		
		//cout << feat;
		if (recorder.readRdy())
		{
			i++;
			auto start = std::chrono::high_resolution_clock::now();
			rawData = recorder.readAudio();
			auto feat = featExtractor.extractFeature(rawData);
			int result = net.getPred(data);
			cout << gestDict[result];
			auto end = std::chrono::high_resolution_clock::now();

			totalTime += std::chrono::duration_cast<std::chrono::duration<float>>(end - start).count();
		}
		
	}

	cout << "Average time: " << totalTime / numTrials << endl;
}
