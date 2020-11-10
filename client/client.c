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
int sock = 0;
char buff[BUFFSIZE], buff2[BUFFSIZE];

int getFile(const char *fileName)
{
    memset(buff, 0, BUFFSIZE);

    if (send(sock, fileName, strlen(fileName), 0) < 0)
    {
        perror("send");
        return -1;
    }
    if (recv(sock, buff, strlen("done"), 0) < 0)
    {
        perror("recv");
        return -1;
    }

    printf("Request for %s has been made\n", fileName);
    memset(buff, 0, BUFFSIZE);
    if (recv(sock, buff, BUFFSIZE, 0) < 0)
    {
        perror("recv");
        return -1;
    }
    puts(buff);
    if (send(sock, "done", strlen("done"), 0) < 0)
    {
        perror("send");
        return -1;
    }
    puts(buff);
    printf("hmmm\n");
    int fileSize = atoi(buff);
    printf("%d\n", fileSize);
    int fd;
    fd = open(fileName, O_WRONLY | O_TRUNC | O_CREAT, 0664);
    if (fd < 0)
    {
        perror("open");
        return -1;
    }
    int bytesread = 0;
    float per;
    int n;
    int c = 0;
    while (bytesread < fileSize)
    {
        per = (float)bytesread / (float)fileSize * 100.00;
        printf("%f %%\r", per);
        memset(buff, 0, BUFFSIZE);
        n = recv(sock, buff, BUFFSIZE, 0);
        // printf("n: %d %d\n", n, c++);
        if (n < 0)
        {
            perror("recv");
            exit(EXIT_FAILURE);
        }
        if (send(sock, "done", strlen("done"), 0) < 0)
        {
            perror("send");
            return -1;
        }
        // puts(buff);
        if (write(fd, buff, n) < 0)
        {
            perror("write");
            return -1;
        }
        memset(buff, 0, BUFFSIZE);
        bytesread += n;
    }
    printf("done\n");
    sleep(1);
    return 0;
}

int main(int argc, char const *argv[])
{
    struct sockaddr_in addr, server_addr;
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
    fflush(NULL);

    char numfiles[BUFFSIZE];
    sprintf(numfiles, "%d", argc - 1);
    if (send(sock, numfiles, strlen(numfiles), 0) < 0)
    {
        perror("send");
        return -1;
    }
    if (recv(sock, buff, BUFFSIZE, 0) < 0)
    {
        perror("recv");
        return -1;
    }

    puts(buff);

    for (int i = 1; i < argc; i++)
    {
        puts("ready");
        getFile(argv[i]);
    }
}