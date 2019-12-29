//
// Created by cherit on 12/29/19.
//

#include "request.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ActionResponse *make_request(key_t ipc_id, int key, char *parameters) {
    ActionRequest* request = (ActionRequest*)malloc(sizeof(ActionRequest));
    ActionResponse* response = (ActionResponse*)malloc(sizeof(ActionResponse));
    request->mtype = key;
    strcpy(request->parameters, parameters);
    msgsnd(ipc_id, request, sizeof(ActionRequest)-sizeof(long), 0);
    msgrcv(ipc_id, response, sizeof(ActionResponse)-sizeof(long), key+100, 0);
    free(request);
    return response;
}
int make_response(key_t ipc_id, int key, int status, char *content) {
    ActionResponse* response = (ActionResponse*)malloc(sizeof(ActionResponse));
    response->mtype = key;
    response->success = status;
    strcpy(response->content, content);
    msgsnd(ipc_id, response, sizeof(ActionResponse)- sizeof(long), 0);
    free(response);
    return 0;
}

char *parse_out(char string[1024]) {
    for(int i=0;i<1024;i++)
    {
        if(string[i] == ';') string[i] = '\n';
    }
    return string;
}