//
// Created by cherit on 12/29/19.
//

#ifndef PROJECT_IPC_REQUEST_H
#define PROJECT_IPC_REQUEST_H
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "../definitions.h"


typedef struct LoginReq{
    long mtype;
    char username[MAX_TEXTFIELD_SIZE];
    char password[MAX_TEXTFIELD_SIZE];
    int pid;
} LoginReq;

typedef struct ActionRequest{
    long mtype;
    char parameters[MAX_TEXTFIELD_SIZE];
} ActionRequest;

typedef struct ActionResponse{
    long mtype;
    int success;
    char content[MAX_CONTENT_SIZE];
} ActionResponse;

ActionResponse *make_request(key_t ipc_id, int key, char parameters[255], int flag);
int make_response(key_t ipc_id, int key, int status, char content[1024]);
char* parse_out(char* string);

#endif //PROJECT_IPC_REQUEST_H
