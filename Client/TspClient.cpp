#include "TspClient.h"
#include <chrono>
#include <iostream>

//Конструктор принимает:
//port - порт на котором будем запускать сервер
//handler - callback-функция запускаямая при подключении клиента
//          объект которого и передают первым аргументом в callback
//          (пример лямбда-функции: [](TcpClient::Client){...do something...})


TcpClient::TcpClient(const uint16_t port) : port(port) 
{
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        wprintf(L"WSAStartup function failed with error: %d\n", iResult);
    }


    SOCKET ConnectSocket;
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) 
    {
        std::cout << "socket function failed: " << WSAGetLastError() << '\n';
    }

    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr("127.0.0.1"); //INADDR_ANY;
    clientService.sin_port = htons(port);


    iResult = connect(ConnectSocket, (SOCKADDR *) & clientService, sizeof (clientService));
    if (iResult == SOCKET_ERROR) {
        wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
        iResult = closesocket(ConnectSocket);
        if (iResult == SOCKET_ERROR)
            wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
    }

    char buf[buffer_size];

    for(;;)
    {
        int result = recv(ConnectSocket, buf, 256, 0);
        if(result == -1) 
        {
            WSACleanup();
            break;
        }
        for(int i = 0; i < result; ++i) std::cout << buf[i];
        std::cout << '\n';
    }


    wprintf(L"Connected to server.\n");





    iResult = closesocket(ConnectSocket);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
    }
}



TcpClient::~TcpClient() 
{
//   if(_status == status::up) stop();
  

#ifdef _WIN32 // Windows NT
    WSACleanup();
#endif
}



#ifdef _WIN32 // Windows NT



#else // *nix

//Запуск сервера (по аналогии с реализацией для Windows)
TcpClient::status TcpClient::start() {
    struct sockaddr_in server;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );
    server.sin_family = AF_INET;
    serv_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(serv_socket == -1) return _status = status::err_socket_init;
    if(bind(serv_socket,(struct sockaddr *)&server , sizeof(server)) < 0) return _status = status::err_socket_bind;
    if(listen(serv_socket, 3) < 0)return _status = status::err_socket_listening;

    _status = status::up;
    handler_thread = std::thread([this]{handlingLoop();});
    return _status;
}

//Остановка сервера
void TcpClient::stop() {
    _status = status::close;
    close(serv_socket);
    joinLoop();
    for(std::thread& cl_thr : client_handler_threads)
        cl_thr.join();
    client_handler_threads.clear ();
    client_handling_end.clear ();
}

// Функиця обработки соединений (по аналогии с реализацией для Windows)
void TcpClient::handlingLoop() {
    while (_status == status::up) {
        int client_socket;
        struct sockaddr_in client_addr;
        int addrlen = sizeof (struct sockaddr_in);
        if((client_socket = accept(serv_socket, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen)) >= 0 && _status == status::up)
            client_handler_threads.push_back(std::thread([this, &client_socket, &client_addr] {
                handler(Client(client_socket, client_addr));
                client_handling_end.push_back (std::this_thread::get_id());
            }));

        if(!client_handling_end.empty())
          for(std::list<std::thread::id>::iterator id_it = client_handling_end.begin (); !client_handling_end.empty() ; id_it = client_handling_end.begin())
            for(std::list<std::thread>::iterator thr_it = client_handler_threads.begin (); thr_it != client_handler_threads.end () ; ++thr_it)
              if(thr_it->get_id () == *id_it) {
                thr_it->join();
                client_handler_threads.erase(thr_it);
                client_handling_end.erase (id_it);
                break;
              }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

// Конструктор клиента по сокету и адресу
TcpClient::Client::Client(int socket, struct sockaddr_in address) : socket(socket), address(address) {}
// Конструктор копирования
TcpClient::Client::Client(const TcpClient::Client& other) : socket(other.socket), address(other.address) {}

TcpClient::Client::~Client() {
    shutdown(socket, 0); //Обрыв соединения сокета
    close(socket); //Закрытие сокета
}

// Геттеры хоста и порта
uint32_t TcpClient::Client::getHost() const {return address.sin_addr.s_addr;}
uint16_t TcpClient::Client::getPort() const {return address.sin_port;}

#endif