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
#include <iostream>


static constexpr uint16_t buffer_size = 4096;

class Client {

    uint16_t static const map_size = 11; 
    char map[map_size][map_size] = {
        {' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'},
        {'0', '&', '&', ' ', '&', ' ', ' ', '&', '&', '&', '&'},
        {'1', ' ', ' ', ' ', '&', ' ', ' ', ' ', ' ', ' ', ' '},
        {'2', '&', ' ', ' ', '&', ' ', '&', '&', '&', ' ', '&'},
        {'3', '&', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '&'},
        {'4', '&', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'5', '&', ' ', ' ', ' ', ' ', '&', ' ', ' ', ' ', '&'},
        {'6', ' ', ' ', ' ', ' ', ' ', '&', ' ', ' ', ' ', '&'},
        {'7', '&', '&', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '&'},
        {'8', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'9', ' ', ' ', ' ', ' ', '&', '&', '&', '&', '&', '&'}
    };
    char* buffer = new char[buffer_size];
    int shots = 0;
    int hits = 0;

    void execCommand(const char* ch, const int size);
    void whoCommand();

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

    void getHostStr(char* buffer) const {
        uint32_t ip = getHost();
        sprintf(buffer, "%d.%d.%d.%d:%d", reinterpret_cast<uint8_t*>(&ip)[0],
            reinterpret_cast<uint8_t*>(&ip)[1],
            reinterpret_cast<uint8_t*>(&ip)[2],
            reinterpret_cast<uint8_t*>(&ip)[3],
            htons(getPort()));
    }; 
    
    uint32_t getHost() const;
    uint16_t getPort() const;

    int getCommand();
    int loadData();
    char* getData();
    int getHits() const;

    bool sendData(const char* buffer, const size_t size) const;
    bool sendMap() const;
    bool sendStats() const;
};

#endif // CLIENT_H