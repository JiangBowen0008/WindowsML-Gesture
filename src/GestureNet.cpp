# include "GestureNet.hpp"

int GestureNet::rankPred(vector<float>& predictions)
{
    int maxIdx = distance(  predictions.begin(),
                            max_element(predictions.begin(),
                                        predictions.end()));
    DBOUT << "Pred: " << maxIdx << "\t(";
    for (float f : predictions) {
        DBOUT << f << " ";
    }
    DBOUT << ")" << endl;

    return maxIdx;
}