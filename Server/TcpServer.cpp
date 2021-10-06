#include "TcpServer.h"
#include <chrono>
#include <iostream>
#include <cstring>

int TcpServer::result; 

TcpServer::TcpServer(const uint16_t port, handler_function_t handler) : port(port), handler(handler) 
{
    // logfile << "asdaqwe  sd\n";
    // logfile.close();
}

TcpServer::~TcpServer() 
{
    stop();
    #ifdef _WIN32 // Windows NT
        WSACleanup();
    #endif
}




void TcpServer::setHandler(TcpServer::handler_function_t handler) 
{
    this->handler = handler;
}


uint16_t TcpServer::getPort() const 
{
    return port;
}

uint16_t TcpServer::setPort(const uint16_t port) 
{
    this->port = port;
    restart(); 
    return port;
}

TcpServer::status TcpServer::restart() 
{
    if(_status == status::up) 
    {
        stop();
        #ifdef _WIN32 // Windows NT
            WSACleanup();
        #endif
    }
    return start();
}


void TcpServer::joinLoop() 
{
    handler_thread.join();
}

void TcpServer::stop() 
{
    _status = status::close; 
    joinLoop();
    closesocket(serv_socket);
}


TcpServer::status TcpServer::start() 
{
    #ifdef _WIN32
        WSAStartup(MAKEWORD(2, 2), &w_data); //Задаём версию WinSocket
    #endif

    SOCKADDR_IN address; //Структура хост/порт/протокол для инициализации сокета
    address.sin_addr.s_addr = INADDR_ANY; //Любой IP адресс
    address.sin_port = htons(port); //Задаём порт
    address.sin_family = AF_INET; //AF_INET - Cемейство адресов для IPv4
    serv_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(static_cast<int>(serv_socket) == SOCKET_ERROR) return _status = status::err_socket_init;

    if(bind(serv_socket, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) return _status = status::err_socket_bind;

    if(listen(serv_socket, SOMAXCONN) == SOCKET_ERROR) return _status = status::err_socket_listening;


    _status = status::up;
    handler_thread = std::thread([this]{handlingLoop();});
    return _status;
}

void TcpServer::handlingLoop() {
    Client* client;
    while(_status == status::up) 
    {
        SOCKET client_socket; 
        SOCKADDR_IN client_addr;
        unsigned int addrlen = sizeof(client_addr); 
        client_socket = accept(serv_socket, (struct sockaddr*) &client_addr, &addrlen);
        if (client_socket != 0 && _status == status::up)
        {
            client = new Client(client_socket, client_addr);
            uint32_t ip = client->getHost();
                        std::cout << "POPA:\n" << ip << '\n';
            std::cout << "POPAAAAA\n" << uint32_t(reinterpret_cast<char*>(&ip)[0]) << '\n';
            std::thread t([this, client]{handler(client);});
            t.detach();
        } 
        else 
        {   
            closesocket(client_socket);
        }
    }
}
