#pragma once

#include "StudentDAO.h"
#include <string>
#include <iostream>

class StudentController {
private:
    StudentDAO studentDAO;

    bool validateStudentData(const std::string& data) {
        std::istringstream iss(data);
        std::string firstName, lastName;

        std::getline(iss, firstName, ',');
        std::getline(iss, lastName, ',');
        if (firstName.empty() || lastName.empty()) {
            return false; // Проверка на пустые поля
        }
        return true;
    }

public:
    void addStudent(const std::string& data) {
        if (!validateStudentData(data)) {
            throw std::invalid_argument("Invalid student data.");
        }

        std::istringstream iss(data);
        std::string firstName, lastName, group, gradesInput;

        std::getline(iss, firstName, ',');
        std::getline(iss, lastName, ',');
        std::getline(iss, group, ',');
        std::getline(iss, gradesInput);

        std::map<std::string, int> grades;
        std::istringstream gradesStream(gradesInput);
        std::string gradeEntry;
        while (std::getline(gradesStream, gradeEntry, ';')) {
            gradeEntry.erase(std::remove_if(gradeEntry.begin(), gradeEntry.end(), ::isspace), gradeEntry.end());
            if (gradeEntry.empty()) {
                continue;
            }

            std::string subject, grStr;
            int grade;
            std::istringstream entryStream(gradeEntry);
            std::getline(entryStream, subject, ':');
            entryStream >> grStr;
            grade = std::stoi(grStr);
            grades[subject] = grade;
        }

        StudentModel student = StudentModel(firstName, lastName, group, grades);
        studentDAO.add(student);
    }

    std::string displayStudents() {
        std::stringstream ss;
        std::vector<StudentModel> students = studentDAO.getAll();
        for (const auto& student : students) {
            ss << student.serialize() << "\n";
        }
        return ss.str();
    }
};

