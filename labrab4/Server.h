#pragma once

#include "TeacherController.h"
#include "StudentController.h"

#include <iostream>
#include <string>
#include <thread>


class Server {
private:
    boost::asio::io_context io_context;
    tcp::acceptor acceptor;
    TeacherController teacherController;
    StudentController studentController;

public:
    Server(int port) : acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {
        startAccept();
    }

    void startAccept() {
        auto socket = std::make_shared<tcp::socket>(io_context);
        acceptor.async_accept(*socket, [this, socket](const boost::system::error_code& error) {
            if (!error) {
                std::thread(&Server::handleClient, this, socket).detach();
            }
            startAccept();
            });
    }

    void handleClient(std::shared_ptr<tcp::socket> socket) {
        try {
            char buffer[1024];
            boost::system::error_code error;

            while (true) {
                size_t length = socket->read_some(boost::asio::buffer(buffer), error);
                if (error) break;

                std::string request(buffer, length);
                std::string response;

                if (request.substr(0, 12) == "ADD_TEACHER") {
                    teacherController.addTeacher(request.substr(12));
                    response = "Teacher added.\n";
                }
                else if (request.substr(0, 13) == "GET_TEACHERS") {
                    response = teacherController.displayTeachers();
                }
                else if (request.substr(0, 12) == "ADD_STUDENT") {
                    studentController.addStudent(request.substr(12));
                    response = "Student added.\n";
                }
                else if (request.substr(0, 13) == "GET_STUDENTS") {
                    response = studentController.displayStudents();
                }
                else {
                    response = "Unknown command.\n";
                }

                boost::asio::write(*socket, boost::asio::buffer(response), error);
                if (error) break;
            }
        }
        catch (std::exception& e) {
            std::cerr << "Exception in client handler: " << e.what() << "\n";
        }
    }

    void run() {
        io_context.run();
    }
};