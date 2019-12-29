//
// Created by cherit on 12/29/19.
//

#ifndef PROJECT_IPC_SERVER_OPERATIONS_H
#define PROJECT_IPC_SERVER_OPERATIONS_H

#include "user.h"
#include "request.h"
#include "group.h"

int login_user(UserList* userList, LoginReq* request, key_t main_key);
int logout_user(User* user);
int list_active_users(UserList* userList, User* user);
int list_users_in_group(User* user, Group* group);
int list_groups(GroupList* groupList, User* user);
int sign_to_group(Group * group, User* user);
int sign_out_group(Group * group, User* user);
#endif //PROJECT_IPC_SERVER_OPERATIONS_H
