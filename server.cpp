#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>


using namespace std;
vector<int> users;
mutex client_sockets_mutex; 

void handle_client(int client_socket)
{
    {
    lock_guard<mutex> lock(client_sockets_mutex);
    users.push_back(client_socket);
    }
    char buffer[1024];
    string name;

    // Чтение имени пользователя
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received > 0) {
        name = string(buffer, bytes_received);
    }

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        
        // Проверка на завершение соединения
        if (bytes_received <= 0)
        {
            cout << "Client disconnected." << endl;
            close(client_socket);
            lock_guard<mutex> lock(client_sockets_mutex);
            users.erase(remove(users.begin(), users.end(), client_socket), users.end());
            break;
        }

        // Завершаем строку символом '\0'
        buffer[bytes_received] = '\0';
        cout << "Received message from " << name << ": " << buffer << endl;

        // Отправляем ответ клиенту
        string note = "[" + name + "] " + buffer;
        const char *response = note.c_str();

        lock_guard<mutex> lock(client_sockets_mutex);
        for (int i =0; i< users.size(); i++)
        {
            if (users[i] != client_socket)
                send(users[i], response, strlen(response), 0);
        }
    }
    
}

int main()
{
    // Создание сокета
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        cerr << "ERROR: cannot create socket" << endl;
        return 1;
    }

    // Настройка адреса и порта сервера
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // Порт 12345
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Принимаем соединения с любого IP

    // Привязка сокета
    if (bind(server_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        cerr << "ERROR: cannot bind socket to address" << endl;
        close(server_socket);
        return 1;
    }

    // Ожидание входящих соединений
    if (listen(server_socket, 5) == -1)
    {
        cerr << "ERROR: cannot listen on socket" << endl;
        close(server_socket);
        return 1;
    }

    cout << "Server is listening on port 12345..." << endl;

    // Основной цикл сервера
    while (true)
    {
        // Принятие нового соединения от клиента
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == -1)
        {
            cerr << "ERROR: cannot accept incoming connection" << endl;
            continue;  // Пропустить ошибку и продолжить слушать соединения
        }

        // Создание нового потока для обслуживания клиента
        thread client_thread(handle_client, client_socket);
        client_thread.detach();  // Позволяет потоку работать независимо
    }

    // Закрытие сокета сервера
    close(server_socket);
    return 0;
}

