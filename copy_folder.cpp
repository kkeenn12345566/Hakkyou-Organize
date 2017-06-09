#include <iostream>
#include <string>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <locale>
namespace BFS = boost::filesystem;

using namespace std;

void copy_folder(BFS::path src, BFS::path dst) {
	
	locale loc("");
	locale::global(loc);
	BFS::path::imbue(locale());
	
	//if (BFS::exists(src)) {
		//if (BFS::is_directory(src)) {
			//cout << src << " is a directory containing:" << endl;
			for (auto& entry : boost::make_iterator_range(BFS::directory_iterator(src), {})) {
				//cout << entry << endl;
				if (!is_regular_file(entry.path())) continue;
				BFS::path file = entry.path().filename();
				BFS::path folder = src.filename();
				BFS::create_directory(dst/folder);
				BFS::copy_file(entry.path(), dst/folder/file);
			}
		//}
		//else
			//cout << src << " exists, but is neither a regular file nor a directory\n";
	//} else
		//cout << src << " does not exist\n";//
}	