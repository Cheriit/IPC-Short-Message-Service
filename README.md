# IPC Messenger for System & Concurrent Programming

## Requirements
- cmake

## Compilation info
```
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
make
```

## Running info
### Server
Inside `cmake-build-debug` use `./server` to run server (do it first). It's crucial to run server first, because client requires it to proper run.

### Client
Inside `cmake-build-debug` use `./client` to run client.

## Files content
### `./src`
- client.c - main file of client's app
- server.c - main file of server's app
- definitions.h - constants definitions
### `./src/helpers`
- files.[ch] - functions required to process files
### `./src/structs`
- group.[ch] - group list operations and structures
- messages.[ch] - message operations and structures
- request.[ch] - request operations and structures
- user.[ch] - user list operations and structures