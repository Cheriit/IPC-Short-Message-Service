#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include "structs/user.h"
#include "structs/group.h"
#include "structs/request.h"
#include "structs/server_operations.h"
#include "definitions.h"
#include "structs/messages.h"

int run_mainloop = 1;

void close_client()
{
    printf("\nShutting down server\n");
    run_mainloop = 0;
}

int mainloop(UserList *userList, GroupList *groupList, key_t main_key) {
    ActionRequest* request = (ActionRequest*)malloc(sizeof(ActionRequest));
    LoginReq *login_request = (LoginReq*)malloc(sizeof(LoginReq));
    Message* message = (Message*)malloc(sizeof(Message));
    UserList* currentUser;
    while(run_mainloop)
    {
        currentUser = userList;
        while(currentUser != NULL && run_mainloop)
        {
            if(msgrcv(main_key, login_request, sizeof(LoginReq)- sizeof(long),LOGIN_REQ,IPC_NOWAIT) >=0)
                login_user(userList, login_request, main_key);
            if(currentUser->user->ipc_id >= 0)
            {
                if(msgrcv(currentUser->user->ipc_id, message, sizeof(Message)- sizeof(long),USR_MSG_REQ,IPC_NOWAIT) >=0)
                    process_usr_msg(currentUser->user, userList, message);
                if(msgrcv(currentUser->user->ipc_id, message, sizeof(Message)- sizeof(long),GRP_MSG_REQ,IPC_NOWAIT) >=0)
                    process_grp_msg(currentUser->user, groupList, message);

                if(msgrcv(currentUser->user->ipc_id, request, sizeof(ActionRequest)- sizeof(long), -MAX_REQ_TYPES, IPC_NOWAIT)>=0)
                {
                    printf("Received request type %ld with content %s\n", request->mtype, request->parameters);
                    if      (request->mtype == LOGOUT_REQ     )   logout_user(currentUser->user);
                    else if (request->mtype == ACTIVE_USR_REQ )   list_active_users(userList, currentUser->user);
                    else if (request->mtype == USR_IN_GRP_REQ )   list_users_in_group(currentUser->user, find_on_grp_list(groupList, request->parameters));
                    else if (request->mtype == LIST_GRP_REQ   )   list_groups(groupList, currentUser->user);
                    else if (request->mtype == GRP_SIGNIN_REQ )  sign_to_group(find_on_grp_list(groupList, request->parameters), currentUser->user);
                    else if (request->mtype == GRP_SIGNOUT_REQ)  sign_out_group(find_on_grp_list(groupList, request->parameters), currentUser->user);
                }
            }
            currentUser = currentUser->next;
        }
    }
    end_sessions(main_key, userList);
    exit(0);
    return 1;
}

/**
 * @todo signal overload (exiting, kill)
 * @todo Wall check and fix (sprintf additional var)
 */
int main(int argc, char** argv) {
    UserList*  userList  = create_usr_list_from_file(CONFIG_FILE);
    GroupList* groupList = create_grp_list_from_file(CONFIG_FILE);
    if(userList == NULL) return 0;

    key_t main_key = msgget(MAIN_PORT, 0666|IPC_CREAT);
    signal(SIGINT, close_client);
    if(main_key>=0)
    {
        printf("Server setup accomplished. Defined queue number: %d.\n", main_key);
        mainloop(userList, groupList, main_key);
        msgctl(main_key, IPC_RMID, 0);
    }
    else
        printf("Unable to open a queue\n");
    return 0;
}
