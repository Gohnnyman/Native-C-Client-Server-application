#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <cstdint>
#include <functional>
#include <thread>
#include <list>
#include <unordered_map>
#include <string>
#include <fstream>


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
static constexpr uint16_t buffer_size = 4096;

struct TcpClient 
{
     //Статус сервера
    enum class status : uint8_t {
        up = 0,
        err_socket_init = 1,
        err_socket_connect = 2,
        err_socket_listening = 3,
        close = 4
    };

private:
    char* buffer = new char[buffer_size];

    static int result;
    uint16_t server_port; 
    char* server_ip;
    status _status = status::close;
    std::thread handler_thread;
    std::thread sender_thread;

    SOCKADDR_IN server_address;
    SOCKET server_socket = INVALID_SOCKET;

#ifdef _WIN32 // Windows NT
    WSAData w_data;
#endif

    void handlingLoop();
    void sendingLoop();

public:
    TcpClient(const char* server_ip, const uint16_t server_port);
    ~TcpClient();

    void getHostStr(char* buffer) const;

    uint32_t getServerHost() const;
    uint16_t getServerPort() const;

    status getStatus() const {return _status;}

    status restart();
    status start();
    
    int loadData();
    char* getData();
    bool sendData(const char* buffer, const size_t size);
    void stop();
    void joinLoop();
};



#endif // TCPSERVER_H