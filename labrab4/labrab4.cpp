
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>

#include "TeacherController.h"
#include "StudentController.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 2048
#define DEFAULT_PORT "9090"

/**
* Необходимо разработать сервер для хранения данных по заданному варианту.
* Данный сервер будет использоваться в последующих лабораторных работах.
* Хранение данных необходимо организовать в собственных файлах, т.е. без использования БД.
* Полный список хранимых данных должен получаться через использование GET-запросов (на разные списки – разные запросы).
*
* 2) Написать сервер для хранения данных по университету.
* Необходимо хранить данные по
* преподавателям (имя, фамилия, преподаваемые предметы, возраст, часы и даты занятий и т.д.)
* и по студентам (имя, фамилия, списки предметов с оценками по ним, название группы и т.д.)
*/
int __cdecl main(void)
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    TeacherController teacherController;
    StudentController studentController;

    // loop inf
    while (true) {

        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        // Receive until the peer shuts down the connection
        do {
            iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
            if (iResult > 0) {
                printf("Bytes received: %d\n", iResult);
                recvbuf[iResult] = '\0'; // Добавляем нуль-терминатор

                std::string request(recvbuf);
                std::string response;

                std::cout << request;

                try {
                    if (request.find("ADD_TEACHER") != std::string::npos) {
                        teacherController.addTeacher(request.substr(request.find("ADD_TEACHER") + 12));
                        response = "Teacher added.\n";
                    }
                    else if (request.find("GET_TEACHERS") != std::string::npos) {
                        response = teacherController.displayTeachers();
                    }
                    else if (request.find("ADD_STUDENT") != std::string::npos) {
                        studentController.addStudent(request.substr(request.find("ADD_STUDENT") + 12));
                        response = "Student added.\n";
                    }
                    else if (request.find("GET_STUDENTS") != std::string::npos) {
                        response = studentController.displayStudents();
                    }
                    else {
                        response = "Unknown command.\n";
                    }
                }
                catch (const std::invalid_argument& e) {
                    response = e.what();
                }

                // buffer back to the sender
                iSendResult = send(ClientSocket, response.c_str(), response.length(), 0);
                if (iSendResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(ClientSocket);
                    WSACleanup();
                    return 1;
                }
                printf("Bytes sent: %d\n", iSendResult);
            }
            else if (iResult == 0)
                printf("Connection closing...\n");
            else {
                printf("recv failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }

        } while (iResult > 0);

        // shutdown the connection since we're done
        iResult = shutdown(ClientSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    }

    // No longer need server socket
    closesocket(ListenSocket);

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}
