#include "FileIO.h"

using namespace boost::filesystem;

bool FileIO::fileExists(path fpath)
{
    FILE* f = fopen(fpath.c_str(), "rb");
    bool doesExist = f;
    fclose(f);
    return doesExist;
}

path FileIO::root()
{
#ifdef __APPLE__
    return "/Users/Brian/Magnate";
#elif _WIN32
    return initial_path() / "../..";
#elif __linux
    return initial_path() / "../..";
#endif
}
