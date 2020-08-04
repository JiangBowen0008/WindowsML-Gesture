#include "utils.hpp"

ostream& operator<<(ostream& os, const vector<float>& v)
{
    for (auto& it : v)
    {
        os << it << "\t";
    }
    os << endl;
    return os;
}
