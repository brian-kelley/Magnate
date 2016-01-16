#include "FileIO.h"

using namespace std;

#ifdef __APPLE__

const string FileIO::root = "../..";

#elif _WIN32

const string FileIO::root = "../..";

#elif __linux

const string FileIO::root = "../..";

#endif