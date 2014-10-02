#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>

#include "../../inc/ipc.h"
#include "../../inc/actions.h"
#include "../../inc/utils.h"


void server(char *address) {
    printf("S %d\n", getpid());

    database_t *database = db_open("db");
    ipc_t *ipc = ipc_listen(address);
    message_t *msg;

    msg = ipc_recv(ipc);
    res_movie_list(ipc, database, msg->sender);
    free(msg);

    msg = ipc_recv(ipc);
    res_buy_ticket(ipc, database, msg->sender, (req_buy_ticket_t*) &(msg->content));
    free(msg);

    msg = ipc_recv(ipc);
    res_get_ticket(ipc, database, msg->sender, (req_get_ticket_t*) &(msg->content));
    free(msg);

    // printf("S %p\n", msg);
    // printf("S %d, %d\n", msg->sender, msg->content_length);
    // printf("S %s\n", msg->content);

    ipc_close(ipc);
    db_close(database);
}


void client(char *address) {
    int client = getpid();
    printf("C %d\n", client);

    ipc_t* ipc = ipc_connect(address);
    message_t *msg;

    req_movie_list(ipc);
    msg = ipc_recv(ipc);
    han_movie_list((res_movie_list_t*) &(msg->content));
    free(msg);

    req_buy_ticket(ipc, 1);
    msg = ipc_recv(ipc);
    ticket_t ticket = ((res_buy_ticket_t*) &(msg->content))->ticket;
    han_buy_ticket((res_buy_ticket_t*) &(msg->content));
    free(msg);

    req_get_ticket(ipc, ticket);
    msg = ipc_recv(ipc);
    han_get_ticket((res_get_ticket_t*) &(msg->content));
    free(msg);

    ipc_close(ipc);
}


int main() {
    char *address = strdup(IPC_TEST_ADDRESS);

    if (fork() > 0)
        server(address);
    else
        client(address);

    free(address);
}
