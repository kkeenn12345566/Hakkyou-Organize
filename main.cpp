#include "json.hpp"
#include "sqlite3.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <locale>
namespace BFS = boost::filesystem;

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
	locale loc("");
	locale::global(loc);
	BFS::path::imbue(locale());
	
	// ----- Parse title-md5 json ----- //

	map<string, string> th_map;

	ifstream th_file("data\\data_json.cgi");
	json th_json;
	th_file >> th_json;

	for (auto th_unit : th_json)
		th_map[th_unit["title"]] = th_unit["md5"];

	// -------- Parse category -------- //

	// --------- Traverse db --------- //

	// f: folder, h: hash, p: path
	map<string, int> fh_count;
	map<string, BFS::path> fp_map;
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
	const char *query = "SELECT parent, folder, path FROM song WHERE hash = ?";
	sqlite3_prepare_v2(db, query, strlen(query), &statement, NULL);
	
	for (auto th : th_map) {
		string hash = th.second;
		sqlite3_bind_text(statement, 1, hash.c_str(), -1, SQLITE_STATIC);

		//cout << th.first << endl;
		
		int count = 0;
		while ((rc = sqlite3_step(statement)) == SQLITE_ROW) {
			string parent(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)));
			string folder(reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)));
			string   path(reinterpret_cast<const char*>(sqlite3_column_text(statement, 2)));
			
			if (!is_contained(parent, good_parent)) continue;

			// Hash hit manually removed. (auto?)
			if (is_contained(folder, hf_map[hash])) {
				cout << "Hash hit dectected: " << folder << " " << hash << endl;
				cout << "Title: " << th.first << endl;
				cout << "Path: " << path << endl << endl;
			} else {
				fh_count[folder]++;
				hf_map[hash].push_back(folder);
				fh_map[folder].push_back(hash);
			}
			
			BFS::path p(path);
			if (fp_map[folder] == BFS::path())
				fp_map[folder] = p.parent_path();
				//fp_map[folder] = p;
			
			//cout << folder << " ";
			
			++count;
		}
		//cout << endl;
		
		if (count == 0) 
			//cout << th.first << " NOT FOUND!" << endl;

		if (rc != SQLITE_DONE)
			cerr << "Error while travesing!" << endl;

		sqlite3_reset(statement);
	}

	sqlite3_close(db);
	
	// -------- Folder Choose -------- //
	
	/*
	cout << endl;
	for (auto fp : fp_map)
		cout << setw(10) << fp.first << "\t|\t" << fp.second.string() << endl;
	*/
	
	for (auto hf : hf_map)		
		if (hf.second.size() > 1) {
			cout << hf.first << endl;
			for (auto f : hf.second)
				//cout << setw(10) << f << setw(3) << fh_count[f];
				cout << setw(3) << fh_count[f] << " | " << fp_map[f].string() << endl;
			cout << endl;
		}
			


	return 0;
}