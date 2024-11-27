#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <mutex>
#include <vector>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <algorithm>
#include <fstream>

using namespace std;

// функция для записи сообщения в файл
void save_message_to_file(const string& message);

// функция для отправки истории клиенту
void send_chat_history(int client_socket);

// функция для обработки подключенного клиента
void handle_client(int client_socket);

#endif // SERVER_HPP