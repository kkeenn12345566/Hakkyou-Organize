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

#include <codecvt>

#include "copy_folder.h"

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
	const char *query = "SELECT parent, folder, path FROM song WHERE hash = ? ORDER BY folder";
	sqlite3_prepare_v2(db, query, strlen(query), &statement, NULL);
	
	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	
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
			
			wstring wpath = converter.from_bytes(path);
			BFS::path p(wpath);
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
	
	//find max folder
	map<string, string> hf_max;

	for (auto hf : hf_map) {
		int max_count = 0;
		for (auto f : hf.second)
			if (fh_count[f] > max_count) {
				hf_max[hf.first] = f;
				max_count = fh_count[f];
			}
	}
		
	
	/*
	//checking h-f list
	for (auto hf : hf_map)
		if (hf.second.size() > 1) {
			cout << hf.first << endl;
			for (auto f : hf.second) {
				//cout << setw(10) << f << setw(3) << fh_count[f];
				cout << setw(3) << fh_count[f] << " | " << fp_map[f].string()  << " | " << fp_map[f].stem().string();
				if (f == hf_max[hf.first]) cout << "   <<<" << endl;
				else cout << endl;
			}
			cout << endl;
		}			
	*/
	
	
	//choose folder	
	vector<string> f_max;
	
	for (auto hf : hf_map) {
		string hash = hf.first;
		if (is_contained(hf_max[hash], f_max)) continue;
		else {
			for (auto f : hf.second) 
				if (is_contained(f, f_max))
					cout << "Max Folder Mismatch!" << endl;
		}
		f_max.push_back(hf_max[hash]);
	}
	
	//checking f_max integrity
	vector<string> h_from_f_max;
	
	for (auto f : f_max)
		for (auto h : fh_map[f]) {
			if (!is_contained(h, h_from_f_max)) h_from_f_max.push_back(h);
			else cout << h << " multiple detected!" << endl;
		}
		
	for (auto hf : hf_map)
		if (!is_contained(hf.first, h_from_f_max)) cout << hf.first << " missing!" << endl;
	
	/*
	//checking chosen path & stem
	for (auto f : f_max)
		cout << setw(40) << fp_map[f].filename().string()  << " | " << fp_map[f].string() << endl;
	*/
	
	//COPY FOLDER!
	
	int inow = 0;
	int imax = f_max.size();
	for (auto f : f_max) {
		copy_folder(fp_map[f], BFS::path("OUTPUT"));
		//cout << "( " << ++inow << " / " << imax << " ) " << fp_map[f].filename() << endl;
		cout << "( " << ++inow << " / " << imax << " ) " << endl;
	}

	return 0;
}