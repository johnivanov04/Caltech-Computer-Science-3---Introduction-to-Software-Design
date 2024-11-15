#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/prctl.h>
#include <time.h>
#include <string.h>
#include "network_util.h"
#include "server_test_util.h"

struct server_handle {
    int port;
    pid_t pid;
};

typedef server_handle_t serv_t;

serv_t *serv_init(int port, pid_t pid) {
    serv_t *s = malloc(sizeof(serv_t));
    s->port = port;
    s->pid = pid;
    return s;
}

serv_t *server_spawn(const char *bin, int port) {
    pid_t pid = fork();
    if (pid) { // parent
        return serv_init(port, pid);
    }
    // child
    // make file die when parent does
    prctl(PR_SET_PDEATHSIG, SIGHUP);
    assert(port < 100000);
    char port_str[6];
    sprintf(port_str, "%d", port);
    char *argv[] = {
        (char *) bin,
        port_str,
        NULL
    };
    // suppress server output
    int devnull = open("/dev/null", O_RDWR);
    dup2(devnull, STDOUT_FILENO);
    dup2(devnull, STDERR_FILENO);
    execv(bin, argv);
    // if the child fails to exec the server, it just aborts.
    abort();
}

void server_kill(serv_t *s) {
    kill(s->pid, SIGINT);
    free(s);
}

char *server_query(serv_t *s, const char *request) {
    const char LOCALHOST_IP[] = "127.0.0.1";
    const long NANOS_PER_SEC = 1000000000;
    const long RETRY_NANOS = NANOS_PER_SEC / 100;
    const struct timespec RETRY_TIME = {
        .tv_sec = 0,
        .tv_nsec = RETRY_NANOS,
    };
    const size_t NUM_ATTEMPTS = NANOS_PER_SEC / RETRY_NANOS / 10;
    connection_t *conn = NULL;
    char *response = NULL;
    for (size_t i = 0; i < NUM_ATTEMPTS && !conn; i++) {
        nanosleep(&RETRY_TIME, NULL);
        conn = nu_connect_server(LOCALHOST_IP, s->port);
    }
    if (!conn) {
        perror("failed to open connection");
        goto EXIT_NO_CONN;
    }
    if (nu_send_str(conn, request) < 0) {
        perror("failed to send request");
        goto EXIT_CONN;
    }
    char *header = NULL;
    while (!header) {
        header = nu_try_read_str(conn);
    }
    char *body = nu_read_str(conn);
    size_t header_len = strlen(header);
    size_t body_len = strlen(body);
    size_t response_len = header_len + body_len;
    response = malloc(response_len + 1);
    strcpy(response, header);
    strcat(response, body);
    free(header);
    free(body);
    EXIT_CONN:
    nu_close_connection(conn);
    EXIT_NO_CONN:
    return response;
}