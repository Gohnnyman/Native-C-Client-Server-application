cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED False)

project(TCPIPServer VERSION 0.2.0)


set(HEADERS TcpServer.h Client.h)
set(SOURCES main.cpp TcpServer.cpp Client.cpp)

add_subdirectory(Logger)

add_executable(TCPIPServer ${SOURCES} ${HEADERS})

target_include_directories(TCPIPServer PRIVATE Logger)
target_link_libraries(TCPIPServer Logger)


# add_subdirectory(../Logger)
if (WIN32)
    target_link_libraries(TCPIPServer ws2_32)
elseif(UNIX)
    find_package(Threads)
    target_link_libraries(TCPIPServer ${CMAKE_THREAD_LIBS_INIT})
endif()

# target_include_directories(TCPIPServer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Logger)
# target_link_directories(TCPIPServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Logger)
