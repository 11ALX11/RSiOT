#pragma once
#include "TeacherDAO.h"



class TeacherController {
private:
    TeacherDAO teacherDAO;

    bool validateTeacherData(const std::string& data) {
        std::istringstream iss(data);
        std::string firstName, lastName;
        int age;

        std::getline(iss, firstName, ',');
        std::getline(iss, lastName, ',');
        if (!(iss >> age)) return false; // �������� �� ������������ ��������

        if (firstName.empty() || lastName.empty() || age < 0) {
            return false; // �������� �� ������ ���� � ������������ �������
        }
        return true;
    }

public:
    void addTeacher(const std::string& data) {
        if (!validateTeacherData(data)) {
            throw std::invalid_argument("Invalid teacher data.");
        }

        std::istringstream iss(data);
        std::string firstName, lastName, subjectsInput;
        int age;

        std::getline(iss, firstName, ',');
        std::getline(iss, lastName, ',');
        iss >> age;
        iss.ignore(); // ���������� ������� ����� ��������
        std::getline(iss, subjectsInput);

        std::vector<std::string> subjects;
        std::istringstream subjectStream(subjectsInput);
        std::string subject;
        while (std::getline(subjectStream, subject, ';')) {
            subject.erase(std::remove_if(subject.begin(), subject.end(), ::isspace), subject.end());
            if (subject.empty()) {
                continue;
            }
            subjects.push_back(subject);
        }

        TeacherModel teacher = TeacherModel(firstName, lastName, age, subjects);
        teacherDAO.add(teacher);
    }

    std::string displayTeachers() {
        std::stringstream ss;
        std::vector<TeacherModel> teachers = teacherDAO.getAll();
        for (const auto& teacher : teachers) {
            ss << teacher.serialize() << "\n";
        }
        return ss.str();
    }
};

