cmake_minimum_required(VERSION 3.0.0)
project(TCPIP VERSION 0.1.0)

# include(CTest)
# enable_testing()

set(HEADERS TcpServer.h)
set(SOURCES main.cpp TcpServer.cpp)
add_executable(TCPIP ${SOURCES} ${HEADERS})

if (WIN32)
    target_link_libraries(TCPIP ws2_32)
elseif(UNIX)
    find_package(Threads)
    target_link_libraries(TCPIP ${CMAKE_THREAD_LIBS_INIT})
endif()

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
    