#include <stdio.h>
#include "includes/html.h"

int generateHtml(char * g_data)
{
    FILE *fp = NULL; 

    fp = fopen("Files/home.html", "w");

    if(fp == NULL)
    {
        printf("Cannot open file\n");
        return -1;
    }

    if(fprintf(fp, "HTTP/1.1 200 OK\nContent-Length: 10000\nContent-Type: text/html\n\n<!DOCTYPE HTML>\n<html><title>%s</title></html>", 
                g_data) <= 0)
    {
        printf("fprintf error\n");
        fclose(fp);
        return -1;   
    };

    fclose(fp);

    return 1;
}
 
