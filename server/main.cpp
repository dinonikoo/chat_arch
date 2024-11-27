#include "server.hpp"

using namespace std;


int main() {
    // создание сокета
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        cerr << "ERROR: cannot create socket" << endl;
        return 1;
    }

    // установка опции SO_REUSEADDR
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        cerr << "ERROR: setsockopt failed" << endl;
        close(server_socket);
        return 1;
    }

    // настройка адреса и порта сервера
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);  // порт 8080
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // привязка сокета к адресу и порту
    if (bind(server_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        cerr << "ERROR: cannot bind socket to address" << endl;
        close(server_socket);
        return 1;
    }
    // ожидание входящих соединений
    if (listen(server_socket, 5) == -1) {
        cerr << "ERROR: cannot listen on socket" << endl;
        close(server_socket);
        return 1;
    }

    cout << "Server is listening on port 8080..." << endl;

    // основной цикл сервера
    while (true) {
        // принятие нового соединения от клиента
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == -1) {
            cerr << "ERROR: cannot accept incoming connection" << endl;
            continue;  // пропустить ошибку и продолжить слушать соединения
        }

        // создание нового потока для обслуживания клиента
        thread client_thread(handle_client, client_socket);
        client_thread.detach();  // позволяет потоку работать независимо
    }

    // закрытие сокета сервера
    close(server_socket);
    return 0;
}
