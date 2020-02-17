gcc src/client.c src/structs/request.h src/structs/request.c src/structs/user.c src/structs/user.h src/helpers/files.c src/helpers/files.h src/definitions.h src/structs/messages.c src/structs/messages.h src/structs/group.c src/structs/group.h -o client -Wall

gcc src/server.c src/structs/request.h src/structs/request.c src/structs/user.c src/structs/user.h src/helpers/files.c src/helpers/files.h src/definitions.h src/structs/messages.c src/structs/messages.h src/structs/group.c src/structs/group.h -o server -Wall
