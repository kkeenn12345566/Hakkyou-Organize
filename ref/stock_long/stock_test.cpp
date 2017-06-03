#include <iostream>
#include <string>
#include <fstream>
#include <curl/curl.h>
#include <vector>

#include <unistd.h>

using namespace std;
/*
typedef struct csv_row_struct
{
	vector<string> data;
}Csv_row;

typedef struct csv_data_struct
{
	Csv_row csv_header;
	vector<Csv_row> data;
}Csv_data;
*/
size_t find_token(const string data, const string tok, const string tokend, size_t start, string* result)
{
	string tag = "<" + tok;
	string tagend = "<" + tokend + ">";
	
	size_t pos = data.find(tag, start);
	if (pos == string::npos) return pos;
	pos = data.find(">", pos) + 1;
	size_t posend = data.find(tagend, pos);
	
	*result = data.substr(pos, posend - pos);
	return posend + tagend.length();
}

size_t find_token2(const string data, const string tok, size_t start, string* result)
{
	return find_token(data, tok, "/" + tok, start, result);
}

size_t parse_token(const string data, const string main_tok, const string sub_tok, size_t start, ostream& csv_file)
{
	string result, result_sub;
	size_t pos = find_token2(data, main_tok, start, &result);
	if (pos == string::npos) return pos;
	size_t posend = pos;
	pos = 0;
	//cout << "parsing|" << result << "(with)" << sub_tok << "|" << endl;
	do
	{
		//cout << "(at)" << pos;
		pos = find_token2(result, sub_tok, pos, &result_sub);
		if (pos != string::npos)
		{
			csv_file << result_sub << "\t";
		} else break;
	}while(true);
	
	csv_file << endl;
	
	return posend;
}

void parse_table_to_data(string htm_code, ofstream& csv_file)
{	
	string result;
	
	find_token(htm_code, "table enableviewstate=\"false\"", "/table", 0, &result);

	size_t pos = parse_token(result, "tr", "th", 0, cout);
	do
	{
		//Csv_row csv_row;
		pos = parse_token(result, "tr", "td", pos, csv_file);
	}while(pos != string::npos);
	
}
/*
void output_to_csv(Csv_data csv_data)
{
	ofstream csv_file;
	csv_file.open("auto_grab.csv");
	for (int j = 0 ; j < csv_data.csv_header.data.size() ; j++){
		csv_file << csv_data.csv_header.data[j] << ",";
	}
	csv_file << endl;
	for (int i = 0 ; i < csv_data.data.size() ; i++){
		Csv_row csv_row = csv_data.data[i];
		for (int j = 0 ; j < csv_row.data.size() ; j++){
			csv_file << csv_row.data[j] << ",";
		}
		csv_file << endl;
	}
	csv_file.close();
}
*/
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void curl_get_htm_code(string* htm_code)
{
	CURL* curl;
	CURLcode res;
  
	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "http://www.cnyes.com/twstock/ps_historyprice/2330.htm");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, htm_code);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
}

int main()
{	
	string htm_code;
	ofstream csv_file;
	csv_file.open("auto_grab.xls");
	curl_get_htm_code(&htm_code);
	parse_table_to_data(htm_code, csv_file);	
	//output_to_csv(csv_data);
	
	return 0;
}