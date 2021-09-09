#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <cstdint>
#include <functional>
#include <thread>
#include <list>

#ifdef _WIN32 // Windows NT

#include <WinSock2.h>

#else // *nix

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#endif

//Буффер для приёма данных от клиента
static constexpr uint16_t buffer_size = 4096;

struct TcpClient {

    class Server;
    typedef std::function<void(Server*)> handler_function_t;

    enum class status : uint8_t {
        up = 0,
        err_socket_init = 1,
        err_socket_bind = 2,
        err_socket_listening = 3,
        close = 4
    };

private:
    uint16_t port; 
    status _status = status::close;
    handler_function_t handler;



#ifdef _WIN32 // Windows NT
    SOCKET serv_socket = INVALID_SOCKET;
    WSAData w_data;
#else // *nix
    int serv_socket;
#endif

    void handlingLoop();

public:
    TcpClient(const uint16_t port);
    ~TcpClient();

    // uint16_t getPort() const;
    // uint16_t setPort(const uint16_t port);

    // status getStatus() const {return _status;}

    // status restart();
    // status start();

    // bool sendData(const char* buffer, const size_t size) const;
    // void stop();
    // void joinLoop();
};


class TcpClient::Server {
#ifdef _WIN32 // Windows NT
    SOCKET socket;
    SOCKADDR_IN address;
    char buffer[buffer_size];
public:
    Server(SOCKET socket, SOCKADDR_IN address);
    const SOCKET& getSocket() const;
    const SOCKADDR_IN& getAddr() const;
#else // *nix
    int socket;
    struct sockaddr_in address;
    char buffer[buffer_size];
public:
    Client(int socket, struct sockaddr_in address);
#endif
public:
    Server(const Server& other);
    ~Server();
    uint32_t getHost() const;
    uint16_t getPort() const;

    int loadData();
    char* getData();

    bool sendData(const char* buffer, const size_t size) const;
};


#endif // TCPCLIENT_H