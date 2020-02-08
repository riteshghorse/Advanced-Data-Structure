#include<stdio.h>
#include<string.h>
#include<stdlib.h>

char command[4], id[15], record[100];
char commands[][5] = {"add", "find", "del", "end"};


int main(int argc, char* argv[])
{
    // 3 arguments to be passed
    // argv[1] -> operation to be performed
    // argv[2] -> name of student db file
    FILE *fp;
    int len;
    char *buf;
    fp = fopen("student.db", "rb");
    while(!feof(fp))
    {
        fread(&len, sizeof(int), 1, fp);
        buf = (char*)malloc(len);
        fread(buf, len, 1, fp);
        printf("\nentry: %s", buf);
    }
    fclose(fp);
    return 0;
}