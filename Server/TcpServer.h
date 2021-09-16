#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "Client.h"
#include <cstdint>
#include <functional>
#include <thread>
#include <list>
#include <unordered_map>
#include <string>

#ifdef _WIN32 // Windows NT

#include <WinSock2.h>

#else // *nix

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define closesocket close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SOCKET int
#define SOCKADDR_IN struct sockaddr_in

#endif

//Буффер для приёма данных от клиента
// static constexpr uint16_t buffer_size = 4096;

struct TcpServer 
{
     //Тип Callback-функции обработчика клиента
    typedef std::function<void(Client*)> handler_function_t;
     //Статус сервера
    enum class status : uint8_t {
        up = 0,
        err_socket_init = 1,
        err_socket_bind = 2,
        err_socket_listening = 3,
        close = 4
    };

private:
    static int result;
    uint16_t port; 
    status _status = status::close;
    handler_function_t handler;
    Client* clnt = nullptr;
    std::thread handler_thread;
    // std::unordered_map<uint32_t, Client*> clients;
    // std::list<std::thread> client_handler_threads;

#ifdef _WIN32 // Windows NT
    SOCKET serv_socket = INVALID_SOCKET;
    WSAData w_data;
#else // *nix
    int serv_socket;
#endif

    void handlingLoop();

public:
    TcpServer(const uint16_t port, handler_function_t handler);
    ~TcpServer();

    void setHandler(handler_function_t handler);

    uint16_t getPort() const;
    uint16_t setPort(const uint16_t port);

    status getStatus() const {return _status;}

    status restart();
    status start();

    bool sendData(const char* buffer, const size_t size) const;
    void stop();
    void joinLoop();
};



#endif // TCPSERVER_H