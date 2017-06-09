#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <locale>
namespace BFS = boost::filesystem;

#include "copy_folder.h"

using namespace std;

int main()
{
	locale loc("");
	locale::global(loc);
	BFS::path::imbue(locale());
	
	copy_folder(BFS::path("あ"), BFS::path("OUTPUT"));
}