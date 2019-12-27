//
// Created by cherit on 12/27/19.
//

#ifndef PROJECT_IPC_USER_H
#define PROJECT_IPC_USER_H

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

typedef struct LoginReq{
    long mtype;
    char username[255];
    char password[255];
    int pid;
} LoginReq;

typedef struct LoginRes{
    long mtype;
    char success;
    int ipc_id;
} LoginRes;

User* _create_user();
User* create_user_from_file(int file);
int login_user(User* user, char* password, int pid);
int logout_user(User* user);

UserList* create_usr_list_from_file(char* filename);
UserList* add_to_usr_list(UserList* list, User* user);
User* find_on_usr_list(UserList* list, char* username);
UserList* remove_user_from_list(UserList* list, User* user);

#endif //PROJECT_IPC_USER_H
