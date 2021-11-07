#include "WOpenGL.h"
#include <stdio.h>
#include <stdlib.h>

char *openShader(const char *fname)
{
    FILE *fp;
    fp = fopen(fname, "r");

    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    // Get file size
    fseek(fp, 0L, SEEK_END);
    long fileSize = ftell(fp);
    rewind(fp);

    char *data = (char*)malloc(fileSize+1);

    fread(data,fileSize,1,fp);

    fclose(fp);
    return data;
}
