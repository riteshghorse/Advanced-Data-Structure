#include<stdio.h>
#include<string.h>


char command[4], id[15], record[100];
char commands[][5] = {"add", "find", "del", "end"};


int main(int argc, char* argv[])
{
    // 3 arguments to be passed
    // argv[1] -> operation to be performed
    // argv[2] -> name of student db file
    FILE *fp;
    char dbname[20], mode[20];
    long rec_off;
    int rec_size;
    int len;
    strcpy(dbname, argv[2]);
    // printf("\n%s", dbname);
    if ( ( fp = fopen( "student.db", "r+b" ) ) == NULL ) 
    { 
        fp = fopen( "student.db", "w+b" ); 
    } 
    do
    {
        
        scanf("%s", command);
        if(!strcmp(command, commands[0]))
        {
            printf("\n in add");
            scanf("%s", id);
            scanf("%s", record);
            len = strlen(record);
            fwrite(&len, sizeof(int), 1, fp);
            fwrite(record, len, 1, fp);
            // add_first_fit(record);
        }
        else if(!strcmp(command, commands[1]))
        {
            scanf("%s", id);
            // find(id);
        }
        else if(!strcmp(command, commands[2]))
        {
            scanf("%s", id);
            // del_first_fit(id);
        }
        else
            break; 

        // printf("\nStart\n%s", command);
        // printf("\n%s", id);
        // printf("\n%s\nEnd", record);
    } while (1);
    fclose(fp);
    
    return 0;
}