#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"  // Äá»‹a chá»‰ IP cá»§a telnet_server
#define SERVER_PORT 8080       // Cá»•ng cá»§a telnet_server
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];

    // Táº¡o socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Lá»—i khi táº¡o socket");
        exit(EXIT_FAILURE);
    }

    // Thiáº¿t láº­p thÃ´ng tin Ä‘á»‹a chá»‰ server
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Káº¿t ná»‘i tá»›i server
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Lá»—i khi káº¿t ná»‘i tá»›i server");
        exit(EXIT_FAILURE);
    }

    printf("ÄÃ£ káº¿t ná»‘i tá»›i server. Nháº­p tÃªn ngÆ°á»i dÃ¹ng vÃ  máº­t kháº©u.\n");

    // Nháº­p tÃªn ngÆ°á»i dÃ¹ng vÃ  máº­t kháº©u tá»« bÃ n phÃ­m
    char username[20];
    char password[20];
    printf("TÃªn ngÆ°á»i dÃ¹ng: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;  // XÃ³a kÃ½ tá»± newline tá»« chuá»—i
    printf("Máº­t kháº©u: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;  // XÃ³a kÃ½ tá»± newline tá»« chuá»—i

    // Gá»­i tÃªn ngÆ°á»i dÃ¹ng vÃ  máº­t kháº©u tá»›i server
    snprintf(buffer, sizeof(buffer), "user %s pass %s", username, password);
    send(client_socket, buffer, strlen(buffer), 0);

    // Nháº­n pháº£n há»“i tá»« server
    memset(buffer, 0, sizeof(buffer));
    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("Pháº£n há»“i tá»« server: %s\n", buffer);

    // Kiá»ƒm tra pháº£n há»“i tá»« server
    if (strcmp(buffer, "ÄÄƒng nháº­p thÃ nh cÃ´ng\n") == 0) {
        // YÃªu cáº§u nháº­p lá»‡nh vÃ  gá»­i cho server
        printf("Nháº­p lá»‡nh: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;  // XÃ³a kÃ½ tá»± newline tá»« chuá»—i
        send(client_socket, buffer, strlen(buffer), 0);

        // Nháº­n káº¿t quáº£ tá»« server vÃ  hiá»ƒn thá»‹
        memset(buffer, 0, sizeof(buffer));
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf("Káº¿t quáº£: %s\n", buffer);
    }

    // ÄÃ³ng káº¿t ná»‘i
    close(client_socket);

    return 0;
}