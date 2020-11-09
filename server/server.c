#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#define PORT 8000
#define BUFFSIZE 1024
char buff[BUFFSIZE];

int sendFile(char *fileName)
{
    FILE *fd = fopen(fileName, "r");
    if (fd == NULL)
    {
        perror("fopen");
        return -1;
    }
    while(fgets(buff,  BUFFSIZE, fd)!= NULL){
        printf("%s\n", buff);
    }
}

int main(int argc, char const *argv[])
{
    int server_fd, new_sock, opt = 1;
    struct sockaddr_in addr;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&addr,
             sizeof(addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    int addrlen = sizeof(addr);
    if ((new_sock = accept(server_fd, (struct sockaddr *)&addr, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    int recvRet = read(new_sock , buff, BUFFSIZE);  
    printf("!%s!\n", buff);
    if (recvRet < 0)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    printf("Recieved request for %s\n", buff);
    sendFile(buff);
}