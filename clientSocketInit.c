#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include "includes/colors.h"
#include "includes/html.h"
#include "includes/server.h"

void addClientToSet(int max_clients, int sd, fd_set readfds, int * client_sockets, int max_sd); 
int confirmMessage(char *confirm, char *message);
int sendMessage(int new_sock, char *message, struct sockaddr_in client);
int recvMessage(int new_sock, struct sockaddr_in client, char *lines);
void sendHtml(int new_sock);
void displayTime();

extern char g_data[];

void clientSocketInit(int master_sock, int client_sock, int max_clients, int *client_sockets)
{
        fd_set readfds;
		struct sockaddr_in client;
        int sd,  max_sd, activity, valread;
		int client_len = sizeof(client);
        char *name;
        char *message = (char *)malloc(SIZE * sizeof(char));
        char buffer[1024];
        char lines[] = " ";
        FILE *fp;
		fp = fopen(SERVER_LOG, "w");

		fprintf(fp, "%s\n", lines);
        //clear socket set
        FD_ZERO(&readfds);
        
        //add master socket to set
        FD_SET(master_sock, &readfds);
        max_sd = master_sock;

        //add client sockets to set
        addClientToSet(max_clients, sd, readfds, client_sockets, max_sd);

        //wait for an activity on one of the sockets, timeout is NULL
        //so wait indefinitly
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }

        //if something happend on the master socket, then it's an incoming connection
        if(FD_ISSET(master_sock, &readfds))
        {
	    	client_sock = accept(master_sock, (struct sockaddr *)&client, (socklen_t *)&client_len);
	    	if(client_sock < 0){
		    	printf("%sSocket accept unsuccessful%s\n", KRED, KWHT);
			    fprintf(fp, "Socket accept unsuccessful\n");
			    exit(1);
		    }

            if(strcmp(inet_ntoa(client.sin_addr), "192.168.0.190") == 0)
            {
                name = (char *)malloc(sizeof(char) * 16);
                memcpy(name, "Raspberry Pi #x", 16);
            }

            displayTime();
		    printf("%s%s%s\n", KCYN, lines, KWHT);
		    printf("\n%s[+]%sSocket accept with %s successful!\n", KGRN, KWHT, inet_ntoa(client.sin_addr));
		    fprintf(fp, "\n[+]Socket accept with %s successful!\n", inet_ntoa(client.sin_addr));

		    recvMessage(client_sock, client, lines); 

		    sendMessage(client_sock, message, client);
	
            generateHtml(g_data);

            //add new socket to array of sockets
            for(int i = 0; i < max_clients; i++)
            {
                //if position is empty
                if(client_sockets[i] == 0)
                {
                    client_sockets[i] = client_sock;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
    }
    //else its some IO operation on some other socket
    for(int i = 0; i < max_clients; i++)
    {
        sd = client_sockets[i];

        if(FD_ISSET(sd, &readfds))
        {
            //check if it was for closing, ad also read the incoming message
            if((valread = read( sd, buffer, 1024)) == 0)
            {
                //somebody disconnected, get details and print
                getpeername(sd, (struct sockaddr*)&client, (socklen_t*)&client_len);
                printf("Host disconnected, ip %s, port  %d \n", inet_ntoa(client.sin_addr),
                        ntohs(client.sin_port));

                //close the socket and mark as 0 in list for reuse
                close(sd);
                client_sockets[i] = 0;
            }
        }
    }

}

void addClientToSet(int max_clients, int sd, fd_set readfds, int *client_sockets,
        int max_sd)
{
        for(int i = 0; i < max_clients; i ++)
        {
            //socket descrtiptor
            sd = client_sockets[i]; 

            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET(sd, &readfds);

            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }

}

int sendMessage(int new_sock, char *message, struct sockaddr_in client)
{
	if(send(new_sock, message, 1024, 0) < 0){
		printf("%s[x]%sError sending message", KRED, KWHT);
		return -1;
	}

    displayTime();
	printf("%s[+]%sMessage sent to %s\n", KGRN, KWHT, inet_ntoa(client.sin_addr));
	printf("Message:\t%s\n", message);
	return 1;
}

int recvMessage(int new_sock, struct sockaddr_in client, char *lines)
{
        char data[10];
		if(recv(new_sock, data, 10, 0) < 0){
			printf("%sError in receiving data%s\n", KRED, KWHT);
			return -1;
		}
        displayTime();
		printf("%s[+]%sMessage Received from %s\n",KGRN, KWHT, inet_ntoa(client.sin_addr));
		printf("%sData: %s%s\n",KCYN, data, KWHT);
		return 1;
}

void sendHtml(int new_sock)
{
	int n;
	FILE *fp = NULL;
	char data[SIZE];

	fp = fopen("Files/home.html", "r");
	if(fp == NULL)
		printf("%s[x]%sCould not find html file\n", KRED, KWHT);

	while(!feof(fp)){
		n = fread(data, sizeof(unsigned char), SIZE, fp);
		if(n < 0){
			printf("%s[x]%sCould not read file", KRED, KWHT);
			break;
		}

		if(send(new_sock, data, SIZE, 0) < 0){
			printf("%s[x]%sCould not send file", KRED, KWHT);
			break;
		}
	}
	printf("%s[+]%sHTML File Sent\n", KGRN, KWHT);
}

void displayTime()
{
    int hours, minutes, seconds, day, month, year;

    time_t now;
    //time() returns current time
    time(&now);

    //convert to local time foramt and print the stdout
    printf("%sTime: %s %s ", KYEL, ctime(&now), KWHT);
}
