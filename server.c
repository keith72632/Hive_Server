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
#include "includes/client_socket.h"

void prompt(char *ipAddr, int *port);
void bindSocket(int sock, int port, char *ipAddr, int *server_addr_len);
int confirm_message(char *confirm, char *message);
int send_message(int new_sock, char *message, struct sockaddr_in client);
int recv_message(int new_sock, struct sockaddr_in client, char *lines);
void send_html(int new_sock);
int confirm_html(char *con_html);
void display_time();
void initClientSockets(int num_clients, int *clients);
void socketOptionReuseAddr(int master_socket, int option);
void initMasterSocket(int *master_sock);
void serverListen(int master_sock, int port, const int pending);

char g_data[] = "hey";
void serverInit(int argc, char **argv)
{
	char ipAddr[16]; 
	int master_sock, new_sock, port=0, x, b, server_addr_len, client_socket[30], max_clients=30, opt=TRUE;
	int *ptr = &port;
	char *lines = "*****************************************************************";
	char confirm[3];
	char con_html[3];
	char *buffer = (char *)malloc(SIZE * sizeof(char));
	char *message = (char *)malloc(SIZE * sizeof(char));

    if(argv[1] && argv[2])
    {
        memcpy(ipAddr, argv[1], strlen(argv[1] + 1));
        port = atoi(argv[2]);
    } else {
	    prompt(ipAddr, ptr);
    }

    initClientSockets(max_clients, client_socket);

    initMasterSocket(&master_sock);

    socketOptionReuseAddr(master_sock, opt);
    
    system("clear");

    bindSocket(master_sock, port, ipAddr, &server_addr_len);

	confirm_message(confirm, message);
	
    confirm_html(con_html);
	
    system("clear");

    serverListen(master_sock, port, 3);

	while(1){
        clientSocketInit(master_sock, new_sock); 
	}
	
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

void initClientSockets(int num_clients, int *clients)
{
    for(int i = 0; i < num_clients; i ++)
        clients[i] = 0;    
}

void initMasterSocket(int *master_sock)
{
    int sock;

	if( (sock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
		printf("%sError Creating Socket%s\n", KRED, KWHT);
	}    
    *master_sock = sock;
}

void socketOptionReuseAddr(int master_socket, int option)
{
    if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&option,
               sizeof(option)) < 0)
    {
       perror("setsockopt");
       exit(EXIT_FAILURE);
    } 
}

void bindSocket(int sock, int port, char *ipAddr, int *server_addr_len)
{
	int x;
	struct sockaddr_in server_addr;

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_addr.s_addr = inet_addr(ipAddr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	x = bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

	if(x < 0)
		printf("%sSocket Bind Unsuccessful%s\n",KRED, KWHT);

    else	
	    printf("%s[+]%sBind successful!\n", KGRN, KWHT);

    *server_addr_len = sizeof(server_addr);
}

void serverListen(int master_sock, int port, const int pending)
{
	if(listen(master_sock, 3) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("%sListening on port: %d\n%s", KYEL, port, KWHT);
}

int confirm_message(char *confirm, char *message)
{
	printf("%s[?]%sWould you like to send an initial message?[yes/no]\n>", KYEL, KWHT);
	scanf("%s", confirm);

	if(strcmp(confirm, "yes") == 0){
		printf("Enter message\n>");
		scanf("%s", message);
	} else {
		strcpy(message, "Hello from server\n");
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

