#include "TcpClient.h"
#include <chrono>
#include <iostream>

//Конструктор принимает:
//port - порт на котором будем запускать сервер
//handler - callback-функция запускаямая при подключении клиента
//          объект которого и передают первым аргументом в callback
//          (пример лямбда-функции: [](TcpClient::Server){...do something...})

int TcpClient::result; 

TcpClient::TcpClient(const uint16_t port, handler_function_t handler) : port(port), handler(handler) 
{
      //----------------------
    // Initialize Winsock
    int iResult;                        

    #ifdef _WIN32
        WSADATA wsaData;
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != NO_ERROR) 
        {
            std::cout << "WSAStartup function failed\n";
        }
    #endif
    //----------------------
    // Create a SOCKET for connecting to server
    SOCKET ConnectSocket;
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cout << "socket function failed with error\n";
        #ifdef _WIN32
            WSACleanup();
        #endif
        // return 1;
    }

    //----------------------


    struct sockaddr_in clientService;

    clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientService.sin_family = AF_INET;
    clientService.sin_port = htons(8080);

    //----------------------
    // Connect to server.
    iResult = connect(ConnectSocket, (struct sockaddr*) & clientService, sizeof (clientService));
    if (iResult == SOCKET_ERROR) 
    {
        std:: cout << "connect function failed with error\n";
        iResult = closesocket(ConnectSocket);
        if (iResult == SOCKET_ERROR)
        std::cout << "closesocket function failed\n";
        closesocket(ConnectSocket);
    }

    std::cout << "Connected to server\n";

    iResult = closesocket(ConnectSocket);
    if (iResult == SOCKET_ERROR) 
    {
        std::cout << "closesocket function failed\n";
        closesocket(ConnectSocket);
    }

}



TcpClient::~TcpClient() 
{
    if(_status == status::up) stop();
  
    #ifdef _WIN32 // Windows NT
        WSACleanup();
    #endif
}

void TcpClient::setHandler(TcpClient::handler_function_t handler) 
{
    this->handler = handler;
}


uint16_t TcpClient::getPort() const 
{
    return port;
}

uint16_t TcpClient::setPort( const uint16_t port) 
{
    this->port = port;
    restart(); 
    return port;
}

TcpClient::status TcpClient::restart() 
{
    if(_status == status::up) stop ();
    return start();
}


void TcpClient::joinLoop() 
{
    handler_thread.join();
}


int TcpClient::Server::loadData() 
{
    result = recv(socket, buffer, buffer_size, 0);
    if (result == SOCKET_ERROR)
    {
        std::cout << "recv failed with error\n";
    }
    return result;
}


char* TcpClient::Server::getData() 
{
    return buffer;
}

bool TcpClient::Server::sendData(const char* buffer, const size_t size) const
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

bool TcpClient::sendData(char const* buffer, const size_t size) const
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

void TcpClient::stop() {
    _status = status::close; 
    closesocket(serv_socket);
    
    joinLoop();
    for(std::thread& cl_thr : client_handler_threads)
        cl_thr.join(); 
    client_handler_threads.clear (); 
}

const SOCKET& TcpClient::Server::getSocket() const 
{
    return socket;
}

const SOCKADDR_IN& TcpClient::Server::getAddr() const
{
    return address;
}

TcpClient::status TcpClient::start() 
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

void TcpClient::handlingLoop() {
    while(_status == status::up) {
        SOCKET client_socket; 
        SOCKADDR_IN client_addr;
        unsigned int addrlen = sizeof(client_addr); 
        client_socket = accept(serv_socket, (struct sockaddr*) &client_addr, &addrlen);

        if (client_socket != 0 && _status == status::up && clnt == nullptr)
        {
            client_handler_threads.push_back(std::thread([this, &client_socket, &client_addr] 
            {
                clnt = new Server(client_socket, client_addr);
                handler(clnt);
            }));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

uint32_t TcpClient::Server::getHost() const {return address.sin_addr.s_addr;}
uint16_t TcpClient::Server::getPort() const {return address.sin_port;}
TcpClient::Server::Server(SOCKET socket, SOCKADDR_IN address) : socket(socket), address(address) {}

TcpClient::Server::Server(const TcpClient::Server& other) : socket(other.socket), address(other.address) {}

TcpClient::Server::~Server() 
{
    shutdown(socket, 0);
    closesocket(socket); 
}