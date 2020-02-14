# Client-server communication protocol
## Communication
Server by default starts single queue, on a defined 'port'. On this queue under a specific id, clients can send login request. 
Server sends response with type equals client's PID. If it's an success, it also creates separate queue to communicate with this user. 
If another process logs into the same user, previous process receives shut down request. Request uses `LoginReq` structure, and response is `ActionResponse`.

Client can send requests with defined parameters on its own queue, and server sends response with a type equals to request type, shifted by a constant. 
All actions can be served by `ActionRequest` and `ActionResponse`.

Client has 2 processes. First, main is taking care of communication with user. Second is receiving messages and displays them on the screen.
Sending and receiving message is based on `Message` structure (sending user also receive `ActionResponse`).

If server shut downs, it sends logout requests to all users, which cause process queue's closing. 
## Structures
- User
    - Username - string field with size up to 255. Unique user name.
    - Password - string field with size up to 255. Password required to login.
    - client_pid - unsigned int field. PID of client's process logged in as this user.
    - ipc_id - unsigned int field. ID of a queue connected to an user's process.
    - failed login count - unsigned int field. Number of failed login attemps. It bigger then 3, then user become locked. 
- UserList
    - user - User field with pointer to an specific user.
    - next - UserList pointer to the next user. 
 - Group
    - name - string field with size up to 255. Unique name of a group.
    - users - Userlist pointer. List of users signed into the group.
 - GroupList
     - group - Group field with pointer to a specific group.
     - next - GroupList pointer to the next group. 
 - Message
    - mtype - long. Message type, bigger then 0.
    - from - string field with size up to 255. Field for an username of sending user.
    - content - string field with size up to 255. Field for a target's username or groupname.
    - content - string field with size up to 1024. Field for a message's content.
- LoginReq
    - mtype - long. Message type, bigger then 0.
    - username - string field with size up to 255. Log in username.
    - password - string field with size up to 255. Log in username.
    - client_pid - unsigned int field. PID of client's process sending request.
 - ActionRequest
    - mtype - long. Message type, bigger then 0. Type of an specific action.
    - parameters - string field with size up to 255. Optional, additional parameters to an action.
 - ActionResponse
    - mtype - long. Message type, bigger then 0. Type of an specific action.
    - success - int. 0 -> action failed, 1 -> action accomplished.
    - content - string field with size up to 1024. Server's response for an specific action.

## Constant
**Default communication port - 1008**

- login request - 1 - LOGIN_REQ
- user message response - 1 - USR_MSG_RES
- group message response - 2 - GRP_MSG_RES
- logout request - 3 - LOGOUT_REQ
- active user list request - 4 - ACTIVE_USR_REQ
- users in group list request - 5 - USR_IN_GRP_REQ
- groups list request - 6 - LIST_GRP_REQ
- group sign in request - 10 - GRP_SIGNIN_REQ
- group sign out request - 11 - GRP_SIGNOUT_REQ
- user message send request - 21 - USR_MSG_REQ
- group message send request - 22 - GRP_MSG_REQ

Maximum request types - 11 - MAX_REQ_TYPES
**There is special shift between request and responses from server (look at REQ_RES_SHIFT constant. By default = 100).**

### All constants are defined in `./src/definitions.h` file.