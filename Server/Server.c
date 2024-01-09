#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    int status = 0; // 0 = idle; 1 = sending file; 2 = recieving file;
    char buffer[1024];

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

        if (status == 0) {
            bzero(buffer, 1024);
            n = recv(newsockfd, buffer, 1024, 0);
            //n = read(newsockfd, buffer, 255);
            if (n < 0) {
                perror("Error reading from socket");
                return 4;
            }
            if (n == 0) {
                printf("Client disconnected\n");
                break;
            }

            buffer[n] = '\0';
            printf("Connected with client socket number %d.\n", newsockfd - 3);
            printf("Client socket %d sent message: %s.\n", newsockfd - 3, buffer);

            const char *sprava = buffer;

            const char *msg = "Nefunguje to";
            if (strcmp(sprava, "R") == 0) {     // recieve file == Sending file to client
                msg = "Sending file to client";
                printf("Sending file to client %d.\n", newsockfd - 3);
                status = 2;
            } else if (strcmp(sprava, "S") == 0) {     // recieve file == Sending file to client
                msg = "Recieving a file from client";
                printf("Recieving a file to client %d.\n", newsockfd - 3);
                status = 1;
            }

            n = write(newsockfd, msg, strlen(msg) + 1);
            if (n < 0) {
                perror("Error writing to socket");
                return 5;
            }
        } else if (status == 1) {
            recv(sockfd, buffer, BUFSIZ, 0);
            int subr = newsockfd - 3;
            const char* fileName = subr + ".txt";
            int file_size = atoi(buffer);
            //fprintf(stdout, "\nFile size : %d\n", file_size);

            FILE* received_file = fopen(fileName, "w");
            if (received_file == NULL)
            {
                fprintf(stderr, "Failed to open file --> %s\n", strerror(errno));

                exit(EXIT_FAILURE);
            }

            int remain_data = file_size;
            ssize_t len;

            while ((remain_data > 0) && ((len = recv(sockfd, buffer, BUFSIZ, 0)) > 0))
            {
                fwrite(buffer, sizeof(char), len, received_file);
                remain_data -= len;
                fprintf(stdout, "Receive %d bytes and we hope :- %d bytes\n", len, remain_data);
            }
            fclose(received_file);
            status = 0;
        } else if (status == 2) { // send

            int subr = newsockfd - 3;
            unsigned long fsize;
            const char* fileName = subr + ".txt";
            FILE* file;
            file = fopen(fileName, "rb");
            if (file == NULL)
            {
                printf("File not found!\n");
                return 1;
            }
            else
            {
                printf("Found file %s\n", fileName);

                fseek(file, 0, SEEK_END);
                fsize = ftell(file);
                rewind(file);

                printf("File contains %ld bytes!\n", fsize);
                printf("Sending the file now");
            }

            while (1)
            {
                int bytes_read = fread(buffer, sizeof(char),sizeof(buffer), file);
                if (bytes_read == 0)
                    break;

                if (bytes_read < 0)
                {
                    printf("ERROR reading from file");
                    return 9;
                }

                void *p_bytes = buffer;
                while (bytes_read > 0)
                {
                    int bytes_written = write(sockfd, buffer, bytes_read);
                    if (bytes_written <= 0)
                    {
                        printf("ERROR writing to socket\n");
                        return 10;
                    }
                    bytes_read -= bytes_written;
                    p_bytes += bytes_written;
                }
            }

            printf("Done Sending the File!\n");
            printf("Now Closing Connection.\n");

            status = 0;
        }

    }

    close(newsockfd);
    close(sockfd);

    return 0;
}