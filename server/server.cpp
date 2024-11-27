#include "server.hpp"

using namespace std;

vector<int> users;
mutex client_sockets_mutex;
mutex file_mutex;
const string history_filename = "chat_history.txt";

// функция для записи сообщения в файл
void save_message_to_file(const string& message) {
    lock_guard<mutex> lock(file_mutex);
    ofstream history_file(history_filename, ios::app);  // открываем файл в ржиме добавления
    if (history_file.is_open()) {
        history_file << message << endl;  // записываем в файл
        history_file.close();  // закрываем файл
    }
}

// функция для отправки истории клиенту
void send_chat_history(int client_socket) {
    lock_guard<mutex> lock(file_mutex);
    ifstream history_file(history_filename);  // открыть файл для чтения
    string line;
    while (getline(history_file, line)) {  // читаем построчно
        send(client_socket, line.c_str(), line.length(), 0);  // отправляем каждую строчку клиенту
        usleep(1000);  // задержка чтобы буфер не переполнялся
    }
    history_file.close();  // закрываем файл
}

// функция для обработки подключенного клиента
void handle_client(int client_socket) {
    {
        lock_guard<mutex> lock(client_sockets_mutex);
        users.push_back(client_socket);
    }

    char buffer[1024];
    string name;

    // чтение имени пользователя
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received > 0) {
        name = string(buffer, bytes_received);
    }

    // отправляем историю новому клиенту
    send_chat_history(client_socket);

    while (true) {
        memset(buffer, 0, sizeof(buffer));  // очищаем буфер
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        // проверка на завершение соединения
        if (bytes_received <= 0) {
            cout << "Client disconnected." << endl;
            close(client_socket);
            lock_guard<mutex> lock(client_sockets_mutex);
            users.erase(remove(users.begin(), users.end(), client_socket), users.end());
            break;
        }

        buffer[bytes_received] = '\0';  // завершаем строку символом '\0'
        string note = "[" + name + "] " + buffer;
        cout << note << endl;

        save_message_to_file(note);  // сохраняем сообщение в файл

        lock_guard<mutex> lock(client_sockets_mutex);
        for (int user_socket : users) {
            if (user_socket != client_socket)
                send(user_socket, note.c_str(), note.length(), 0);
        }
    }
}