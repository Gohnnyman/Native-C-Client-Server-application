#include "TcpServer.h"

#include <iostream>
#include <string>
#include <cstring>

//Парсер ip в std::string
std::string getHostStr(const Client& client) {
  uint32_t ip = client.getHost();
  return std::string() + std::to_string(int(reinterpret_cast<char*>(&ip)[0])) + '.' +
         std::to_string(int(reinterpret_cast<char*>(&ip)[1])) + '.' +
         std::to_string(int(reinterpret_cast<char*>(&ip)[2])) + '.' +
         std::to_string(int(reinterpret_cast<char*>(&ip)[3])) + ':' +
         std::to_string( client.getPort ());
}

int main() {
    TcpServer server(1041,
        [](Client* client){
            std::string str;
            std::cout << &str << '\n';
            str = "Connected host: " + getHostStr(*client) + "\n";

            std::cout << str;
            // server->log(str.c_str());

            const char answer[] = "How to play:\n1a, a1, 1A or A1 for hiting row 1 and column a.\nquit or q for quit.\nPress any button to continue\n";
            ssize_t result; 


            int size = 0;
            client->sendData(answer, sizeof (answer));
            size = client->loadData();
            char* ch;
            while(client->getHits() < 31 && size != 0)
            {
                client->sendMap();
                size = client->getCommand();
                if(!size)
                {
                    std::cout << "client " << getHostStr(*client) << " leaved\n";
                    break;
                } 
                else if(size < 0) continue;
            }
            if(size) client->sendStats();
            delete client;
        }
    );

    //Запуск серевера
    if(server.start() == TcpServer::status::up) {
        std::cout << "Server is up!" << std::endl;
    } else {
        std::cout << "Server start error! Error code:"<< int(server.getStatus()) << std::endl;
        return -1;
    }

    std::string input;
    while (std::cin >> input)
    {
        if(input == "quit")
        {
            break;    
        }
    }
}