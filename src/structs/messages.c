//
// Created by cherit on 12/29/19.
//

#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "messages.h"
#include "request.h"

int send_msg(key_t main_key, Message *msg) {
    msgsnd(main_key, msg, sizeof(Message)- sizeof(long), 0);
    return 1;
}

int receive_msg(key_t key, Message *msg) {
    int status = msgrcv(key, msg, sizeof(Message)- sizeof(long), -1*GRP_MSG_RES, 0);
    printf("\33[2K\r");
    if(msg->mtype == USR_MSG_RES)
    {
        printf("[U] %s: %s",msg->from, msg->content);
    }
    else if (msg->mtype == GRP_MSG_RES)
    {
        printf("(%s) %s: %s", msg->to, msg->from, msg->content);
    }
    return status;
}

int make_msg(User *user) {
    Message* message = (Message*)malloc(sizeof(Message));
    ActionResponse* response = (ActionResponse*)malloc(sizeof(ActionResponse));
    char type[MAX_TEXTFIELD_SIZE];
    char target[MAX_TEXTFIELD_SIZE];
    scanf("%s", type);
    scanf("%s", target);
    fgets(message->content, MAX_CONTENT_SIZE, stdin);
    strcpy(message->from, user->username);
    strcpy(message->to, target);
    if(strcmp(type, "grp") == 0 || strcmp(type, "group") == 0)
    {
        message->mtype = GRP_MSG_REQ;
    }
    else if (strcmp(type, "usr") == 0 || strcmp(type, "user") == 0)
    {
        message->mtype = USR_MSG_REQ;
    }
    else
    {
        fflush(stdin);
        printf("Unknown parameter.\n");
        return -1;
    }
    msgsnd(user->ipc_id, message, sizeof(Message)- sizeof(long), 0);
    msgrcv(user->ipc_id, response, sizeof(ActionResponse)- sizeof(long), message->mtype+REQ_RES_SHIFT, 0);
    if(!response->success)
    {
        printf("%s", response->content);
    }
    else
    {
        printf("\33[2K\r");
    }
    free(message);
    free(response);
    return 1;
}

int process_usr_msg(User *user, UserList *userList, Message *msg) {
    if(strcmp(msg->to, msg->from)== 0)
    {
        printf("User message has been send to itself %s\n", msg->to);
        make_response(user->ipc_id, USR_MSG_REQ+REQ_RES_SHIFT,0, "You cannot send messages to yourself.\n");
        return 0;
    }
    else if(user->failed_login_count > 3)
    {
        printf("User %s is locked.\n", msg->to);
        make_response(user->ipc_id, USR_MSG_REQ+REQ_RES_SHIFT,0, "You've been locked. You can't send user messages\n");
        return 0;
    }

    User* targetUser = find_on_usr_list(userList, msg->to);
    msg->mtype = USR_MSG_RES;
    if(targetUser == NULL)
    {
        printf("User message has been send to nonexisting user %s\n", msg->to);
        make_response(user->ipc_id, USR_MSG_REQ+REQ_RES_SHIFT,0, "User doesn't exists.\n");
        return 0;
    }
    else if(targetUser->ipc_id == -1)
    {
        printf("User %s isn't active right now.\n", msg->to);
        make_response(user->ipc_id, USR_MSG_REQ+REQ_RES_SHIFT,0, "User isn't active.\n");
        return 0;
    }
    else if(targetUser->failed_login_count > 3)
    {
        printf("User %s is locked.\n", msg->to);
        make_response(user->ipc_id, USR_MSG_REQ+REQ_RES_SHIFT,0, "Target user is locked.\n");
        return 0;
    }
    else
    {
//        send_msg(user->ipc_id, msg);
        send_msg(targetUser->ipc_id, msg);
        make_response(user->ipc_id, USR_MSG_REQ+REQ_RES_SHIFT,1, "Success.\n");
        printf("User message has been send from %s to %s\n", msg->from, msg->to);
        return 1;
    }
}

int process_grp_msg(User *user, GroupList *groupList, Message *msg) {
    Group* targetGroup = find_on_grp_list(groupList, msg->to);
    msg->mtype = GRP_MSG_RES;
    if(targetGroup == NULL)
    {
        printf("Group message has been send to not existing group %s\n", msg->to);
        make_response(user->ipc_id, GRP_MSG_REQ+REQ_RES_SHIFT,0, "Group doesn't exists.\n");
        return 0;
    }
    else if(user->failed_login_count > 3)
    {
        printf("User %s is locked.\n", msg->to);
        make_response(user->ipc_id, USR_MSG_REQ+REQ_RES_SHIFT,0, "You've been locked. You can't send group messages.\n");
        return 0;
    }
    else
    {
        if( find_on_usr_list(targetGroup->users, user->username))
        {
            UserList* temporaryUser = targetGroup->users;
            while(temporaryUser != NULL && temporaryUser->user != NULL && temporaryUser->user->ipc_id != -1)
            {
                if(strcmp(temporaryUser->user->username, user->username) != 0 && temporaryUser->user->failed_login_count<=3)
                {
                    send_msg(temporaryUser->user->ipc_id, msg);
                }
                temporaryUser = temporaryUser->next;
            }
            printf("Group message has been send from %s to %s\n", msg->from, msg->to);
            make_response(user->ipc_id, GRP_MSG_REQ+REQ_RES_SHIFT,1, "Success.\n");
            return 1;
        }
        else
        {
            printf("Group message has been send from %s to %s. User don't exists in this group.\n", msg->from, msg->to);
            make_response(user->ipc_id, GRP_MSG_REQ+REQ_RES_SHIFT,0, "User is not signed into this group.\n");
            return 0;
        }
    }
}
