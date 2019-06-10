#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
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
				assistedCourses(other.assistedCourses) {}

	// Move constructor.
	Assistant(Assistant&& other) : Assistant() {
		swap(*this, other);
	}

	// Copy assignment operator - commented out because it created ambiguity.
	Assistant& operator=(const Assistant& other) {
		name = other.name;
		hoursToAssist = other.hoursToAssist;
		maxCourses = other.maxCourses;
		assistedCourses = other.assistedCourses;
		return *this;
	}

	// Move assignment operator.
	Assistant& operator=(Assistant&& other) {
		swap(*this, other);
		return *this;
	}

	friend inline void swap(Assistant& first, Assistant& second) {
		// Enable ADL.
		using std::swap;
		swap(first.name, second.name);
		swap(first.hoursToAssist, second.hoursToAssist);
		swap(first.maxCourses, second.maxCourses);
		swap(first.assistedCourses, second.assistedCourses);
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
	Course(const Course& other)
			: courseID(other.courseID),
				instructorName(other.instructorName),
				minTAHours(other.minTAHours),
				maxTAHours(other.maxTAHours),
				minTACount(other.minTACount) {}

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

struct ScheduleEntry {
	Course course;
	std::vector<std::pair<Assistant, unsigned short>> assignedTAs;

	friend inline void swap(ScheduleEntry& first, ScheduleEntry& second);

	ScheduleEntry() = default;
	ScheduleEntry(Course course, Assistant assistant, u_short assignedTime) : course(course) {
		assignedTAs.push_back(std::make_pair(assistant, assignedTime));
	}

	// Copy constructor
	ScheduleEntry(const ScheduleEntry& other) : course(other.course), assignedTAs(other.assignedTAs) {}
	// Move constructor.
	ScheduleEntry(ScheduleEntry&& other) : ScheduleEntry() {
		swap(*this, other);
	}

	ScheduleEntry& operator=(const ScheduleEntry& other) {
		course = other.course;
		assignedTAs = other.assignedTAs;
		return *this;
	}

	ScheduleEntry& operator=(ScheduleEntry&& other) {
		swap(*this, other);
		return *this;
	}

	friend inline void swap(ScheduleEntry& first, ScheduleEntry& second) {
		using std::swap;
		swap(first.course, second.course);
		swap(first.assignedTAs, second.assignedTAs);
	}

	void assignTA(const Course& course, const Assistant& assistant, u_short time);
};

struct Schedule {
	std::vector<ScheduleEntry> timetable;

	friend inline void swap(Schedule& first, Schedule& second);

	Schedule() = default;
	Schedule(std::vector<ScheduleEntry> timetable) : timetable(timetable) {}
	Schedule(const Schedule& other) : timetable(other.timetable) {}
	Schedule(Schedule&& other) : Schedule() {
		swap(*this, other);
	}

	Schedule& operator=(const Schedule& other) {
		timetable = other.timetable;
		return *this;
	}

	Schedule& operator=(Schedule&& other) {
		swap(*this, other);
		return *this;
	}

	friend inline void swap(Schedule& first, Schedule& second) {
		using std::swap;
		swap(first.timetable, second.timetable);
	}

	void appendTimeTable(const ScheduleEntry& entry) {
		auto criterion = [&entry](const ScheduleEntry& s) {
			return (s.course.courseID == entry.course.courseID);
		};
		auto search = std::find_if(timetable.begin(), timetable.end(), criterion);
		if (search == timetable.end())
			timetable.push_back(entry);
		else
			search->assignedTAs.insert(search->assignedTAs.end(),
																 entry.assignedTAs.begin(),
																 entry.assignedTAs.end());
	}
};

auto generateCombinations(unsigned short n, unsigned short r) {
	std::vector<unsigned short> indexes;
	std::vector<std::vector<unsigned short>> combinations;
	std::vector<bool> v(n);
	std::fill(v.begin(), v.begin() + r, true);
	do {
		for (int i = 0; i < n; ++i)
			if (v[i])
				indexes.push_back(i);
		combinations.push_back(indexes);
		indexes.clear();
	} while (std::prev_permutation(v.begin(), v.end()));
	return combinations;
}

auto combination(int n, int r) {
	auto result = 1.0f;
	auto denom = std::fmin(r, n - r);
	auto limit = std::fmax(r, n - r);
	for (; n > limit; --n) {
		result *= n / (denom--);
	}
	return (int)result;
}

auto combowrep(int n, int r) {
	auto sum = [](std::vector<int> a) {
		return std::accumulate(a.begin(), a.end(), 0);
	};
	std::vector<int> result(r, 1);

	int index = r - 1;
	for (int i = 0; i < combination(n, r); i++) {
		for (auto item : result) {
			std::cout << item << " ";
		}
		std::cout << std::endl;

		if (sum(result) < n)
			result[index]++;
		else {
			int resetIndex = index;
			while (sum(result) >= n) {
				// std::cout << "k: " << k << std::endl;
				result[resetIndex] = 1;
				result[resetIndex - 1]++;
				resetIndex--;
				if (sum(result) == n)
					break;
			}
		}
	}
}

int main(int argc, char const* argv[]) {
	combination(10,3);
	// combowrep(10, 3);
	CSVHandler assistantsCSV{"../assistants.csv"};
	CSVHandler coursesCSV{"../courses.csv"};

	auto assistantData = assistantsCSV.read();
	auto courseData = coursesCSV.read();

	auto assistants = std::vector<Assistant>();
	auto courses = std::vector<Course>();

	for (auto dataRow : assistantData)
		assistants.push_back(Assistant(dataRow));

	// auto descending = [](const Assistant& a, const Assistant& b) {
	// 	return a.assistedCourses.size() > b.assistedCourses.size();
	// };
	// std::sort(assistants.begin(), assistants.end(), descending);

	for (auto dataRow : courseData)
		courses.push_back(Course(dataRow));

	auto coursesDesc = [](const Course& a, const Course& b) {
		return a.minTAHours > b.minTAHours;
	};
	std::sort(courses.begin(), courses.end(), coursesDesc);
	auto start = std::chrono::system_clock::now();

	for (auto assistant : assistants) {
		for (auto maxCourses = 1; maxCourses <= assistant.maxCourses; ++maxCourses) {
			auto courseSelection = generateCombinations(courses.size(), maxCourses);
			for (auto row : courseSelection) {
				for (int i = 0; i < combination(assistant.hoursToAssist / 5, row.size()); i++) {
					for (int j = 0; j < row.size(); j++) {
					}
				}
			}
		}
		break;
	}

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsedTime = end - start;
	std::cout << "Elapsed time: " << elapsedTime.count() << std::endl;
	return 0;
}
