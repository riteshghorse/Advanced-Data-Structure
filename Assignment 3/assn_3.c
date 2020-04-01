#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<limits.h>
#include<math.h>

typedef struct file_record{
    char filename[30];    //which file's part is in buffer
    int read_till;  // how much of the file is read
    int index;  // entries of buffer
    int flag;   //if all files are read
}file_record;

int file_count = 0, total_runs = 0, files_read = 0;

char METHODS[3][20] = {"--basic", "--multistep", "--replacement"};
char method[20], ip_filename[20], op_filename[20];


void printBuffer(int buffer[1000])
{   
    int i;
    for(i=0; i<1000; ++i)
        printf(" %d\n", buffer[i]);
}

int comparator(const void *first, const void *second)
{
    int l = *(int*)first;
    int r = *(int*)second;
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

int fillBuffer(int i, int n, int *ip_buffer, file_record *fr, char format[30], int file_num)
{
    FILE *fp;
    int j, ind=0;
    char file[30];
    int buff[1];
    
    strcpy(file, fr[i].filename);
    
    fp = fopen(file, "rb");
    // printf("%s", fr[i].filename);
    fseek(fp, sizeof(int)*fr[i].read_till, SEEK_SET);
    // printf("indexx\n");
    if(feof(fp) && file_count < file_num)
    {
        fclose(fp);
        sprintf(file, "%s.%3d", format, file_count);
        ++file_count;
        strcpy(fr[i].filename, file);
        fr[i].flag = 0;
        fr[i].read_till = 0;
        fp = fopen(file, "rb");
    }
    else if(feof(fp) && file_count >= file_num)
    {
        fr[i].flag = 1;
        fclose(fp);
        return -1;
    }
    fr[i].index = n * i;
    ind = fr[i].index;

    for(j=0; j<n; ++j)
    {
        if(fread(buff, sizeof(int), 1, fp) == 0)
        {
            fr[i].flag = 1;
            ++files_read;
            break;
        }
        // fread(buff, sizeof(int), 1, fp);
        ip_buffer[ind] = buff[0];
        ++ind;
        fr[i].read_till += 1;
    }
    fclose(fp);
    return 0;
}

void mergeGeneral(int runs, char *format, int size, char *op_file)
{
    // runs -> number of runs
    // format -> file name format
    // size -> part of each run
    file_record fr[runs];
    int ip_buffer[1000], op_buffer[1000];
    int op_index, temp_index, min_value;
    int i, written=0;
    char file[30];
    FILE *op;
    for(i=0; i<runs; ++i)
    {
        sprintf(file, "%s.%03d", format, file_count);
        ++file_count;
        fr[i].read_till = 0;
        fr[i].flag = 0;
        strcpy(fr[i].filename, file);
        fillBuffer(i, size, ip_buffer, fr, format, runs);
    }
    op_index = 0;
    written = 0;
    files_read = file_count - runs;
    // printf("files_read: %d\n", files_read);
    // printf("files_count: %d\n", file_count);
    while(written<runs || files_read<file_count)
    {
        min_value = INT_MAX;
        temp_index = -1;
        // find the minimum
        for(i=0; i<runs; ++i)
        {
            if(min_value > ip_buffer[fr[i].index])
            {
                temp_index = i;
                min_value = ip_buffer[fr[i].index];
            }
        }
        if(temp_index != -1)
        {
            op_buffer[op_index++] = min_value;
            ip_buffer[fr[temp_index].index] = INT_MAX;
            fr[temp_index].index += 1;
            if((fr[temp_index].index)%size == 0 && fr[temp_index].flag==0)
            {
                fillBuffer(temp_index, size, ip_buffer, fr, format, runs);
            }
        }

        if(op_index >= 1000)
        {
            ++written;
            // printf("in op_index: %d\n", op_index);
            op = fopen(op_file, "ab+");
            fwrite(op_buffer, sizeof(int), 1000, op);
            fclose(op);
            op_index = 0;
        }
    }
    if(op_index != 0)
    {
        op = fopen(op_file, "ab+");
        fwrite(op_buffer, sizeof(int), op_index, op);
        fclose(op);
    } 
}

void basicMergesort()
{
    // we will do 25-way merge
    int size;
    file_count = 0;
    size = 1000 / total_runs;
    mergeGeneral(total_runs, "input.bin", size, op_filename);
}

void multistepMerge(int n)
{
    // createRuns already called
    // now we have 25 files with 1000 record each
    // merge in the sets of 15
    // total superrun files = ceil(25 / 15) = 2
    
    int super_runs, temp, i;
    char filename[30], run_file[30];

    temp = total_runs;
    super_runs = 0;
    while(temp > 0)
    {
        temp -= n;
        ++super_runs;
    }
    temp = total_runs;
    // printf("total runs: %d\n", total_runs);
    // printf("super runs: %d\n", super_runs);
    file_count = 0;
    for(i=0; i<super_runs; ++i)
    {
        sprintf(filename, "input.bin.super.%03d", i);
        printf("%s\n", filename);
        printf("temp: %d\n", temp);
        if(temp > n)
            mergeGeneral(n, "input.bin", 1000/n, filename);
        else
            mergeGeneral(temp, "input.bin", 1000/temp, filename);
        temp -= n;
    }
    file_count = 0;
    mergeGeneral(super_runs, "input.bin.super", 1000/super_runs, op_filename);
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
        basicMergesort();
    }
    else if(!strcmp(method, METHODS[1]))
    {
        // multistep
        createRuns(ip_filename);
        multistepMerge(15);
    }
    else if(!strcmp(method, METHODS[2]))
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