#include "Client.h"
#include <chrono>
#include <iostream>



int Client::loadData() 
{
    ssize_t result = recv(socket, buffer, buffer_size, 0);
    if (result == SOCKET_ERROR)
    {
        std::cout << "recv failed with error\n";
    }

    return result;
}


char* Client::getData() 
{
    return buffer;
}

bool Client::sendData(const char* buffer, const size_t size) const
{
    ssize_t result = send(socket, buffer, size, 0); 
    if (result == SOCKET_ERROR) 
    {
        std::cout << "send failed with errorn\n";
        // WSACleanup();
        return false;
    }
    return true;
}



const SOCKET& Client::getSocket() const 
{
    return socket;
}

const SOCKADDR_IN& Client::getAddr() const
{
    return address;
}



uint32_t Client::getHost() const {return address.sin_addr.s_addr;}
uint16_t Client::getPort() const {return address.sin_port;}
Client::Client(SOCKET socket, SOCKADDR_IN address) : socket(socket), address(address) {}

Client::Client(const Client& other) : socket(other.socket), address(other.address) {}

Client::~Client() 
{
    shutdown(socket, 0);
    closesocket(socket);
}