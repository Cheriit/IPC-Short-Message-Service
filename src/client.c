#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <wait.h>
#include "structs/user.h"
#include "structs/request.h"
#include "definitions.h"

int main_loop = 1;

User* login(key_t queueKey)
{
    int success = 0;
    int pid = getpid();
    LoginReq       *request  = (LoginReq*)malloc(sizeof(LoginReq));
    ActionResponse *response = (ActionResponse*)malloc(sizeof(ActionResponse));
    request->mtype  = LOGIN_REQ;
    request->pid    = pid;
    response->mtype = pid;
    while(!success)
    {
        printf("Enter login: ");
        scanf("%s", request->username);
        printf("Enter password: ");
        scanf("%s", request->password);
        msgsnd(queueKey, request, sizeof(LoginReq) - sizeof(long), 0);
        msgrcv(queueKey, response, sizeof(ActionResponse) - sizeof(long), pid, 0);
        if(response->success == 1)
        {
            User* user = (User*)malloc(sizeof(User));
            user->username = (char*)malloc(MAX_TEXTFIELD_SIZE* sizeof(char));
            strcpy(user->username, request->username);
            user->client_pid = pid;
            user->ipc_id = atoi(response->content);
            free(request);
            free(response);
            return user;
        }
        else if(response->success == 0)
        {
            printf("Incorrect credentials. Try again\n");
        }
    }
    return NULL;
}

int logout(User *user) {
    ActionResponse* response = make_request(user->ipc_id, LOGOUT_REQ, "NULL");
    printf("%s\n", response->content);
    free(response);
    return response->success;
}

int list_req(User *user) {
    char command[MAX_CONTENT_SIZE];
    char details[MAX_TEXTFIELD_SIZE];
    ActionResponse* response;
    scanf("%s", command);
    scanf("%s", details);
    strcat(command, " ");
    strcat(command, details);
    if(strcmp(command, "available groups") == 0)
    {
        response = make_request(user->ipc_id, LIST_GRP_REQ, "NULL");
        strcpy(command, "Existing groups\n");
    }
    else if(strcmp(command, "group users") == 0)
    {
        scanf("%s", details);
        response = make_request(user->ipc_id, USR_IN_GRP_REQ, details);
        sprintf(command, "Users in \"%s\" group\n", details);
    }
    else if(strcmp(command, "logged users") == 0)
    {
        response = make_request(user->ipc_id, ACTIVE_USR_REQ, "NULL");
        strcpy(command, "Signed in users\n");
    }else{
        printf("Command doesn't exist\n");
        return 0;
    }
    printf("%s\n%s\n", command, response->content);
    free(response);
    return response->success;
}

int sign_in_grp(User *user) {
    char command[MAX_TEXTFIELD_SIZE];
    ActionResponse* response;
    scanf("%s", command);
    response = make_request(user->ipc_id, GRP_SIGNIN_REQ, command);
    printf("%s\n", response->content);
    free(response);
    return response->success;
}

int sign_out_grp(User *user) {
    char command[MAX_TEXTFIELD_SIZE];
    ActionResponse* response;
    scanf("%s", command);
    response = make_request(user->ipc_id, GRP_SIGNOUT_REQ, command);
    printf("%s\n", response->content);
    free(response);
    return response->success;
}

/**
 * @todo Listener on defined actions (logout, message)
 */
int main(int argc, char **argv) {
    key_t main_key = msgget(MAIN_PORT, IPC_CREAT | 0666);
    if(main_key>=0)
    {
        User* user = login(main_key);
        if(!fork())
        {
            char command[MAX_TEXTFIELD_SIZE];
            while(main_loop)
            {
                printf("%s>> ", user->username);
                scanf("%s", command);
                if     ( strcmp(command, "list")     == 0) list_req(user);
                else if( strcmp(command, "sign_in")  == 0) sign_in_grp(user);
                else if( strcmp(command, "sign_out") == 0) sign_out_grp(user);
                else if( strcmp(command, "exit")     == 0) main_loop = 0;
                else printf("Command doesn't exists.\n");
            }
            logout(user);
            exit(0);
        }
        else
        {
            wait(NULL);
            printf("exiting");
        }
    }
    else
        printf("Unable to open a communication queue with server\n");
    return 0;
}
