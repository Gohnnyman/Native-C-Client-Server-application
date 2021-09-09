#include "TcpServer.h"

#include <iostream>
#include <string>

//Парсер ip в std::string
std::string getHostStr(const TcpServer::Client& client) {
  uint32_t ip = client.getHost ();
  return std::string() + std::to_string(int(reinterpret_cast<char*>(&ip)[0])) + '.' +
         std::to_string(int(reinterpret_cast<char*>(&ip)[1])) + '.' +
         std::to_string(int(reinterpret_cast<char*>(&ip)[2])) + '.' +
         std::to_string(int(reinterpret_cast<char*>(&ip)[3])) + ':' +
         std::to_string( client.getPort ());
}

int main() {
    //Создание объекта TcpServer с передачей аргументами порта и лябда-фунции для обработк клиента
    TcpServer server(8080,
        [](TcpServer::Client* client){

            //Вывод адреса подключившего клиента в консоль
            std::cout << "Connected host:" << getHostStr(*client) <<std::endl;

            const char answer[] = "Hello World from Server";
            client->sendData(answer, sizeof (answer));

            while(true)
            {
                int size = 0;
                char* ch;
                while (!(size = client->loadData ()));

                ch = client->getData();
                std::cout
                    << "size: " << size << " bytes" << std::endl
                    << ch << std::endl;
                
                if(*ch == 'q') 
                {
                    // delete client;
                    break;
                }
            }
        }

    );

    //Запуск серевера
    if(server.start() == TcpServer::status::up) {
        std::cout << "Server is up!" << std::endl;
    } else {
        std::cout << "Server start error! Error code:"<< int(server.getStatus()) << std::endl;
        return -1;
    }


    std::string server_msg;

    while(server.getStatus() == TcpServer::status::up)
    {
        std::cin >> server_msg;
        server.sendData(server_msg.c_str(), server_msg.size());
    }

}