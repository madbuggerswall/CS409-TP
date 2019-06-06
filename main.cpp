#include <algorithm>
#include <any>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

using DictPair = std::pair<std::string, unsigned short>;

struct CSVHandler {
	std::fstream inputStream;

	CSVHandler(std::string filePath) {
		inputStream.open(filePath);

		if (!inputStream.is_open()) {
			std::perror("CSVHandler constructor");
		}
	}

	auto read() -> std::vector<std::vector<std::string>> {
		std::vector<std::string> fieldRows;
		std::vector<std::vector<std::string>> fields;
		std::string line;
		std::string field;
		std::stringstream lineStream;
		while (std::getline(inputStream, line)) {
			lineStream = std::stringstream{line};
			while (std::getline(lineStream, field, ',')) {
				field.erase(std::remove(field.begin(), field.end(), ' '), field.end());
				fieldRows.push_back(field);
			}
			fields.push_back(fieldRows);
			fieldRows.clear();
		}
		return fields;
	}

	// RAII
	~CSVHandler() {
		inputStream.close();
		std::cout << "RAII" << std::endl;
	}
};

struct Assistant {

	std::string name;
	unsigned short maxCourses;
	std::vector<DictPair> assistedCourses;

	Assistant(const std::vector<std::string> &args) {
		name = args[0];
		maxCourses = std::stoi(args[1]);
		for (short i = 2; i < args.size(); ++i) {
			appendAssistedCourses(args[i]);
		}
	}

	void appendAssistedCourses(std::string course) {
		auto criterion = [&course](const DictPair &p) {
			return (p.first == course);
		};
		auto search = std::find_if(assistedCourses.begin(), assistedCourses.end(), criterion);
		if (search == assistedCourses.end())
			assistedCourses.push_back(std::make_pair(course, 1));
		else
			search->second++;
		auto descending = [](DictPair &a, DictPair &b) { return a.second > b.second; };
		std::sort(assistedCourses.begin(), assistedCourses.end(), descending);
	}

	void sortAssistedCourses() {
	}
};

struct Course {
	std::string courseID;
	std::string instructorName;
	unsigned short minTAHours;
	unsigned short maxTAHours;
	unsigned short minTACount;
};

int main(int argc, char const *argv[]) {
	CSVHandler assistantsCSV{"../assistants.csv"};
	auto params = assistantsCSV.read();
	Assistant a(params[0]);
	std::cout << a.name << " - " << a.maxCourses << std::endl;
	std::cout << a.assistedCourses[1].first << std::endl;
	std::cout << a.assistedCourses[1].second << std::endl;
	std::cout << a.assistedCourses[2].first << std::endl;
	std::cout << a.assistedCourses[2].second << std::endl;

	for (auto fieldRow : params) {
		for (auto field : fieldRow) {
		}
	}
	return 0;
}
