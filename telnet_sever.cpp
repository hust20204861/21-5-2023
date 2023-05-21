#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main() {
    int server_socket, client_socket[MAX_CLIENTS];
    struct sockaddr_in server_address, client_address;
    int max_clients = MAX_CLIENTS;
    int activity, i, valread, sd;
    int max_sd;
    char buffer[BUFFER_SIZE];
    fd_set readfds;

    // Khá»Ÿi táº¡o máº£ng client_socket Ä‘á»ƒ lÆ°u cÃ¡c socket cá»§a cÃ¡c client
    for (i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    // Táº¡o socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("KhÃ´ng thá»ƒ táº¡o socket");
        exit(EXIT_FAILURE);
    }

    // Thiáº¿t láº­p thÃ´ng tin Ä‘á»‹a chá»‰ vÃ  cá»•ng cá»§a server
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    // GÃ¡n Ä‘á»‹a chá»‰ vÃ  cá»•ng cho server socket
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("KhÃ´ng thá»ƒ gÃ¡n Ä‘á»‹a chá»‰ vÃ  cá»•ng cho server socket");
        exit(EXIT_FAILURE);
    }

    // Láº¯ng nghe cÃ¡c káº¿t ná»‘i Ä‘áº¿n
    if (listen(server_socket, 3) < 0) {
        perror("Lá»—i trong quÃ¡ trÃ¬nh láº¯ng nghe káº¿t ná»‘i Ä‘áº¿n");
        exit(EXIT_FAILURE);
    }

    printf("Telnet server Ä‘Ã£ sáºµn sÃ ng\n");

    while (1) {
        // Thiáº¿t láº­p táº­p file descriptors Ä‘á»ƒ theo dÃµi
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        max_sd = server_socket;

        // ThÃªm cÃ¡c socket cá»§a clients vÃ o táº­p file descriptors
        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // Sá»­ dá»¥ng hÃ m select Ä‘á»ƒ chá» sá»± kiá»‡n xáº£y ra trÃªn cÃ¡c socket
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        /*if ((activity < 0) && (errno != EINTR)) {
            perror("Lá»—i trong quÃ¡ trÃ¬nh chá» sá»± kiá»‡n");
            exit(EXIT_FAILURE);
        }*/

        // Xá»­ lÃ½ káº¿t ná»‘i má»›i
        if (FD_ISSET(server_socket, &readfds)) {
            int new_socket;
            int client_address_length = sizeof(client_address);
            if ((new_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_address_length)) < 0) {
                perror("Lá»—i trong quÃ¡ trÃ¬nh cháº¥p nháº­n káº¿t ná»‘i má»›i");
                exit(EXIT_FAILURE);
            }
            printf("Káº¿t ná»‘i má»›i Ä‘Æ°á»£c cháº¥p nháº­n\n");

            // ThÃªm káº¿t ná»‘i má»›i vÃ o máº£ng client_socket
            for (i = 0; i < max_clients; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    break;
                }
            }
        }

        // Xá»­ lÃ½ dá»¯ liá»‡u tá»« cÃ¡c client Ä‘ang káº¿t ná»‘i
        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            if (FD_ISSET(sd, &readfds)) {
                // Äá»c dá»¯ liá»‡u tá»« client
                memset(buffer, 0, sizeof(buffer));
                valread = read(sd, buffer, sizeof(buffer));

                // Xá»­ lÃ½ yÃªu cáº§u Ä‘Äƒng nháº­p
                if (strstr(buffer, "user") != NULL && strstr(buffer, "pass") != NULL) {
                    char user[50];
                    char pass[50];
                    sscanf(buffer, "user %s pass %s", user, pass);

                    // So sÃ¡nh vá»›i file cÆ¡ sá»Ÿ dá»¯ liá»‡u
                    FILE *database = fopen("database.txt", "r");
                    char line[100];
                    int found = 0;
                    while (fgets(line, sizeof(line), database) != NULL) {
                        char db_user[50];
                        char db_pass[50];
                        sscanf(line, "%s %s", db_user, db_pass);
                        if (strcmp(user, db_user) == 0 && strcmp(pass, db_pass) == 0) {
                            found = 1;
                            break;
                        }
                    }
                    fclose(database);

                    // Gá»­i káº¿t quáº£ Ä‘Äƒng nháº­p cho client
                    if (found) {
                        send(sd, "ÄÄƒng nháº­p thÃ nh cÃ´ng\n", strlen("ÄÄƒng nháº­p thÃ nh cÃ´ng\n"), 0);
                    } else {
                        send(sd, "Lá»—i Ä‘Äƒng nháº­p\n", strlen("Lá»—i Ä‘Äƒng nháº­p\n"), 0);
                    }
                }
                // Xá»­ lÃ½ lá»‡nh tá»« client
                else {

                   
                    FILE *output_file = fopen("out.txt", "w"); // Má»Ÿ file out.txt Ä‘á»ƒ lÆ°u káº¿t quáº£ lá»‡nh
                    if (output_file == NULL) {
                        perror("Lá»—i khi má»Ÿ file");
                        exit(EXIT_FAILURE);
                    } else printf("Mo file thÃ nh cÃ´ng!\n");

                    
                    // LÆ°u káº¿t quáº£ vÃ o file out.txt
                    snprintf(output_file, sizeof(buffer), "%s\n", buffer);

                    printf("Ä‘aÌƒ nhÃ¢Ì£n yÃªu cáº§u!\n");


                    // Gá»­i káº¿t quáº£ lá»‡nh cho client
                    send(sd, "Gá»­i thÃ nh cÃ´ng!\n", strlen("Gá»­i thÃ nh cÃ´ng!\n"), 0);
                }
            }
        }
    }

return 0;
}