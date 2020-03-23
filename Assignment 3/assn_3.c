#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int comparator(const void *first, const void *second)
{
    int l = (int)first;
    int r = (int)second;
    return (l - r);
}

void createRuns(char filename[20])
{
    int ip_buffer[1000];
    char op_file[30], temp[20];
    FILE *ip, *op;
    int index;

    ip = fopen(filename, "rb");
    index = 0;
    while (fread(ip_buffer, sizeof(char), 1000, ip) != 0)
    {
        sprintf(temp, ".%03d", index);
        strcpy(op_file, filename);
        strcat(op_file, temp);
        qsort((void*)ip_buffer, 1000, sizeof(int), comparator);
        op = fopen(op_file, "wb");
        fwrite(ip_buffer, sizeof(char), 1000, op);
        index += 1;        
    }
}

int main(int argc, char *argv[])
{
    char METHODS[3][20] = {"--basic", "--multistep", "--replacement"};
    char method[20], ip_filename[20], op_filename[20];
    int ip_buffer[1000], op_buffer[1000];

    strcpy(method, argv[1]);
    strcpy(ip_filename, argv[2]);
    strcpy(op_filename, argv[3]);

    if(!strcmp(method, METHODS[0]))
    {
        // basic
        createRuns(ip_filename);

    }
    else if(!strcmp(method, METHODS[0]))
    {
        // multistep
    }
    else if(!strcmp(method, METHODS[0]))
    {
        // replacement
    }
    else
    {
        printf("Pass a valid method\n");
    }


    // for(i=0; i<argc; ++i)
    //     printf("%s\n", argv[i]);    
    return 0;
}