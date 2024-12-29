#pragma once

#include "StudentDAO.h"

class StudentController {
private:
    StudentDAO studentDAO;

public:
    void addStudent(const std::string& data) {
        std::istringstream iss(data);
        std::string firstName, lastName, group, gradesInput;

        std::getline(iss, firstName, ',');
        std::getline(iss, lastName, ',');
        std::getline(iss, group, ',');
        std::getline(iss, gradesInput);

        std::map<std::string, int> grades;
        std::istringstream gradesStream(gradesInput);
        std::string gradeEntry;
        while (std::getline(gradesStream, gradeEntry, ',')) {
            std::string subject;
            int grade;
            std::istringstream entryStream(gradeEntry);
            std::getline(entryStream, subject, ':');
            entryStream >> grade;
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

