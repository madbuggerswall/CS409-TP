#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using DictPair = std::pair<std::string, unsigned short>;

struct CSVHandler {
	std::fstream inputFile;

	CSVHandler(const std::string& filePath) {
		inputFile.open(filePath);
		if (!inputFile.is_open()) {
			std::perror("CSVHandler constructor");
		}
	}

	// RAII
	~CSVHandler() {
		inputFile.close();
		std::cout << "RAII" << std::endl;
	}

	// Read .csv data into a 2D std::vector of std::string.
	auto read() -> std::vector<std::vector<std::string>> {
		std::vector<std::string> fieldRows;
		std::vector<std::vector<std::string>> fields;
		std::string line;
		std::string field;
		std::stringstream lineStream;
		while (std::getline(inputFile, line)) {
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
};

struct Assistant {
	std::string name;
	unsigned short hoursToAssist;
	unsigned short maxCourses;
	std::vector<DictPair> assistedCourses;

	friend inline void swap(Assistant& first, Assistant& second);

	Assistant() = default;
	Assistant(const std::vector<std::string>& args)
			: name(args[0]),
				hoursToAssist(std::stoi(args[1])),
				maxCourses(std::stoi(args[2])) {
		for (short i = 3; i < args.size(); ++i) {
			appendAssistedCourses(args[i]);
		}
	}

	// Copy constructor.
	Assistant(const Assistant& other)
			: name(other.name),
				hoursToAssist(other.hoursToAssist),
				maxCourses(other.maxCourses),
				assistedCourses(other.assistedCourses) {
		std::cout << "Copy constructor." << std::endl;
	}

	// Move constructor.
	Assistant(Assistant&& other) : Assistant() {
		std::cout << "Move constructor." << std::endl;
		swap(*this, other);
	}

	// Copy assignment operator - commented out because it created ambiguity.
	Assistant& operator=(const Assistant& other) {
		std::cout << "Copy assignment operator." << std::endl;
		name = other.name;
		hoursToAssist = other.hoursToAssist;
		maxCourses = other.maxCourses;
		assistedCourses = other.assistedCourses;
		return *this;
	}

	// Move assignment operator.
	Assistant& operator=(Assistant&& other) {
		std::cout << "Move assignment operator." << std::endl;
		swap(*this, other);
		return *this;
	}

	friend inline void swap(Assistant& first, Assistant& second) {
		// Enable ADL.
		// using std::swap;
		std::swap(first.name, second.name);
		std::swap(first.hoursToAssist, second.hoursToAssist);
		std::swap(first.maxCourses, second.maxCourses);
		std::swap(first.assistedCourses, second.assistedCourses);
	}

	void appendAssistedCourses(const std::string& course) {
		auto criterion = [&course](const DictPair& p) { return (p.first == course); };
		auto search = std::find_if(assistedCourses.begin(), assistedCourses.end(), criterion);
		if (search == assistedCourses.end())
			assistedCourses.push_back(std::make_pair(course, 1));
		else
			search->second++;
		auto descending = [](const DictPair& a, const DictPair& b) {
			return a.second > b.second;
		};
		std::sort(assistedCourses.begin(), assistedCourses.end(), descending);
	}
};

struct Course {
	std::string courseID;
	std::string instructorName;
	unsigned short minTAHours;
	unsigned short maxTAHours;
	unsigned short minTACount;

	friend inline void swap(Course& first, Course& second);

	Course() = default;
	Course(const std::vector<std::string>& args) {
		courseID = args[0];
		instructorName = args[1];
		minTAHours = std::stoi(args[2]);
		maxTAHours = std::stoi(args[3]);
		minTACount = std::stoi(args[4]);
	}
	// Copy constructor.
	Course(const Course& other) : courseID(other.courseID),
																instructorName(other.instructorName),
																minTAHours(other.minTAHours),
																maxTAHours(other.maxTAHours),
																minTACount(other.minTACount) {
	}

	// Move constructor
	Course(Course&& other) : Course() {
		swap(*this, other);
	}

	Course& operator=(const Course& other) {
		courseID = other.courseID;
		instructorName = other.instructorName;
		minTAHours = other.minTAHours;
		maxTAHours = other.maxTAHours;
		minTACount = other.minTACount;
		return *this;
	}

	Course& operator=(Course&& other) {
		swap(*this, other);
		return *this;
	}

	friend inline void swap(Course& first, Course& second) {
		using std::swap;
		swap(first.courseID, second.courseID);
		swap(first.instructorName, second.instructorName);
		swap(first.minTAHours, second.minTAHours);
		swap(first.maxTAHours, second.maxTAHours);
		swap(first.minTACount, second.minTACount);
	}
};

struct Scheduler{
	
};

int main(int argc, char const* argv[]) {
	CSVHandler assistantsCSV{"../assistants.csv"};
	CSVHandler coursesCSV{"../courses.csv"};

	auto assistantData = assistantsCSV.read();
	auto courseData = coursesCSV.read();

	auto assistants = std::vector<Assistant>();
	auto courses = std::vector<Course>();

	for (auto dataRow : assistantData)
		assistants.push_back(Assistant(dataRow));

	auto descending = [](const Assistant& a, const Assistant& b) {
		return a.assistedCourses.size() > b.assistedCourses.size();
	};
	std::sort(assistants.begin(), assistants.end(), descending);

	for (auto assistant : assistants)
		std::cout << assistant.name << " " << assistant.assistedCourses.size() << std::endl;

	for (auto dataRow : courseData)
		courses.push_back(Course(dataRow));

	auto coursesDesc = [](const Course& a, const Course& b) {
		return a.minTAHours > b.minTAHours;
	};
	std::sort(courses.begin(), courses.end(), coursesDesc);

	for (auto course : courses)
		std::cout << course.courseID << " " << course.minTAHours << std::endl;

	return 0;
}
