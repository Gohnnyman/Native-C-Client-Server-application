#include "TcpClient.h"
#include "Logger.h"
#include <chrono>
#include <iostream>
#include <cstring>

int TcpClient::result; 

TcpClient::TcpClient(const char* server_ip, const uint16_t server_port) : server_port(server_port) 
{
    this->server_ip = new char[32];
    strcpy(this->server_ip, server_ip);
}

TcpClient::~TcpClient() 
{
    delete[] this->server_ip;
    stop();
}

TcpClient::status TcpClient::restart() 
{
    if(_status == status::up) 
    {
        stop();

    }
    return start();
}

void TcpClient::joinLoop() 
{
    handler_thread.~thread();
    // handler_thread.join();
}

void TcpClient::stop() 
{
    _status = status::close; 

    joinLoop();
    
    closesocket(server_socket);
    #ifdef _WIN32 
        WSACleanup();
    #endif

    std::cout << "Client stopped\n";
}


TcpClient::status TcpClient::start() 
{
    #ifdef _WIN32
        WSAStartup(MAKEWORD(2, 2), &w_data); //Задаём версию WinSocket
    #endif

    server_address.sin_addr.s_addr = inet_addr(this->server_ip); //IP адресс сервера
    server_address.sin_port = htons(server_port); // Порт сервера
    server_address.sin_family = AF_INET; //AF_INET - Cемейство адресов для IPv4
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(static_cast<int>(server_socket) == SOCKET_ERROR) return _status = status::err_socket_init;
    if(connect(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) return _status = status::err_socket_connect;


    _status = status::up;
    handler_thread = std::thread([this]{handlingLoop();});
    sendingLoop();
    return _status;
}

void TcpClient::getHostStr(char* buffer) const 
{
    uint32_t ip = getServerHost();
    sprintf(buffer, "%d.%d.%d.%d:%d", reinterpret_cast<uint8_t*>(&ip)[0],
            reinterpret_cast<uint8_t*>(&ip)[1],
            reinterpret_cast<uint8_t*>(&ip)[2],
            reinterpret_cast<uint8_t*>(&ip)[3],
        htons(getServerPort()));
}

bool TcpClient::sendData(const char* buffer, const size_t size)
{
    char prefix[64];
    char tmp[25];
    getHostStr(tmp);
    sprintf(prefix, "\nUser to server %s:\n", tmp);
    Logger::log(buffer, prefix);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    ssize_t result = send(server_socket, buffer, size, 0); 
    if (result == SOCKET_ERROR)
    {
        std::cout << "send failed with error\n";
        stop();
        return false;
    }

    return true;
}

void TcpClient::sendingLoop() 
{
    while(std::cin >> buffer && _status == status::up)
    {
        if(strcmp(buffer, "quit") == 0 || strcmp(buffer, "q") == 0) 
            stop();
        else 
            sendData(buffer, buffer_size);
    }
}

int TcpClient::loadData() 
{
    ssize_t result = recv(server_socket, buffer, buffer_size, 0);

    buffer[result] = '\0';
    char prefix[64];
    char tmp[25];
    getHostStr(tmp);
    sprintf(prefix, "\nServer %s to user:\n", tmp);
    Logger::log(buffer, prefix);

    if (result == SOCKET_ERROR)
    {
        std::cout << "recv failed with error\n";
        stop();
    }

    return result;
}

char* TcpClient::getData() 
{
    return buffer;
}

void TcpClient::handlingLoop() {
    char tmp[128];
    char clientIp[32];
    char* ch;
    getHostStr(clientIp);
    sprintf(tmp, "Connected to: %s\n", clientIp);
    std::cout << tmp;

    while (_status == status::up)
    {
        result = loadData();
        ch = getData();
        if (result < 0)
        {
            std::cout << "recv failed with error\n";
            stop();
        } 
        else if(result == 0)
        {
            std::cout << "Server was closed\n";
            stop();
        }
        std::cout << ch << '\n';
    }

}

uint32_t TcpClient::getServerHost() const {return server_address.sin_addr.s_addr;}
uint16_t TcpClient::getServerPort() const {return server_address.sin_port;}