cmake_minimum_required(VERSION 3.0.0)
project(TCPIPClient VERSION 0.1.0)

# include(CTest)
# enable_testing()

set(HEADERS TspClient.h)
set(SOURCES main.cpp TspClient.cpp)
add_executable(TCPIPClient ${SOURCES} ${HEADERS})

if (WIN32)
    target_link_libraries(TCPIPClient ws2_32)
elseif(UNIX)
    find_package(Threads)
    target_link_libraries(TCPIPClient ${CMAKE_THREAD_LIBS_INIT})
endif()

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
    