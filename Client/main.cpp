#include "TcpClient.h"
#include "Logger.h"

#include <iostream>
#include <string>
#include <cstring>

int main() {
    Logger::clear();
    TcpClient client("127.0.0.1", 1041);

    //Запуск клиента
    if(client.start() == TcpClient::status::up) {
        std::cout << "Client is up!" << std::endl;
    } else {
        std::cout << "Client start error! Error code:"<< int(client.getStatus()) << std::endl;
        return -1;
    }
}