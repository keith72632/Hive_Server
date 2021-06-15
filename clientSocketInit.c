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

int confirm_message(char *confirm, char *message);
int send_message(int new_sock, char *message, struct sockaddr_in client);
int recv_message(int new_sock, struct sockaddr_in client, char *lines);
void send_html(int new_sock);
void display_time();

extern char g_data[];

void clientSocketInit(int master_sock, int client_sock)
{
       fd_set readfds;
		struct sockaddr_in client;
        int max_sd;
		int client_len = sizeof(client);
        char *name;
        char *message = (char *)malloc(SIZE * sizeof(char));
        char lines[] = " ";
        FILE *fp;
		fp = fopen(SERVER_LOG, "w");

		fprintf(fp, "%s\n", lines);
        //clear socket set
        FD_ZERO(&readfds);
        
        //add master socket to set
        FD_SET(master_sock, &readfds);
        max_sd = master_sock;

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

        display_time();
		printf("%s%s%s\n", KCYN, lines, KWHT);
		printf("\n%s[+]%sSocket accept with %s successful!\n", KGRN, KWHT, inet_ntoa(client.sin_addr));
		fprintf(fp, "\n[+]Socket accept with %s successful!\n", inet_ntoa(client.sin_addr));



        while(1){
//            send_html(new_sock);

		    if(send_message(client_sock, message, client) < 1)break;
	
		    if(recv_message(client_sock, client, lines) < 1)break;
            
            generate_html(g_data);

        }


}

int send_message(int new_sock, char *message, struct sockaddr_in client)
{
	if(send(new_sock, message, 1024, 0) < 0){
		printf("%s[x]%sError sending message", KRED, KWHT);
		return -1;
	}

    display_time();
	printf("%s[+]%sMessage sent to %s\n", KGRN, KWHT, inet_ntoa(client.sin_addr));
	printf("Message:\t%s\n", message);
	return 1;
}

int recv_message(int new_sock, struct sockaddr_in client, char *lines)
{
        char data[10];
        //buffer = (char *)malloc(40 * sizeof(char));
		if(recv(new_sock, data, 10, 0) < 0){
			printf("%sError in receiving data%s\n", KRED, KWHT);
			return -1;
		}
        //data = atoi(buffer);
        display_time();
		printf("%s[+]%sMessage Received from %s\n",KGRN, KWHT, inet_ntoa(client.sin_addr));
		printf("%sData: %s%s\n",KCYN, data, KWHT);
		//printf("%s%s%s\n", KCYN, lines, KWHT);
        //g_data = data;
		return 1;
}

void send_html(int new_sock)
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

void display_time()
{
    int hours, minutes, seconds, day, month, year;

    time_t now;
    //time() returns current time
    time(&now);

    //convert to local time foramt and print the stdout
    printf("%sTime: %s %s ", KYEL, ctime(&now), KWHT);
}
