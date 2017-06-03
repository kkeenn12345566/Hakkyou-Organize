#include "json.hpp"
#include "sqlite3.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using json = nlohmann::json;
using namespace std;

vector<string> good_parent = {"20cfa851","210dc266","234b7c3f","22891608","27c6008d","26046aba","2442d4e3","2580bed4","ac6d45fc","ac2538bf"};

bool is_contained(string text, vector<string> container) {
	for (auto _text : container)
		if (text == _text) return true;
	
	return false;
}

int main()
{
	// ----- Parse title-md5 json ----- //

	map<string, string> th_map;

	ifstream th_file("data\\data_json.cgi");
	json th_json;
	th_file >> th_json;

	for (auto th_unit : th_json)
		th_map[th_unit["title"]] = th_unit["md5"];

	// -------- Parse category -------- //



	// --------- Traverse db --------- //

	map<string, int> fm_count, fh_count;
	map<string, vector<string>> fh_map, hf_map;

	sqlite3 *db;
	int rc;

	rc = sqlite3_open("db\\song.db", &db);
	if (rc){
		cerr << "Can't open database!" << endl;
		return 0;
	} else {
		cerr << "Opened database successfully." << endl;
	}

	sqlite3_stmt *statement;
	const char *query = "SELECT parent, folder FROM song WHERE hash = ?";
	sqlite3_prepare_v2(db, query, strlen(query), &statement, NULL);
	
	for (auto th : th_map) {
		string hash = th.second;
		sqlite3_bind_text(statement, 1, hash.c_str(), -1, SQLITE_STATIC);

		//cout << th.first << endl;

		hf_map[hash] = {};
		
		while ((rc = sqlite3_step(statement)) == SQLITE_ROW) {
			string parent(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)));
			string folder(reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)));
			
			if (!is_contained(parent, good_parent)) continue;

			if (is_contained(folder, hf_map[hash])) {
				cout << "Hash hit dectected: " << folder << " " << hash << endl;
				cout << "...in " << th.first << endl;
			} else {
				fh_count[folder]++;
				hf_map[hash].push_back(folder);
			}
			//cout << folder << " ";
		}
		cout << endl;

		if (rc != SQLITE_DONE)
			cerr << "Error while travesing!" << endl;

		sqlite3_reset(statement);
	}

	sqlite3_close(db);


	return 0;
}