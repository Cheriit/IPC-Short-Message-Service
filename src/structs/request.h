//
// Created by cherit on 12/29/19.
//

#ifndef PROJECT_IPC_REQUEST_H
#define PROJECT_IPC_REQUEST_H
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


typedef struct LoginReq{
    long mtype;
    char username[255];
    char password[255];
    int pid;
} LoginReq;

typedef struct LoginRes{
    long mtype;
    int success;
    int ipc_id;
} LoginRes;

typedef struct ActionRequest{
    long mtype;
    char parameters[255];
} ActionRequest;

typedef struct ActionResponse{
    long mtype;
    int success;
    char content[1024];
} ActionResponse;

ActionResponse* make_request(key_t ipc_id, int key, char parameters[255]);
int make_response(key_t ipc_id, int key, int status, char content[1024]);
char* parse_out(char* string);

#endif //PROJECT_IPC_REQUEST_H
