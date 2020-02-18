//
// Created by cherit on 12/27/19.
//

#ifndef PROJECT_IPC_GROUP_H
#define PROJECT_IPC_GROUP_H


#include "user.h"

typedef struct Group{
    char* name;
    UserList* users;
} Group;

typedef struct GroupList{
    Group* group;
    struct GroupList* next;
} GroupList;

Group* _create_group();
Group* create_group_from_file(int file);

GroupList* create_grp_list_from_file(char* filename, UserList* userList);
GroupList* add_to_grp_list(GroupList* list, Group* group);
Group* find_on_grp_list(GroupList* list, char* group_name);

int add_user_to_grp(User* user, Group* group);
int remove_user_from_grp(User* user, Group* group);

#endif //PROJECT_IPC_GROUP_H
