#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <fstream>
#include <sstream>
#include <utility>
#include <tuple>
#include<any>
#include <algorithm>

using DictPair = std::pair<std::string, unsigned short>;

struct CSVHandler{
	std::fstream inputStream;
	
	CSVHandler(std::string filePath){
		inputStream.open(filePath);
		
		if(!inputStream.is_open()){
			std::perror("CSVHandler constructor");
		}
	}

	auto read(){
		std::tuple<std::any> fields;
		std::string line;
		while(std::getline(inputStream, line)){
			std::stringstream lineStream{line};
			std::string field;
			while(std::getline(lineStream, field, ',')){
				field.erase (std::remove (field.begin(), field.end(), ' '), field.end());
				fields = std::tuple_cat(fields, std::make_tuple(field));
				std::cout << field << std::endl;
			}
		}
		return fields;
	}	

	// RAII
	~CSVHandler(){
		inputStream.close();
		std::cout << "RAII" << std::endl;
	}
};

struct Assistant{
	
	std::string name;
	unsigned short maxCourses;
	std::vector<DictPair> assistedCourses;

	template<typename ...Ts>
	Assistant() {
		
	}
	template<typename F, typename S, typename ...Ts>
	Assistant(F name, S maxCourses, Ts... args) : name{name}, maxCourses{maxCourses} {
		(appendAssistedCourses(args),...);
	}

	void appendAssistedCourses(std::string course){
		auto criterion = [&course](const DictPair& p){
		if(p.first == course)
			return true;
		else
			return false;
		};
		auto search = std::find_if(assistedCourses.begin(), assistedCourses.end(), criterion);
		if(search == assistedCourses.end())
			assistedCourses.push_back({course, 1});
		else
			search->second++;
	}
};

struct Course{
	std::string courseID;
	std::string instructorName;
	unsigned short minTAHours;
	unsigned short maxTAHours;
	unsigned short minTACount;

};

int main(int argc, char const *argv[])
{
	CSVHandler assistantsCSV{"../assistants.csv"};
	auto params = assistantsCSV.read();
	Assistant a(params);
	// std::vector<std::pair<std::string, unsigned short>>test;
	// test.push_back(std::make_pair("Smith", 1)); 
	// test.push_back({"Stuff", 3}); 
	// test.push_back({"Chippendale", 4}); 
	// test.push_back({"Doe", 2}); 

	// std::string course = "Stuff";
	// auto criterion = [&course](const DictPair& p){
	// 	if(p.first == course)
	// 		return true;
	// 	else
	// 		return false;
	// };
	// auto search = std::find_if(test.begin(), test.end(), criterion);
	// search->second++;
	// std::cout << "FOUND - " << search->first << ": " << search->second << std::endl;
	// auto descending = [](DictPair& a, DictPair& b){
	// 	return a.second > b.second;
	// };

	// std::sort(test.begin(), test.end(), descending);
	// for(auto pair : test){
	// 	std::cout << pair.first << ": " << pair.second << std::endl;
	// }
	return 0;
}
