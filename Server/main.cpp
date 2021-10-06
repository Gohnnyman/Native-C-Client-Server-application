#include "TcpServer.h"
#include "Logger.h"

#include <iostream>
#include <string>
#include <cstring>

int main() {
    Logger::clear();
    TcpServer server(1041,
        [](Client* client){
            char tmp[128];
            char clientIp[32];
            client->getHostStr(clientIp);
            sprintf(tmp, "Client connected: %s\n", clientIp);

            std::cout << tmp;
            Logger::log(tmp);

            const char answer[] = "How to play:\n1a, a1, 1A or A1 for hiting row 1 and column a.\nquit or q for quit.\n";
            ssize_t result; 


            int size = 0;
            client->sendData(answer, sizeof(answer));
            client->sendMap();

            char* ch;
            while(client->getHits() < 31)
            {
                size = client->getCommand();
                if(!size)
                {
                    sprintf(tmp, "Client leaved: %s\n", clientIp);
                    std::cout << tmp;
                    Logger::log(tmp);
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