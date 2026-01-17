cmake_minimum_required(VERSION 3.10)
project(OrderMatcher)

set(CMAKE_CXX_STANDARD 17)

# 1. The original Simulation (Phase 1 & 2)
add_executable(exchange main.cpp OrderBook.cpp)

# 2. The new Networked Server (Phase 3)
add_executable(server Server.cpp OrderBook.cpp)

# 3. Link Windows Sockets library (Required for Server.cpp)
if(WIN32)
    target_link_libraries(server ws2_32)
endif()