#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include "includes/colors.h"
#include "includes/html.h"

#define CONFIG_FILE "Files/server_config.txt"
#define INDEX_FILE "Files/index.html"
#define SIZE 11000
#define SERVER_LOG "Files/server_logs.txt"

void prompt(char *ipAddr, int *port);
int bind_socket(int sock, int port, char *ipAddr);
int confirm_message(char *confirm, char *message);
int send_message(int new_sock, char *message, struct sockaddr_in client);
int recv_message(int new_sock, struct sockaddr_in client, char *lines);
void send_html(int new_sock);
int confirm_html(char *con_html);
void display_time();

char *g_data;
void server(int argc, char **argv)
{
	char ipAddr[16]; 
	int sock, new_sock, port=0, x, b;
	int *ptr = &port;
	char *lines = "*****************************************************************";
	char confirm[3];
	char con_html[3];
	char *buffer = (char *)malloc(SIZE * sizeof(char));
	char *message = (char *)malloc(SIZE * sizeof(char));
	FILE *fp = NULL;

    if(argv[1] && argv[2])
    {
        memcpy(ipAddr, argv[1], strlen(argv[1] + 1));
        port = atoi(argv[2]);
    } else {
	    prompt(ipAddr, ptr);
    }

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if(sock < 0){
		printf("%sError Creating Socket%s\n", KRED, KWHT);
	}

	system("clear");

	printf("\n%s[+]%sSocket creating successful\n", KGRN, KWHT);


	b = bind_socket(sock, port, ipAddr);

	if(b < 0)
		printf("%s[x]%sError binding socket\n", KRED, KWHT);

	printf("%s\n", lines);
	printf("%s%s%s\n", KCYN, lines, KWHT);

	listen(sock, 3);

	confirm_message(confirm, message);
	x = confirm_html(con_html);
	system("clear");

	printf("Listenting on port: %d\n", port);
#if 0
    printf("Before\n");
    generate_html(g_data);
    printf("After\n");
#endif
	while(1){
		struct sockaddr_in client;
		int client_len = sizeof(client);
        char *name;

		fp = fopen(SERVER_LOG, "w");

		printf("%s%s%s\n", KCYN, lines, KWHT);
		fprintf(fp, "%s\n", lines);


		new_sock = accept(sock, (struct sockaddr *)&client, (socklen_t *)&client_len);
		if(new_sock < 0){
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


		if(x == 0){
			send_html(new_sock);
		}

        while(1){
		    if(send_message(new_sock, message, client));
	
		    if(recv_message(new_sock, client, lines));

        }

	}
		fclose(fp);
		close(new_sock);


}

void prompt(char *ipAddr, int *port)
{
	printf("Enter Ip Address of server or write local\n>");
	scanf("%s", ipAddr);
	if(strcmp(ipAddr, "local") == 0)
		strcpy(ipAddr, "127.0.0.1");
	printf("Enter port you wish to run server on\n>");
	scanf("%d", port);
	if(*port == 0){
		*port = 8080;
	}
}

int bind_socket(int sock, int port, char *ipAddr)
{
	int x = -1;
	struct sockaddr_in server_addr;

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_addr.s_addr = inet_addr(ipAddr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	x = bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

	if(x < 0)
		printf("%sSocket Bind Unsuccessful%s\n",KRED, KWHT);
	
	printf("%s[+]%sBind successful!\n", KGRN, KWHT);
	return x;
}

int confirm_message(char *confirm, char *message)
{
	printf("%s[?]%sWould you like to send an initial message?[yes/no]\n>", KYEL, KWHT);
	scanf("%s", confirm);

	if(strcmp(confirm, "yes") == 0){
		printf("Enter message\n>");
		scanf("%s", message);
	} else {
		message = "Hello from server";
	}
    message = (char*)realloc(message, strlen(message));
	return 0;
}

void server_config()
{
	char c;
	FILE *fp = NULL;

	fp = fopen(CONFIG_FILE, "r");

	c = fgetc(fp);
	while(c != EOF){
		printf("%c", c);
		c = fgetc(fp);
	}
	fclose(fp);
	printf("\n");
}

int confirm_html(char *con_html)
{
	printf("%s[?]%sWould you like to broadcast info.html?\n>", KYEL, KWHT);
	scanf("%s", con_html);
	if(strcmp(con_html, "yes") < 0){
		return -1;
	} else {
		return 0;
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
        g_data = data;
		return 1;
}

void send_html(int new_sock)
{
	int n;
	FILE *fp = NULL;
	char data[SIZE];

	fp = fopen(INDEX_FILE, "r");
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
	fclose(fp);
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
