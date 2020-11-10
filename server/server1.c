#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define PURPLE "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[0m"

#define PORT 8000
#define BUFFSIZE 1024
char buff[BUFFSIZE], buff2[BUFFSIZE];

int server_fd, new_sock, opt = 1;
struct sockaddr_in addr;
int numFiles;

int sendFile()
{
    char fileName[BUFFSIZE];
    memset(fileName, 0, BUFFSIZE);

    int recvRet = recv(new_sock, fileName, BUFFSIZE, 0);
    send(new_sock, "done", strlen("done"), 0);
    if (recvRet < 0)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }

    printf("reciecved request for %s\n", fileName);
    int fd = open(fileName, O_RDONLY, 0664);
    // int fd2 = open("testing", O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd < 0)
    {
        perror("open");
        return -1;
    }
    int fileSize = lseek(fd, 0, SEEK_END);
    char fileSizeStr[BUFFSIZE];
    sprintf(fileSizeStr, "%d", fileSize);
    printf("%s\n", fileSizeStr);
    send(new_sock, fileSizeStr, strlen(fileSizeStr), 0);
    recvRet = recv(new_sock, buff, strlen("done"), 0);
    puts(buff);
    if (recvRet < 0)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }

    lseek(fd, 0, SEEK_SET);
    int bytesRead = 0;
    int n;
    float per;
    while (bytesRead < fileSize)
    {
        per = (float)bytesRead / (float)fileSize * 100.00;
        printf("%f %%\r", per);
        memset(buff, 0, BUFFSIZE);
        memset(buff2, 0, BUFFSIZE);
        if (fileSize - bytesRead >= BUFFSIZE)
            n = read(fd, buff, BUFFSIZE);
        else
            n = read(fd, buff, BUFFSIZE);
        int q = send(new_sock, buff, strlen(buff), 0);
        // int m = recv(new_sock, buff2, BUFFSIZE, 0);
        // if (m < 0)
        // {
        //     perror("recv");
        //     exit(EXIT_FAILURE);
        // }
        if (q < 0)
        {
            perror("send");
            exit(EXIT_FAILURE);
        }
        // puts(buff2);

        bytesRead += n;
    }
    printf("%ld", lseek(fd, 0, SEEK_CUR));
    return 0;
}

int main(int argc, char const *argv[])
{
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

    printf(CYAN "server connected!\n" WHITE);
    memset(buff, 0, BUFFSIZE);
    int recvRet = recv(new_sock, buff, BUFFSIZE, 0);
    send(new_sock, "done", sizeof("done"), 0);
    printf("numfiles %s\n", buff);
    if (recvRet < 0)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    numFiles = atoi(buff);

    for (int i = 0; i < numFiles; i++)
    {
        sendFile();
    }

    close(new_sock);
}