#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include "structs/user.h"
#include "structs/group.h"

int main(int argc, char** argv) {
    UserList* userList = create_usr_list_from_file("../config.txt");
    GroupList* groupList = create_grp_list_from_file("../config.txt");
    int main_key = msgget(1008, 0666|IPC_CREAT);
    int run_mainloop = 1;
    if(main_key)
    {
            LoginReq *login_request = (LoginReq*)malloc(sizeof(LoginReq));
            printf("Waiting for login request\n");
            msgrcv(main_key, login_request, sizeof(LoginReq)- sizeof(long),1,0);
            printf("%s %s\n", login_request->username, login_request->password);
//                login_user(
//                        find_on_usr_list(userList, login_request->username),
//                        login_request->password,
//                        login_request->pid);
    }
    printf("Hello, Server!\n");
    return 0;
}
