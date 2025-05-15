#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <sys/wait.h>

#define PORT 8080

static int pam_conv_callback(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr) {
    struct pam_response *response = (struct pam_response *)malloc(sizeof(struct pam_response) * num_msg);
    for (int i = 0; i < num_msg; i++) {
        if (msg[i]->msg_style == PAM_PROMPT_ECHO_OFF) {
            response[i].resp = strdup((char *)appdata_ptr);
            response[i].resp_retcode = 0;
        } else {
            response[i].resp = NULL;
            response[i].resp_retcode = 0;
        }
    }
    *resp = response;
    return PAM_SUCCESS;
}

int pam_auth(const char *user, const char *password) {
    struct pam_conv pam_conversation = {pam_conv_callback, (void *)password};
    pam_handle_t *pamh = NULL;
    int ret;

    ret = pam_start("passwd-auth", user, &pam_conversation, &pamh);
    if (ret != PAM_SUCCESS) {
        return 0;
    }

    ret = pam_authenticate(pamh, 0);
    pam_end(pamh, ret);

    return (ret == PAM_SUCCESS);
}

void start_daemon() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("Daemon Running...\n");

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        read(client_fd, buffer, 1024);
        printf("Request: %s\n", buffer);

        char username[100], old_password[100], new_password[100];
        sscanf(buffer, "%s %s %s", username, old_password, new_password);

        if (pam_auth(username, old_password)) {
            pid_t pid = fork();
            if (pid == 0) { // 子行程
                char *cmd = "/usr/sbin/chpasswd";
                char input[200];
                snprintf(input, sizeof(input), "%s:%s", username, new_password);
                char *argv[] = {"chpasswd", NULL};
                char *envp[] = {NULL};

                int pipefd[2];
                pipe(pipefd);
                write(pipefd[1], input, strlen(input));
                close(pipefd[1]);
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[0]);

                execve(cmd, argv, envp);
                perror("execve failed");
                exit(1);
            }
            wait(NULL);
            write(client_fd, "Password Changed!\n", strlen("Password Changed!\n"));
        } else {
            write(client_fd, "Authentication Failed!\n", strlen("Authentication Failed!\n"));
        }

        close(client_fd);
    }
}

int main() {
    start_daemon();
    return 0;
}
