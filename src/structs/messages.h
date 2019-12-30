//
// Created by cherit on 12/29/19.
//

#ifndef PROJECT_IPC_MESSAGES_H
#define PROJECT_IPC_MESSAGES_H
#include <sys/ipc.h>
#include "../definitions.h"
#include "user.h"
#include "group.h"

typedef struct Message{
    long mtype;
    char from[MAX_TEXTFIELD_SIZE];
    char to[MAX_TEXTFIELD_SIZE];
    char content[MAX_CONTENT_SIZE];
} Message;

int receive_msg(key_t key, Message* msg);
int make_msg(User* user);
int send_msg(key_t main_key, Message* msg);
int process_usr_msg(User* user, UserList* userList, Message* msg);
int process_grp_msg(User* user, GroupList* groupList, Message* msg);
#endif //PROJECT_IPC_MESSAGES_H
