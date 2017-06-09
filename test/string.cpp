#include <iostream>
#include <string>

#include "copyFolder.h"

using namespace std;

int main() {
	string str("asdf\\");
	
	//for (auto chr : str) cout << int(chr) << endl;
	//cout << "Str length: " << str.length() << endl;
	
	if (!copyDir(str.c_str(), "羅鈺凱\\", true)) cout << "Error!" << endl;
	
	return 0;
}