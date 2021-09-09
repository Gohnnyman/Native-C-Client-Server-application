#include "TcpServer.h"
#include <chrono>
#include <iostream>

//Конструктор принимает:
//port - порт на котором будем запускать сервер
//handler - callback-функция запускаямая при подключении клиента
//          объект которого и передают первым аргументом в callback
//          (пример лямбда-функции: [](TcpServer::Client){...do something...})

int TcpServer::result; 

TcpServer::TcpServer(const uint16_t port, handler_function_t handler) : port(port), handler(handler) {}



TcpServer::~TcpServer() 
{
  if(_status == status::up) stop();
  

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

uint16_t TcpServer::setPort( const uint16_t port) 
{
    this->port = port;
    restart(); 
    return port;
}

TcpServer::status TcpServer::restart() 
{
    if(_status == status::up) stop ();
    return start();
}


void TcpServer::joinLoop() 
{
    handler_thread.join();
}


int TcpServer::Client::loadData() 
{
    result = recv(socket, buffer, buffer_size, 0);
    if (result == SOCKET_ERROR)
    {
        std::cout << "recv failed with error\n";
    }
    return result;
}


char* TcpServer::Client::getData() 
{
    return buffer;
}

bool TcpServer::Client::sendData(const char* buffer, const size_t size) const
{
    result = send(socket, buffer, size, 0); 
    if (result == SOCKET_ERROR) 
    {
        std::cout << "send failed with errorn\n";
        // WSACleanup();
        return false;
    }
    return true;
}

bool TcpServer::sendData(char const* buffer, const size_t size) const
{
    // return clnt->sendData(buffer, size);
    // SOCKET s = clnt->getSocket();
    result = send(clnt->getSocket(), buffer, size, 0); 
    if (result == SOCKET_ERROR) 
    {
        std::cout << "send failed with errorn\n";
        return false;
    }
    return true;
}

void TcpServer::stop() {
    _status = status::close; 
    closesocket(serv_socket);
    
    joinLoop();
    for(std::thread& cl_thr : client_handler_threads)
        cl_thr.join(); 
    client_handler_threads.clear (); 
}

const SOCKET& TcpServer::Client::getSocket() const 
{
    return socket;
}

const SOCKADDR_IN& TcpServer::Client::getAddr() const
{
    return address;
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
    while(_status == status::up) {
        SOCKET client_socket; 
        SOCKADDR_IN client_addr;
        unsigned int addrlen = sizeof(client_addr); 
        client_socket = accept(serv_socket, (struct sockaddr*) &client_addr, &addrlen);

        if (client_socket != 0 && _status == status::up && clnt == nullptr)
        {
            client_handler_threads.push_back(std::thread([this, &client_socket, &client_addr] 
            {
                clnt = new Client(client_socket, client_addr);
                handler(clnt);
            }));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

uint32_t TcpServer::Client::getHost() const {return address.sin_addr.s_addr;}
uint16_t TcpServer::Client::getPort() const {return address.sin_port;}
TcpServer::Client::Client(SOCKET socket, SOCKADDR_IN address) : socket(socket), address(address) {}

TcpServer::Client::Client(const TcpServer::Client& other) : socket(other.socket), address(other.address) {}

TcpServer::Client::~Client() 
{
    shutdown(socket, 0);
    closesocket(socket); 
}