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

User* login(key_t queueKey)
{
    int success = 0;
    int pid = getpid();
    LoginReq *login_request = (LoginReq*)malloc(sizeof(LoginReq));
    LoginRes *login_response = (LoginRes*)malloc(sizeof(LoginRes));
    login_request->mtype = 1;
    login_request->pid = pid;
    login_response->mtype = pid;
    while(!success)
    {
        printf("Enter login: ");
        scanf("%254s", login_request->username);
        printf("Enter password: ");
        scanf("%254s", login_request->password);
        msgsnd(queueKey, login_request, sizeof(LoginReq)- sizeof(long), 0);
        msgrcv(queueKey, login_response, sizeof(LoginRes)- sizeof(long), pid, 0);
        if(login_response->success == 1)
        {
            User* user = (User*)malloc(sizeof(User));
            user->username = (char*)malloc(255* sizeof(char));
            strcpy(user->username, login_request->username);
//            user->username = login_request->username;
            user->pid = pid;
            user->ipc_id = login_response->ipc_id;
            free(login_request);
            free(login_response);
            return user;
        }
        else if(login_response->success == 0)
        {
            printf("Incorrect credentials. Try again\n");
        }
    }
    return NULL;
}

int logout(User *user) {
    printf("Signing off");
    ActionResponse* response = make_request(user->ipc_id, 1, "NULL");
    if(response->success) printf("%s\n", response->content);
    free(response);
    return response->success;
}

int list_req(User *user) {
    char command[512];
    char details[255];
    ActionResponse* response;
    scanf("%s", command);
    scanf("%s", details);
    strcat(command, details);
    if(strcmp(command, "availablegroups") == 0)
    {
        response = make_request(user->ipc_id, 5, "NULL");
        strcpy(command, "Existing groups\n");
    }
    else if(strcmp(command, "groupusers") == 0)
    {
        scanf("%s", details);
        response = make_request(user->ipc_id, 4, details);
        sprintf(command, "Users in \"%s\" group\n", details);
    }
    else if(strcmp(command, "loggedusers") == 0)
    {
        response = make_request(user->ipc_id, 3, "NULL");
        strcpy(command, "Signed in users\n");
    }else{
        printf("Command doesn't exist\n");
        return 0;
    }
    if(response->success) printf("%s\t%s\n", command, response->content);
    else printf("An error has occured:\n\t%s", response->content);
    free(response);
    return response->success;
}

int sign_in_grp(User *user) {
    char command[255];
    ActionResponse* response;
    scanf("%s", command);
    response = make_request(user->ipc_id, 10, command);
    if(response->success) printf("Successfully signed in group %s\n", command);
    else printf("An error has occured:\n\t%s", response->content);
    free(response);
    return response->success;
}

int sign_out_grp(User *user) {
    char command[255];
    ActionResponse* response;
    scanf("%s", command);
    response = make_request(user->ipc_id, 11, command);
    if(response->success) printf("Successfully signed out of group %s\n", command);
    else printf("An error has occured:\n\t%s", response->content);
    free(response);
    return response->success;
}

/**
 * @todo Listener on defined actions (logout)
 *
 *
 */
int main(int argc, char **argv) {
    key_t main_key = msgget(1008, IPC_CREAT | 0666);
    int main_loop = 1;
    if(main_key>=0)
    {
        User* user = login(main_key);
        if(!fork())
        {
            char command[255];
            while(main_loop)
            {
                printf("%s>> ", user->username);
                scanf("%s", command);
                if( strcmp(command, "list" ) == 0) list_req(user);
                else if( strcmp(command, "sign_in") == 0) sign_in_grp(user);
                else if( strcmp(command, "sign_out") == 0) sign_out_grp(user);
                else if( strcmp(command, "exit") == 0) main_loop = 0;
                else printf("Command doesn't exists.\n");
            }
            logout(user);
        }
        else
        {
            wait(NULL);
            printf("exiting");
        }
    }
    else
    {
        printf("Unable to open a communication queue with server\n");
    }
    return 0;
    }
