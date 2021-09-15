#ifndef CLIENT_H
#define CLIENT_H
#include <cstdint>
#include <functional>
#include <thread>
#include <list>
#include <unordered_map>

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


static constexpr uint16_t buffer_size = 4096;

class Client {
#ifdef _WIN32 // Windows NT
    SOCKET socket;
    SOCKADDR_IN address;
public:
    Client(SOCKET socket, SOCKADDR_IN address);
#else // *nix
    int socket;
    struct sockaddr_in address;
    
public:
    Client(int socket, struct sockaddr_in address);
#endif
public:
    Client(const Client& other);
    ~Client();
    const SOCKET& getSocket() const;
    const SOCKADDR_IN& getAddr() const;

    uint32_t getHost() const;
    uint16_t getPort() const;
    char buffer[buffer_size];

    int loadData();
    char* getData();

    bool sendData(const char* buffer, const size_t size) const;
};

#endif // CLIENT_H