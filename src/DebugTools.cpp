#include "DebugTools.h"

template<>
ostream& operator<<(ostream& os, const vector<int>& val);

template<>
ostream& operator<<(ostream& os, const vector<double>& val);

template<>
ostream& operator<<(ostream& os, const vector<string>& val);
