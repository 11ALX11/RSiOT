#pragma once

#include <fstream>
#include <vector>
#include "StudentModel.h"

class StudentDAO {
public:
    void add(StudentModel& student) {
        std::ofstream file("students.txt", std::ios::app);
        if (student.id == 0) 
            student.id = getNextId();
        file << student.serialize() << "\n";
    }

    std::vector<StudentModel> getAll() {
        std::ifstream file("students.txt");
        std::string line;
        std::vector<StudentModel> students;

        while (std::getline(file, line)) {
            students.push_back(StudentModel::deserialize(line));
        }
        return students;
    }

private:
    int getNextId() {
        std::ifstream file("students.txt");
        std::string line;
        int maxId = 0;

        while (std::getline(file, line)) {
            int id = std::stoi(line.substr(0, line.find(',')));
            if (id > maxId) {
                maxId = id;
            }
        }
        return maxId + 1;
    }
};
