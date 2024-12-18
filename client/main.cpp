#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <thread>
#include "client.hpp"

using namespace std;

int main()
{
    // создание сокета
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        cerr << "ERROR: cannot create socket" << endl;
        return 1;
    }

    // установка адреса и порта сервера
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);                    // порт 8080
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // адрес сервера (localhost)

    // установка соединения с сервером
    if (connect(client_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        cerr << "ERROR: cannot connect to server" << endl;
        close(client_socket);
        return 1;
    }

    cout << "Connected to server." << endl;

    string name;
    cout << "Please enter your name: ";
    getline(cin, name);
    send(client_socket, name.c_str(), name.length(), 0);
    cout << "Now your name is " << name << ". Enter message (type 'exit' to quit): " << endl;

    thread receiver_thread(receive_messages, client_socket);

    // бесконечный цикл для ввода сообщений
    string message;
    while (true)
    {
        getline(cin, message);

        // если пользователь вводит "exit", завершаем соединение
        if (message == "exit")
        {
            close(client_socket);
            break;
        }

        // отправка сообщения серверу
        send(client_socket, message.c_str(), message.length(), 0);
    }

    receiver_thread.detach(); // ожидание завершения потока

    // закрытие соединения
    close(client_socket);
    return 0;
}
