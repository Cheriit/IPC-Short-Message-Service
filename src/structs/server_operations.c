//
// Created by cherit on 12/29/19.
//
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/msg.h>
#include <signal.h>
#include "server_operations.h"
#include "group.h"

int login_user(UserList* userList, LoginReq* request, key_t main_key) {
    LoginRes* login_response = (LoginRes*)malloc(sizeof(LoginRes));
    login_response->mtype = request->pid;
    User* user = find_on_usr_list(userList, request->username);
    int success;
    if(user == NULL)
    {
        printf("Failed login request. User \"%s\" doesn't exists.\n", request->username);
        login_response->success = 0;
        login_response->ipc_id = 0;
    }
    else if(strcmp(user->password, request->password) == 0)
    {
        if (user->client_pid != 0)
        {
            logout_user(user);
        }
        user->ipc_id = msgget(1000+request->pid, 0666|IPC_CREAT);
        user->client_pid = request->pid;

        login_response->success = 1;
        login_response->ipc_id = user->ipc_id;
        printf("Login success. User \"%s\", welcome.\n", request->username);
    }
    else
    {
        login_response->success = 0;
        login_response->ipc_id = 0;
        printf("Failed login request. Incorrect password from %d.\n", request->pid);
    }
    msgsnd(main_key, login_response, sizeof(LoginRes)- sizeof(long), 0);
    success = login_response->success;
    free(login_response);
    return success;
}

int logout_user(User *user) {
    make_response(user->ipc_id, 101, 1, "User successfully signed out\n");
    printf("User %s has been signed out\n", user->username);
    user->client_pid = 0;
    user->ipc_id = 0;
    msgctl(user->ipc_id, IPC_RMID, 0);
    kill(user->server_pid, 3);
    return 1;
}

int list_active_users(UserList* userList, User* user){
    char content[1024] = "";
    UserList* currentUser = userList;
    while(currentUser != NULL)
    {
        if(currentUser->user->client_pid > 0)
            sprintf(content, "%s\t%s\n",content, currentUser->user->username);
        currentUser = currentUser->next;
    }
    make_response(user->ipc_id, 103, 1, content);
    return 1;
}

int list_users_in_group(User* user, Group* group){
    char content[1024] = "";
    if(group!=NULL)
    {
        UserList* currentUser = group->users;
        while(currentUser != NULL && currentUser->user != NULL)
        {
            sprintf(content, "%s\t%s\n",content, currentUser->user->username);
            currentUser=currentUser->next;
        }
        make_response(user->ipc_id, 104, 1, content);
        return 1;
    }
    else
    {
        make_response(user->ipc_id, 104, -1, "Group doesn't exists.\n");
        return -1;

    }
}

int list_groups(GroupList* groupList, User* user){
    char content[1024] = "";
    GroupList* currentGroup = groupList;
    while(currentGroup != NULL)
    {
        sprintf(content, "%s\t%s\n",content, currentGroup->group->name);
        currentGroup = currentGroup->next;
    }
    make_response(user->ipc_id, 105, 1, content);
    return 1;
}

int sign_to_group(Group * group, User* user)
{
    char content[1024];
    int status;
    if(group != NULL)
    {
        if(add_user_to_grp(user, group)){
            sprintf(content, "Succesfully added user %s to group %s.", user->username, group->name);
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
    make_response(user->ipc_id, 110, status, content);
    return status;
}
int sign_out_group(Group * group, User* user)
{
    char content[1024];
    int status;
    if(group != NULL)
    {
        if(remove_user_from_grp(user, group)){
            sprintf(content, "Succesfully removed user %s from group %s.", user->username, group->name);
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
    make_response(user->ipc_id, 111, status, content);
    return status;
}