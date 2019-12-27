#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "structs/user.h"


int main(int argc, char **argv) {
    int main_key = msgget(1008, IPC_EXCL);
    if(main_key)
    {
        LoginReq *login_request = (LoginReq*)malloc(sizeof(LoginReq));
        login_request->mtype = 1;
        strcpy(login_request->username, "test");
        strcpy(login_request->password, "test2");
        msgsnd(main_key, login_request, sizeof(LoginReq)- sizeof(long), 0);
    }
    printf("Hello, Client");
    return 0;
    }
