//
// Created by cherit on 12/27/19.
//

#include "user.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "../helpers/files.h"
#include "../definitions.h"

User *_create_user() {
    User* user       = (User*)malloc(sizeof(User));
    user->username   = (char*)malloc(MAX_TEXTFIELD_SIZE * sizeof(char));
    user->password   = (char*)malloc(MAX_TEXTFIELD_SIZE * sizeof(char));
    user->ipc_id     = -1;
    user->client_pid = 0;
    return user;
}

User *create_user_from_file(int file) {
    char* string    = (char *) malloc(sizeof(char*)*MAX_TEXTFIELD_SIZE);
    User* user      = _create_user();

    get_line(file, &string);
    strcpy(user->username, string);

    get_line(file, &string);
    strcpy(user->password, string);
    user->failed_login_count = 0;
    return user;
}

UserList *add_to_usr_list(UserList *list, User *user) {
    if(list == NULL)
    {
        list        = (UserList*)malloc(sizeof(UserList));
        list->user  = user;
        list->next  = NULL;
    }
    else if(list->user == NULL)
        list->user = user;
    else
        list->next = add_to_usr_list(list->next, user);
    return list;
}

User *find_on_usr_list(UserList *list, char *username) {
    if(list == NULL || list->user == NULL)          return NULL;
    if(strcmp(list->user->username, username) == 0) return list->user;
    return find_on_usr_list(list->next, username);
}

UserList* remove_user_from_list(UserList *list, User *user) {
    if(list == NULL) return NULL;
    if(list->user == user)
    {
        UserList* newRoot = list->next;
        free(list);
        return newRoot;
    }
    else
    {
        list->next = remove_user_from_list(list->next, user);
        return list;
    }
}

UserList *create_usr_list_from_file(char *filename) {
    int config_file = open(filename, O_RDONLY);
    if(config_file == -1)
    {
        printf("Unable to open a file \"%s\"\n", filename);
        return 0;
    }
    UserList* userList = NULL;
    char* string = (char *) malloc(sizeof(char*)*MAX_TEXTFIELD_SIZE);
    while(read_to_char(config_file, '\n', &string)){
        if( strcmp(string, CFG_USR_HEADER) == 0)
        {
            User* user = create_user_from_file(config_file);
            userList = add_to_usr_list(userList, user);
        }
    }
    free(string);
    close(config_file);
    return userList;
}


