#include <stdio.h>
#include "includes/html.h"

int generate_html(char * g_data)
{
    FILE *fp = NULL; 

    fp = fopen(HTML_FILE, "w");

    fprintf(fp, "%s", "<!DOCTYPE HTML>\n<html><title>Hello World</title><html>");

    fclose(fp);

    return 1;
}
