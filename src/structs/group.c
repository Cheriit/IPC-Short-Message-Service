//
// Created by cherit on 12/27/19.
//

#include "group.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "../helpers/files.h"
#include "../definitions.h"

Group *_create_group() {
    Group* group = (Group*)malloc(sizeof(Group));
    group->name  = (char*)malloc(MAX_TEXTFIELD_SIZE* sizeof(char));
    group->users = (UserList*)malloc(sizeof(UserList*));
    return group;
}

Group *create_group_from_file(int file) {
    char string[MAX_TEXTFIELD_SIZE];
    Group* group = _create_group();

    get_line(file, string);
    strcpy(group->name, string);

    return group;
}


GroupList *add_to_grp_list(GroupList *list, Group *group) {
    if(list == NULL)
    {
        list = (GroupList*)malloc(sizeof(UserList));
        list->group = group;
        list->next  = NULL;
    }
    else if(list->group == NULL)
    {
        list->group = group;
    }else
        list->next  = add_to_grp_list(list->next, group);
    return list;
}

Group *find_on_grp_list(GroupList *list, char *group_name) {
    if(list == NULL || list->group == NULL) return NULL;
    if(strcmp(list->group->name, group_name) == 0) return list->group;
    return find_on_grp_list(list->next, group_name);
}

int add_user_to_grp(User *user, Group *group) {
    if(find_on_usr_list(group->users, user->username)==NULL)
    {
        group->users = add_to_usr_list(group->users, user);
        return 1;
    }
    return 0;
}

int remove_user_from_grp(User *user, Group *group) {
    if(find_on_usr_list(group->users, user->username)==NULL)
        return 0;
    group->users = remove_user_from_list(group->users, user);
    return 1;
}

GroupList *create_grp_list_from_file(char *filename, UserList* userList) {
    int config_file = open(filename, O_RDONLY);
    if(config_file == -1)
    {
        return 0;
    }
    int is_grp = 0;
    Group* currentGroup = NULL;
    GroupList* groupList = NULL;
    char string[MAX_TEXTFIELD_SIZE];
    while(read_to_char(config_file, '\n', string)){
        if( strcmp(string, CFG_GRP_HEADER) == 0)
        {
            Group* group = create_group_from_file(config_file);
            groupList = add_to_grp_list(groupList, group);
            currentGroup = group;
            is_grp = 1;
        } else if (strcmp(string, CFG_GRP_FOOTER) == 0){
            is_grp = 0;
        } else if(is_grp == 1) {
            User* user = find_on_usr_list(userList, string);
            if(user != NULL)
                add_user_to_grp(user, currentGroup);
        }
    }
    close(config_file);
    return groupList;
}
