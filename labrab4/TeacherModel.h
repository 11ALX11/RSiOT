#pragma once
#include <string>
#include <vector>
#include <sstream>

// преподавателям(имя, фамилия, преподаваемые предметы, возраст и т.д.)
class TeacherModel {
public:
    int id;
    std::string firstName;
    std::string lastName;
    std::vector<std::string> subjects;
    int age;

    TeacherModel(const std::string& fn, const std::string& ln, int a, const std::vector<std::string>& sub)
        : id(0), firstName(fn), lastName(ln), age(a), subjects(sub) {}

    TeacherModel(int id, const std::string& fn, const std::string& ln, int a, const std::vector<std::string>& sub)
        : id(id), firstName(fn), lastName(ln), age(a), subjects(sub) {}

    std::string serialize() const {
        std::ostringstream oss;
        oss << id << "," << firstName << "," << lastName << "," << age << ",";
        for (const auto& subject : subjects) {
            oss << subject << ";";
        }
        return oss.str();
    }

    static TeacherModel deserialize(const std::string& data) {
        std::istringstream iss(data);
        std::string idStr, fn, ln, sub, ageStr;
        std::getline(iss, idStr, ',');
        std::getline(iss, fn, ',');
        std::getline(iss, ln, ',');
        std::getline(iss, ageStr, ',');
        int id = std::stoi(idStr);
        int age = std::stoi(ageStr);
        std::vector<std::string> subjects;
        while (std::getline(iss, sub, ';')) {
            subjects.push_back(sub);
        }
        return TeacherModel(id, fn, ln, age, subjects);
    }
};
