#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct file_record{
    char filename[30];    //which file's part is in buffer
    int read_till;  // how much of the file is read
    int index;  // entries of 
}file_record;

int file_count = 0, total_runs = 0;

char METHODS[3][20] = {"--basic", "--multistep", "--replacement"};
char method[20], ip_filename[20], op_filename[20];

int comparator(const void *first, const void *second)
{
    int l = (int)first;
    int r = (int)second;
    return (l - r);
}

void fillBuffer(int i, int *ip_buffer, file_record *fr)
{
    
    FILE *fp;
    int j, ind=0;
    char file[30];
    strcpy(file, fr[i].filename);
    fp = fopen(file, "rb");
    fseek(fp, fr[i].read_till, SEEK_CUR);
    if(feof(fp) && file_count < total_runs)
    {
        fclose(fp);
        sprintf(file, "input.bin.%3d", file_count);
        ++file_count;
        fp = fopen(file, "rb");
    }
    else if(feof(fp) && file_count >= total_runs)
    {
        fclose(fp);
        return;
    }
    fr[i].index = 20 * i;
    ind = fr[i].index;
    for(j=0; j<20; ++j)
    {
        fread(ip_buffer[ind], sizeof(int), 1, fp);
        ++ind;
        ++(fr[i].read_till);
    }
    fclose(fp);
}

void basicMergesort()
{
    // we will do 50-way merge
    file_record fr[50]; 
    char file[30];
    int ip_buffer[1000], op_buffer[1000];
    int i, written=0;   
    int op_index=0, temp_index, max_value;
    FILE *op;

    // load 20 keys from first 50 files each
    for(i=0; i<50 and i<total_runs; ++i)
    {
        sprintf(file, "input.bin.%3d", file_count);
        ++file_count;
        fr[i].read_till = 0
        strcpy(fr[i].filename, file);
        fillBuffer(i, ip_buffer, fr);
    }

    while(written < total_runs)
    {
        temp_index = 0;
        min_value = ip_buffer[fr[0].index];

        for(i=1; i<50; ++i)
        {
            if(min_value > ip_buffer[fr[i].index])
            {
                temp_index = i;
                min_value = ip_buffer[fr[i].index];
            }
        }
        op_buffer[op_index++] = min_value;
        ++fr[temp_index].index;
        // check for op buffer full and if run is exhausted
        if(op_index == 1000)
        {
            op = fopen(op_filename, "ab+");
            fwrite(op_buffer, sizeof(int), 1000, op);
            fclose(op);
            ++written;
        }
        if(fr[temp_index].index % 20 == 0)
        {
            fillBuffer(temp_index, ip_buffer, fr);
        }
    }
    
}

void createRuns(char filename[20])
{
    int ip_buffer[1000];
    char op_file[30], temp[20];
    FILE *ip, *op;
    int index;

    ip = fopen(filename, "rb");
    index = 0;
    while (fread(ip_buffer, sizeof(int), 1000, ip) != 0)
    {
        sprintf(temp, ".%03d", index);
        strcpy(op_file, filename);
        strcat(op_file, temp);
        qsort((void*)ip_buffer, 1000, sizeof(int), comparator);
        op = fopen(op_file, "wb");
        fwrite(ip_buffer, sizeof(int), 1000, op);
        fclose(op);
        total_runs++;
        index++;        
    }
}

int main(int argc, char *argv[])
{
    strcpy(method, argv[1]);
    strcpy(ip_filename, argv[2]);
    strcpy(op_filename, argv[3]);

    if(!strcmp(method, METHODS[0]))
    {
        // basic
        createRuns(ip_filename);
        basicMergesort()

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