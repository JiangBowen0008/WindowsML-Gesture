#include "GestureNet.hpp"

GestureNet::GestureNet( string modelName,
                        string inputName,
                        string outputName):
                model_{modelName},
                input_{model_, inputName},
                output_{model_, outputName}
{}

int GestureNet::getPred(vector<float> &_input)
{
    input_.set_data(_input);
    model_.run({&input_}, output_);
    auto results =  output_.get_data<float>();

    int maxIdx = distance(results.begin(),
                            max_element(results.begin(),
                                        results.end()));
    DBOUT << "Pred: " << maxIdx << "\t(";
    for (float f : results) {
        DBOUT << f << " ";
    }
    DBOUT << ")" << endl;

    return 0;
}