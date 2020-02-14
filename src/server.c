#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include "structs/user.h"
#include "structs/group.h"
#include "structs/request.h"
#include "structs/messages.h"
#include "definitions.h"

int run_mainloop = 1;

int login_user(UserList* userList, LoginReq* request, key_t main_key);

int logout_user(User* user);

int list_active_users(UserList* userList, User* user);

int list_users_in_group(User* user, Group* group);

int list_groups(GroupList* groupList, User* user);

int sign_to_group(Group * group, User* user);

int sign_out_group(Group * group, User* user);

int end_sessions(key_t main_key, UserList* userList);

void close_client();

int mainloop(UserList *userList, GroupList *groupList, key_t main_key) {
    ActionRequest* request = (ActionRequest*)malloc(sizeof(ActionRequest));
    LoginReq *login_request = (LoginReq*)malloc(sizeof(LoginReq));
    Message* message = (Message*)malloc(sizeof(Message));
    UserList* currentUser;
    while(run_mainloop)
    {
        currentUser = userList;
        while(currentUser != NULL && run_mainloop)
        {
            if(msgrcv(main_key, login_request, sizeof(LoginReq)- sizeof(long),LOGIN_REQ,IPC_NOWAIT) >=0)
                login_user(userList, login_request, main_key);
            if(currentUser->user->ipc_id >= 0)
            {
                if(msgrcv(currentUser->user->ipc_id, message, sizeof(Message)- sizeof(long),USR_MSG_REQ,IPC_NOWAIT) >=0)
                    process_usr_msg(currentUser->user, userList, message);
                if(msgrcv(currentUser->user->ipc_id, message, sizeof(Message)- sizeof(long),GRP_MSG_REQ,IPC_NOWAIT) >=0)
                    process_grp_msg(currentUser->user, groupList, message);

                if(msgrcv(currentUser->user->ipc_id, request, sizeof(ActionRequest)- sizeof(long), -MAX_REQ_TYPES, IPC_NOWAIT)>=0)
                {
                    printf("Received request type %ld with content %s\n", request->mtype, request->parameters);
                    if      (request->mtype == LOGOUT_REQ     )   logout_user(currentUser->user);
                    else if (request->mtype == ACTIVE_USR_REQ )   list_active_users(userList, currentUser->user);
                    else if (request->mtype == USR_IN_GRP_REQ )   list_users_in_group(currentUser->user, find_on_grp_list(groupList, request->parameters));
                    else if (request->mtype == LIST_GRP_REQ   )   list_groups(groupList, currentUser->user);
                    else if (request->mtype == GRP_SIGNIN_REQ )  sign_to_group(find_on_grp_list(groupList, request->parameters), currentUser->user);
                    else if (request->mtype == GRP_SIGNOUT_REQ)  sign_out_group(find_on_grp_list(groupList, request->parameters), currentUser->user);
                }
            }
            currentUser = currentUser->next;
        }
    }
    end_sessions(main_key, userList);
    exit(0);
        return 1;
}


int main(int argc, char** argv) {
    UserList*  userList  = create_usr_list_from_file(CONFIG_FILE);
    GroupList* groupList = create_grp_list_from_file(CONFIG_FILE);
    if(userList == NULL) return 0;

    key_t main_key = msgget(MAIN_PORT, 0666|IPC_CREAT);
    signal(SIGINT, close_client);
    if(main_key>=0)
    {
        printf("Server setup accomplished. Defined queue number: %d.\n", main_key);
        mainloop(userList, groupList, main_key);
        msgctl(main_key, IPC_RMID, 0);
    }
    else
        printf("Unable to open a queue\n");
    return 0;
}

int login_user(UserList* userList, LoginReq* request, key_t main_key) {
    ActionResponse* login_response = (ActionResponse*)malloc(sizeof(ActionResponse));
    login_response->mtype = request->pid;
    User* user = find_on_usr_list(userList, request->username);
    int success;
    if(user == NULL)
    {
        printf("Failed login request. User \"%s\" doesn't exists.\n", request->username);
        login_response->success = 0;
        strcpy(login_response->content, "Failed login request. User doesn't exists.\n");
    }
    else if(user->failed_login_count>3)
    {
        login_response->success = 0;
        strcpy(login_response->content, "Failed login request. User is locked.");
        printf("Failed login request from %d. User is locked.\n", request->pid);

    }
    else if(strcmp(user->password, request->password) == 0)
    {
        if (user->client_pid != 0)
        {
            logout_user(user);
        }
        user->ipc_id = msgget(REQ_RES_SHIFT+request->pid, 0666|IPC_CREAT);
        user->client_pid = request->pid;

        login_response->success = 1;
        sprintf(login_response->content,"%d", user->ipc_id);
        printf("Login success. User \"%s\", welcome.\n", request->username);
    }
    else
    {
        login_response->success = 0;
        strcpy(login_response->content, "Failed login request. Incorrect password.");
        printf("Failed login request. Incorrect password from %d.\n", request->pid);
        user->failed_login_count++;
    }
    msgsnd(main_key, login_response, sizeof(ActionResponse)- sizeof(long), 0);
    success = login_response->success;
    free(login_response);
    return success;
}

int logout_user(User *user) {
    make_response(user->ipc_id, LOGOUT_REQ+REQ_RES_SHIFT, 1, "User successfully signed out\n");
    printf("User %s has been signed out\n", user->username);
    kill(user->client_pid, SIGINT);
    msgctl(user->ipc_id, IPC_RMID, 0);
    user->client_pid = 0;
    user->ipc_id = -1;
    return 1;
}

int list_active_users(UserList* userList, User* user){
    char content[MAX_CONTENT_SIZE] = "";
    UserList* currentUser = userList;
    while(currentUser != NULL)
    {
        if(currentUser->user->client_pid > 0)
        {
            strcat(content, "\t");
            strcat(content, currentUser->user->username);
            strcat(content, "\n");
        }
        currentUser = currentUser->next;
    }
    make_response(user->ipc_id, ACTIVE_USR_REQ+REQ_RES_SHIFT, 1, content);
    return 1;
}

int list_users_in_group(User* user, Group* group){
    char content[MAX_CONTENT_SIZE] = "\t";
    if(group!=NULL)
    {
        UserList* currentUser = group->users;
        while(currentUser != NULL && currentUser->user != NULL)
        {
            strcat(content, "\t");
            strcat(content, currentUser->user->username);
            strcat(content, "\n");
            currentUser=currentUser->next;
        }
        make_response(user->ipc_id, USR_IN_GRP_REQ+REQ_RES_SHIFT, 1, content);
        return 1;
    }
    else
    {
        make_response(user->ipc_id, USR_IN_GRP_REQ+REQ_RES_SHIFT, -1, "Group doesn't exists.\n");
        return -1;

    }
}

int list_groups(GroupList* groupList, User* user){
    char content[MAX_CONTENT_SIZE] = "";
    GroupList* currentGroup = groupList;
    while(currentGroup != NULL)
    {
        strcat(content, "\t");
        strcat(content, currentGroup->group->name);
        strcat(content, "\n");
        currentGroup = currentGroup->next;
    }
    make_response(user->ipc_id, LIST_GRP_REQ+REQ_RES_SHIFT, 1, content);
    return 1;
}

int sign_to_group(Group * group, User* user)
{
    char content[MAX_CONTENT_SIZE];
    int status;
    if(group != NULL)
    {
        if(add_user_to_grp(user, group)){
            sprintf(content, "Successfully added user %s to group %s.", user->username, group->name);
            status = 1;
        }
        else
        {
            sprintf(content, "User %s already exists in group %s.", user->username, group->name);
            status = 2;
        }
    }
    else
    {
        sprintf(content, "Group doesn't exists");
        status = 0;
    }
    printf("%s\n", content);
    make_response(user->ipc_id, GRP_SIGNIN_REQ+REQ_RES_SHIFT, status, content);
    return status;
}

int sign_out_group(Group * group, User* user)
{
    char content[MAX_CONTENT_SIZE];
    int status;
    if(group != NULL)
    {
        if(remove_user_from_grp(user, group)){
            sprintf(content, "Successfully removed user %s from group %s.", user->username, group->name);
            status = 1;
        }
        else
        {
            sprintf(content, "User %s don't exists in group %s.", user->username, group->name);
            status = 2;
        }
    }
    else
    {
        sprintf(content, "Group %s doesn't exists",  group->name);
        status = 0;
    }
    printf("%s\n", content);
    make_response(user->ipc_id, GRP_SIGNOUT_REQ+REQ_RES_SHIFT, status, content);
    return status;
}

int end_sessions(key_t main_key, UserList *userList) {
    UserList* currentEl = userList;
    while(currentEl != NULL)
    {
        if(currentEl->user->ipc_id >=0)
            logout_user(currentEl->user);
        currentEl = currentEl->next;
    }
    msgctl(main_key, IPC_RMID, 0);
    return 0;
}

void close_client() {
    printf("\nShutting down server\n");
    run_mainloop = 0;
}
