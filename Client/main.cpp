#include "TcpClient.h"

#include <iostream>
#include <string>

//Парсер ip в std::string
std::string getHostStr(const TcpClient::Server& client) {
  uint32_t ip = client.getHost ();
  return std::string() + std::to_string(int(reinterpret_cast<char*>(&ip)[0])) + '.' +
         std::to_string(int(reinterpret_cast<char*>(&ip)[1])) + '.' +
         std::to_string(int(reinterpret_cast<char*>(&ip)[2])) + '.' +
         std::to_string(int(reinterpret_cast<char*>(&ip)[3])) + ':' +
         std::to_string( client.getPort ());
}

int main() 
{
    //Создание объекта TcpClient с передачей аргументами порта и лябда-фунции для обработк клиента
    TcpClient client(8080,
        [](TcpClient::Server* server){

            while(true)
            {
                int size = 0;
                char* ch;
                while (!(size = server->loadData ()));

                ch = server->getData();
                std::cout
                    << "size: " << size << " bytes" << std::endl
                    << ch << std::endl;
                
                if(ch == "quit") 
                {
                    // delete client;
                    break;
                }
            }
        }

    );



    // std::string server_msg;

    // while(client.getStatus() == TcpClient::status::up)
    // {
    //     std::cin >> server_msg;
    //     client.sendData(server_msg.c_str(), server_msg.size());
    // }

}