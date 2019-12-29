//
// Created by cherit on 12/27/19.
//

#include "group.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "../helpers/files.h"

Group *_create_group() {
    Group* group = (Group*)malloc(sizeof(Group));
    group->name = (char*)malloc(255* sizeof(char));
    group->users = (UserList*)malloc(sizeof(UserList*));
    return group;
}

Group *create_group_from_file(int file) {
    char* string = (char *) malloc(sizeof(char*));
    Group* group = _create_group();

    get_line(file, &string);
    strcpy(group->name, string);

    return group;
}


GroupList *add_to_grp_list(GroupList *list, Group *group) {
    if(list == NULL || list->group == NULL)
    {
        list = (GroupList*)malloc(sizeof(UserList));
        list->group = group;
        list->next = NULL;
    }else {
        list->next = add_to_grp_list(list->next, group);
    }
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
    {
        return 0;
    }
    group->users = remove_user_from_list(group->users, user);
    return 1;
}

GroupList *create_grp_list_from_file(char *filename) {
    int config_file = open(filename, O_RDONLY);
    if(config_file == -1)
    {
        printf("Unable to open a file \"%s\"\n", filename);
        return 0;
    }
    GroupList* groupList = NULL;
    char* string = (char *) malloc(sizeof(char*));
    while(read_to_char(config_file, '\n', &string)){
        if( strcmp(string, "[GROUP]") == 0)
        {
            Group* group = create_group_from_file(config_file);
            groupList = add_to_grp_list(groupList, group);
        }
    }
    free(string);
    close(config_file);
    return groupList;
}
