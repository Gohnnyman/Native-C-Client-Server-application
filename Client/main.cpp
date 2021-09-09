#include "TspClient.h"

#include <iostream>
#include <string>

//Парсер ip в std::string
// std::string getHostStr(const TcpClient::Client& client) {
//   uint32_t ip = client.getHost ();
//   return std::string() + std::to_string(int(reinterpret_cast<char*>(&ip)[0])) + '.' +
//          std::to_string(int(reinterpret_cast<char*>(&ip)[1])) + '.' +
//          std::to_string(int(reinterpret_cast<char*>(&ip)[2])) + '.' +
//          std::to_string(int(reinterpret_cast<char*>(&ip)[3])) + ':' +
//          std::to_string( client.getPort ());
// }

int main() {
    //Создание объекта TcpClient с передачей аргументами порта и лябда-фунции для обработк клиента
    TcpClient client(8080);

}