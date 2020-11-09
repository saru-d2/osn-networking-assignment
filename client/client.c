#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#define PORT 8000
#define BUFFSIZE 1024
int sock = 0;

int getFile(const char *fileName)
{   
    send(sock, fileName, strlen(fileName), 0);
    printf("Request for %s has been made\n", fileName);
    return 0;
}

int main(int argc, char const *argv[])
{
    struct sockaddr_in addr, server_addr;
    int sock = 0;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation");
        return -1;
    }
    memset(&server_addr, '0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported ");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection Failed");
        return -1;
    }
    //connected
    if (argc == 1)
    {
        printf("No files were requested...\n");
        return 0;
    }

    for (int i = 1; i < argc; i++)
    {
        getFile(argv[i]);
    }
}