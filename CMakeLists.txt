cmake_minimum_required(VERSION 3.15)
project(Project_IPC C)

set(CMAKE_C_STANDARD 99)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

add_executable(server src/server.c src/structs/user.c src/structs/user.h src/helpers/files.c src/helpers/files.h src/structs/group.c src/structs/group.h src/structs/messages.c src/structs/messages.h src/structs/request.h src/structs/request.c src/definitions.h)

add_executable(client src/client.c src/structs/request.h src/structs/request.c src/structs/user.c src/structs/user.h src/helpers/files.c src/helpers/files.h src/definitions.h src/structs/messages.c src/structs/messages.h src/structs/group.c src/structs/group.h)
