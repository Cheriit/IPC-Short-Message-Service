//
// Created by cherit on 12/29/19.
//

#ifndef PROJECT_IPC_DEFINITIONS_H
#define PROJECT_IPC_DEFINITIONS_H

#define MAX_TEXTFIELD_SIZE 255
#define MAX_CONTENT_SIZE 1024

#define CONFIG_FILE "../config.txt"
#define CFG_USR_HEADER "[USER]"
#define CFG_GRP_HEADER "[GROUP]"

#define MAIN_PORT 1008

#define MAX_REQ_TYPES 11
#define REQ_RES_SHIFT 100

#define LOGIN_REQ 1
#define LOGOUT_REQ 2
#define ACTIVE_USR_REQ 3
#define USR_IN_GRP_REQ 4
#define LIST_GRP_REQ 5
#define GRP_SIGNIN_REQ 10
#define GRP_SIGNOUT_REQ 11
#endif //PROJECT_IPC_DEFINITIONS_H
