# IPC Messenger for System & Concurrent Programming

Simple messaging system between processes.

## Requirements
- gcc

## Compilation info
```
gcc src/client.c src/structs/request.h src/structs/request.c src/structs/user.c src/structs/user.h src/helpers/files.c src/helpers/files.h src/definitions.h src/structs/messages.c src/structs/messages.h src/structs/group.c src/structs/group.h -o client -Wall

gcc src/server.c src/structs/request.h src/structs/request.c src/structs/user.c src/structs/user.h src/helpers/files.c src/helpers/files.h src/definitions.h src/structs/messages.c src/structs/messages.h src/structs/group.c src/structs/group.h -o server -Wall
```

or run `compile.sh` script.
## Running info
### Server
Use `./server` to run server (do it first). It's crucial to run server first, because client requires it to proper run.

### Client
Use `./client` to run client.

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
