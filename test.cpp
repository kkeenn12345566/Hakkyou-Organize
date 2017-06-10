#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <locale>
namespace BFS = boost::filesystem;

#include <codecvt>
#include <string>

#include "copy_folder.h"

using namespace std;

int main()
{
	locale loc("");
	locale::global(loc);
	BFS::path::imbue(locale());
	
	string s("あ");
	
	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	wstring ws = converter.from_bytes(s);
	
	copy_folder(BFS::path(ws), BFS::path("OUTPUT"));
}