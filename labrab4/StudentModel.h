#pragma once
#include <string>
#include <map>
#include <sstream>

// по студентам(имя, фамилия, списки предметов с оценками по ним, название группы и т.д.)
class StudentModel {
public:
    int id;
    std::string firstName;
    std::string lastName;
    std::map<std::string, int> grades; // subject -> grade
    std::string group;

    StudentModel(const std::string& fn, const std::string& ln, const std::string& g,
        const std::map<std::string, int>& gr)
        : id(0), firstName(fn), lastName(ln), group(g), grades(gr) {}
    
    StudentModel(int id, const std::string& fn, const std::string& ln, const std::string& g,
        const std::map<std::string, int>& gr)
        : id(id), firstName(fn), lastName(ln), group(g), grades(gr) {}

    std::string serialize() const {
        std::ostringstream oss;
        oss << id << "," << firstName << "," << lastName << "," << group << ",";
        for (const auto& pair : grades) {
            oss << pair.first << ":" << pair.second << ";";
        }
        return oss.str();
    }

    static StudentModel deserialize(const std::string& data) {
        std::istringstream iss(data);
        std::string idStr, fn, ln, group, gradeStr;
        std::getline(iss, idStr, ',');
        std::getline(iss, fn, ',');
        std::getline(iss, ln, ',');
        std::getline(iss, group, ',');
        int id = std::stoi(idStr);
        std::map<std::string, int> grades;
        while (std::getline(iss, gradeStr, ';')) {
            std::string subject;
            int grade;
            std::istringstream gradeStream(gradeStr);
            std::getline(gradeStream, subject, ':');
            gradeStream >> grade;
            grades[subject] = grade;
        }
        return StudentModel(id, fn, ln, group, grades);
    }
};
