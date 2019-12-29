//
// Created by cherit on 12/27/19.
//

#ifndef PROJECT_IPC_USER_H
#define PROJECT_IPC_USER_H

#include <sys/ipc.h>

typedef struct User{
    char* username;
    char* password;
    int pid;
    int ipc_id;
} User;

typedef struct UserList{
    User* user;
    struct UserList* next;
} UserList;


User* _create_user();
User* create_user_from_file(int file);

UserList* create_usr_list_from_file(char* filename);
UserList* add_to_usr_list(UserList* list, User* user);
User* find_on_usr_list(UserList* list, char* username);
UserList* remove_user_from_list(UserList* list, User* user);

#endif //PROJECT_IPC_USER_H
