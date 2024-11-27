#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <thread>

using namespace std;


void receive_messages(int client_socket)
{
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0)
        {
            cerr << "Disconnected from server." << endl;
            close(client_socket);
            exit(0);
        }
        buffer[bytesReceived] = '\0';
        cout << buffer << endl;
    }
}

#endif // CLIENT_HPP