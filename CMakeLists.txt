cmake_minimum_required(VERSION 3.5)
project(p2 C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(Inc)

add_executable(onecard Src/main.c Src/data_struct.c Inc/data_struct.h Src/player.c Inc/player.h Src/server.c Inc/server.h Src/cli.c Inc/cli.h Inc/main.h)
