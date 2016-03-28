#ifndef __FILEIO_H__
#define __FILEIO_H__

#include <boost/filesystem.hpp>
#include <cstdio>

namespace FileIO
{
    bool fileExists(boost::filesystem::path fpath);
    boost::filesystem::path root();
}

#endif
