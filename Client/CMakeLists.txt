cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED False)

project(TCPIPClient VERSION 0.2.0)

add_subdirectory(Logger)

set(HEADERS TcpClient.h)
set(SOURCES main.cpp TcpClient.cpp)

add_executable(TCPIPClient ${SOURCES} ${HEADERS})

target_include_directories(TCPIPClient PRIVATE Logger)
target_link_libraries(TCPIPClient Logger)

if (WIN32)
    target_link_libraries(TCPIPClient ws2_32)
elseif(UNIX)
    find_package(Threads)
    target_link_libraries(TCPIPClient ${CMAKE_THREAD_LIBS_INIT})
endif()


    