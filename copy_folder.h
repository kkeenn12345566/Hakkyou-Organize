#ifndef COPY_FOLDER_H
#define COPY_FOLDER_H

#include <iostream>
#include <string>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <locale>
namespace BFS = boost::filesystem;

void copy_folder(BFS::path src, BFS::path dst);

#endif