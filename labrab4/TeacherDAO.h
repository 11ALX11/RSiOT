#pragma once

#include <vector>
#include <fstream>
#include "TeacherModel.h"

class TeacherDAO {
public:
    void add(TeacherModel& teacher) {
        std::ofstream file("teachers.txt", std::ios::app);
        if (teacher.id == 0)
            teacher.id = getNextId();
        file << teacher.serialize() << "\n";
    }

    std::vector<TeacherModel> getAll() {
        std::ifstream file("teachers.txt");
        std::string line;
        std::vector<TeacherModel> teachers;

        while (std::getline(file, line)) {
            teachers.push_back(TeacherModel::deserialize(line));
        }
        return teachers;
    }

private:
    int getNextId() {
        std::ifstream file("teachers.txt");
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
