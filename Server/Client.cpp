#include "Client.h"
#include "Logger.h"
#include <chrono>
#include <iostream>
#include <cstring>


int Client::loadData() 
{
    ssize_t result = recv(socket, buffer, buffer_size, 0);

    buffer[result] = '\0';
    char prefix[64];
    char tmp[25];
    getHostStr(tmp);
    sprintf(prefix, "\nUser %s to server:\n", tmp);
    Logger::log(buffer, prefix);

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
    char prefix[64];
    char tmp[25];
    getHostStr(tmp);
    sprintf(prefix, "\nServer to %s:\n", tmp);
    Logger::log(buffer, prefix);


    ssize_t result = send(socket, buffer, size, 0); 
    if (result == SOCKET_ERROR)
    {
        std::cout << "send failed with error\n";
        // WSACleanup();
        return false;
    }

    return true;
}

bool Client::sendMap() const
{
    size_t size = (map_size * map_size * 2 + map_size + 1);
    strcpy(buffer, "");

    // strcat(buffer, "\x1B[2J\x1B[H");
    for(int i = 0; i < map_size; ++i)
    {
        for(int j = 0; j < map_size; ++j)
        {
            if(map[i][j] == '&')
                strncat(buffer, " ", 1);
            else 
                strncat(buffer, &map[i][j], 1);
            strncat(buffer, "|", 1); 
        }
        strncat(buffer, "\n", 1);
    }
    return sendData(buffer, size);
}

int Client::getHits() const
{
    return hits;
}

bool Client::sendStats() const
{
    strcpy(buffer, "");
    sprintf(buffer, "You win!\nShots: %d\nHits: %d\nMisses: %d\n", shots, hits, shots - hits);
    return sendData(buffer, strlen(buffer));
}

int Client::getCommand()
{
    int size;
    char* ch;
    size = loadData();
    if(size <= 0)
        return size; 

    ch = getData();
    execCommand(ch, size);
    return size;
}

void Client::execCommand(const char* ch, const int size)
{
    char error_string[] = "Invalid command!\n";
    char hit[] = "Hit the target!\n";
    char miss[] = "Miss.\n";
    try
    {
        char tmp_first = ch[0];
        char tmp_second = ch[1];
        int i = -1;
        int j = -1;

        if('0' <= tmp_first && tmp_first <= '9')
        {
            i = tmp_first - '0';
            if('A' <= tmp_second && tmp_second <= 'J')
                j = tmp_second - 'A';
            else if('a' <= tmp_second && tmp_second <= 'j')
                j = tmp_second - 'a';
        } 
        else if ('0' <= tmp_second && tmp_second <= '9') 
        {
            i = tmp_second - '0';
            if('A' <= tmp_first && tmp_first <= 'J')
                j = tmp_first - 'A';
            else if('a' <= tmp_first && tmp_first <= 'j')
                j = tmp_first - 'a';
        }

        if(i == -1 || j == -1)
            throw std::exception();

        if(map[i + 1][j + 1] == '&')
        {
            hits++;
            map[i + 1][j + 1] = '*'; 
            sendData(hit, sizeof(hit));
        }
        else if(map[i + 1][j + 1] == ' ')
        {
            map[i + 1][j + 1] = 'X';
            sendData(miss, sizeof(miss));
        } 
        else 
        {
            sendData(miss, sizeof(miss));
        }
        shots++;
    }
    catch(const std::exception& e)
    {
        sendData(error_string, sizeof(error_string));
    }
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
    delete[] buffer;
    shutdown(socket, 0);
    closesocket(socket);
}