#include "FileIO.h"

using namespace boost::filesystem;

path FileIO::root()
{
#ifdef __APPLE__
    return initial_path() / "../..";
#elif _WIN32
    return initial_path() / "../..";
#elif __linux
    return initial_path() / "../..";
#endif
}