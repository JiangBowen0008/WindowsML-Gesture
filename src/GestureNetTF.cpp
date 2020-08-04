#include "GestureNetTF.hpp"

GestureNetTF::GestureNetTF( string modelName,
                            string inputName,
                            string outputName):
                        model_{modelName},
                        input_{model_, inputName},
                        output_{model_, outputName},
                        lstm1_{ model_, "biLSTM_1_Reshape" },
                        lstm2_{ model_, "biLSTM_2_Reshape" }
{}

int GestureNetTF::getPred(vector<float> &_input)
{
    input_.set_data(_input);
    model_.run({ &input_ }, { &lstm1_, &lstm2_, &output_ });
    //cout << "temp" << lstm1_.get_data<float>() << endl;
    auto preds =  output_.get_data<float>();

    return rankPred(preds);
}