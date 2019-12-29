#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include "structs/user.h"
#include "structs/group.h"
#include "structs/request.h"
#include "structs/server_operations.h"


int user_mainloop(User *user, UserList *userList, GroupList *groupList) {
    if(fork())
    {
        ActionRequest* request = (ActionRequest*)malloc(sizeof(ActionRequest));
        while(user->pid)
        {
            msgrcv(user->ipc_id, request, sizeof(ActionRequest)- sizeof(long), -11, 0);
            printf("Received request type %ld with content %s\n", request->mtype, request->parameters);
            if(request->mtype == 1) logout_user(user);
            else if (request->mtype == 3) list_active_users(userList, user);
            else if (request->mtype == 4) list_users_in_group(user, find_on_grp_list(groupList, request->parameters));
            else if (request->mtype == 5) list_groups(groupList, user);
            else if (request->mtype == 10) sign_to_group(find_on_grp_list(groupList, request->parameters), user);
            else if (request->mtype == 11) sign_out_group(find_on_grp_list(groupList, request->parameters), user);

        }
        exit(0);
    }
    return 1;
}

/**
 * @todo closing queues
 * @todo waiting for processes
 * @todo signal overload
 * @todo kill all processes
 * @todo multiple processes (shared memory)
 * @todo messenger
 * @todo Wall check and fix (sprintf additional var)
 */
int main(int argc, char** argv) {
    UserList* userList = create_usr_list_from_file("../config.txt");
    if(userList == NULL) return 0;
    GroupList* groupList = create_grp_list_from_file("../config.txt");
    key_t main_key = msgget(1008, 0666|IPC_CREAT);
    int run_mainloop = 1;
    if(main_key>=0)
    {
        LoginReq *login_request = (LoginReq*)malloc(sizeof(LoginReq));
        printf("Waiting for login request\n");
        while(run_mainloop)
        {
            msgrcv(main_key, login_request, sizeof(LoginReq)- sizeof(long),1,0);
            if (login_user(userList, login_request, main_key)) user_mainloop(find_on_usr_list(userList, login_request->username), userList, groupList);
        }
        wait(NULL);
        msgctl(main_key, IPC_RMID, 0);
    }
    else
    {
        printf("Unable to open a queue\n");

    }
    return 0;
}
