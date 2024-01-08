#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

/*
void transmit_file(){

    std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::cout<<"[LOG] : Transmission Data Size "<<contents.length()<<" Bytes.\n";

    std::cout<<"[LOG] : Sending...\n";

    int bytes_sent = send(new_socket_descriptor , contents.c_str() , contents.length() , 0 );
    std::cout<<"[LOG] : Transmitted Data Size "<<bytes_sent<<" Bytes.\n";

    std::cout<<"[LOG] : File Transfer Complete.\n";
}*/

int main(int argc, char *argv[])
{
    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char buffer[256];

    if (argc < 2)
    {
        fprintf(stderr,"usage %s port\n", argv[0]);
        return 1;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 1;
    }

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error binding socket address");
        return 2;
    }

    listen(sockfd, 5);
    cli_len = sizeof(cli_addr);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            return 3;
        }

        bzero(buffer, 256);
        n = recv(newsockfd, buffer, 256, 0);
        //n = read(newsockfd, buffer, 255);
        if (n < 0)
        {
            perror("Error reading from socket");
            return 4;
        }
        if (n == 0)
        {
            printf("Client disconnected\n");
            break;
        }

        buffer[n] = '\0';
        printf("Connected with client socket number %d.\n", newsockfd - 3);
        printf("Client socket %d sent message: %s.\n", newsockfd - 3, buffer);

        const char* sprava = buffer;
        printf("%d.\n",strcmp(sprava, "1"));

        const char* msg = "Nefunguje to";
        if (strcmp(sprava, "1") == 0) {
            msg = "Hociaky string";
            printf("Treba dorobit v cliente cyklus, kde bude odpocuvat server.\n");
        }

        n = write(newsockfd, msg, strlen(msg)+1);
        if (n < 0) {
            perror("Error writing to socket");
            return 5;
        }
    }

    close(newsockfd);
    close(sockfd);

    return 0;
}