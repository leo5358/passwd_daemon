#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char message[1024];
    char response[1024] = {0};

    printf("Username: ");
    char username[100];
    scanf("%s", username);

    printf("Old Password: ");
    char old_password[100];
    scanf("%s", old_password);

    printf("New Password: ");
    char new_password[100];
    scanf("%s", new_password);

    sprintf(message, "%s %s %s", username, old_password, new_password);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    send(sock, message, strlen(message), 0);
    read(sock, response, 1024);

    printf("Response: %s\n", response);

    close(sock);
    return 0;
}

