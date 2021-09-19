cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED False)

project(Logger VERSION 0.2.0)


set(HEADERS Logger.h)
set(SOURCES main.cpp Logger.cpp)
add_executable(Logger ${SOURCES} ${HEADERS})

# if (WIN32)
#     target_link_libraries(TCPIPServer ws2_32)
# elseif(UNIX)
#     find_package(Threads)
#     target_link_libraries(TCPIPServer ${CMAKE_THREAD_LIBS_INIT})
# endif()
