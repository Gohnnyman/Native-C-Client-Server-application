#include "TcpServer.h"
#include <chrono>
#include <iostream>
#include <cstring>
#include <time.h>
#include <mutex>

std::mutex mtx;
int TcpServer::result; 

TcpServer::TcpServer(const uint16_t port, handler_function_t handler) : port(port), handler(handler) 
{
    logfile.open(logfilename, std::fstream::app);    
    // logfile << "asdaqwe  sd\n";
    // logfile.close();
}

TcpServer::~TcpServer() 
{
    logfile.close();
    stop();
    #ifdef _WIN32 // Windows NT
        WSACleanup();
    #endif
}

void TcpServer::log(const char* buffer)
{
    mtx.lock();
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strcpy(this->buffer, "[");
    sprintf(this->buffer, "[%s]: %s", getlocaltime(timeinfo), buffer);
    logfile << this->buffer;
    logfile.flush();
    mtx.unlock();
}

char* TcpServer::getlocaltime(const struct tm *timeptr) const
{
  static const char wday_name[][4] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };
  static const char mon_name[][4] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  static char result[26];
  sprintf(result, "%.3s %.3s%3d %.2d:%.2d:%.2d %d",
    wday_name[timeptr->tm_wday],
    mon_name[timeptr->tm_mon],
    timeptr->tm_mday, timeptr->tm_hour,
    timeptr->tm_min, timeptr->tm_sec,
    1900 + timeptr->tm_year);
  return result;
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
            std::thread t([this, client]{handler(client);});
            t.detach();
        } 
        else 
        {   
            closesocket(client_socket);
        }
    }
}
