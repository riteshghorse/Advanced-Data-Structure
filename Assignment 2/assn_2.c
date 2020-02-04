#include<stdio.h>
#include<string.h>

int main(int argc, char* argv[])
{
    // 3 arguments to be passed
    // argv[1] -> operation to be performed
    // argv[2] -> name of student db file
    FILE *fp;
    char dbname[20], mode[20];
    char command[10];
    char operations[][5]={"add", "find", "del", "end"};
    printf("\nhere");
    strcpy(mode, argv[1]);
    strcpy(dbname, argv[2]);
    
    fp = fopen(dbname, "r+b");
    do
    {
        fread(command, 1, 3*sizeof(char), fp);
        // add key rec
        if(!strcmp(command, operations[0]))
            printf("\nadd");
        // find key
        if(!strcmp(command, operations[1]))
            printf("\nfind");
        // del key
        if(!strcmp(command, operations[2]))
            printf("\ndel");
        // end
        // if(!strcmp(command, "end"))
        //     break;
    } while (strcmp(command, operations[3]));
    fclose(fp);
    return 0;
}