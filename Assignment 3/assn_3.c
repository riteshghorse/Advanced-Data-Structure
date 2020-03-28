#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<limits.h>

typedef struct file_record{
    char filename[30];    //which file's part is in buffer
    int read_till;  // how much of the file is read
    int index;  // entries of buffer
    int flag;   //if all files are read
}file_record;

int file_count = 0, total_runs = 0;

char METHODS[3][20] = {"--basic", "--multistep", "--replacement"};
char method[20], ip_filename[20], op_filename[20];

int comparator(const void *first, const void *second)
{
    int l = *(int*)first;
    int r = *(int*)second;
    return (l - r);
}

void fillBuffer(int i, int *ip_buffer, file_record *fr)
{
    FILE *fp;
    int j, ind=0;
    char file[30];
    int buff[1];
    // printf("i= %d", i); 
    // printf("%s", fr[i].filename);
    // printf("in fill\n");
    
    strcpy(file, fr[i].filename);
    
    fp = fopen(file, "rb");
    // printf("%s", fr[i].filename);
    fseek(fp, sizeof(int)*fr[i].read_till, SEEK_SET);
    // printf("indexx\n");
    if(feof(fp) && file_count < total_runs)
    {
        fclose(fp);
        sprintf(file, "input.bin.%3d", file_count);
        ++file_count;
        strcpy(fr[i].filename, file);
        fr[i].flag = 0;
        fp = fopen(file, "rb");
    }
    else if(feof(fp) && file_count >= total_runs)
    {
        fr[i].flag = 1;
        fclose(fp);
        return;
    }
    fr[i].index = 40 * i;
    ind = fr[i].index;

    for(j=0; j<40; ++j)
    {
        if(feof(fp))
        {
            fr[i].flag = 1;
            break;
        }
        fread(buff, sizeof(int), 1, fp);
        ip_buffer[ind] = buff[0];
        ++ind;
        fr[i].read_till += 1;
    }
    fclose(fp);
}
void printBuffer(int buffer[1000])
{
    int i;
    for(i=0; i<1000; ++i)
        printf(" %d\n", buffer[i]);
}

void basicMergesort()
{
    // printf("in basic\n");

    // we will do 25-way merge
    file_record fr[25]; 
    char file[30];
    int ip_buffer[1000], op_buffer[1000];
    int i, j, written=0;   
    int op_index=0, temp_index, min_value;
    FILE *op;
    // load 40 keys from each run file
    file_count = 0;
    for(i=0; i<total_runs; ++i)
    {
        sprintf(file, "input.bin.%03d", file_count);
        ++file_count;
        fr[i].read_till = 0;
        fr[i].flag = 0;
        strcpy(fr[i].filename, file);
        fillBuffer(i, ip_buffer, fr);
    }
    
    temp_index = 0;
    while(written < total_runs)
    {
        temp_index = -1;
        min_value = INT_MAX;

        // get the min value assuming first to be the smallest
        // check if file were empty as well
        for(i=0; i<total_runs; ++i)
        {
            if(fr[i].flag==0 && min_value > ip_buffer[fr[i].index])
            {
                temp_index = i;
                min_value = ip_buffer[fr[i].index];
            }
        }
        // make sure that we found min value
        if(min_value != INT_MAX)
        {
            if(min_value == 2146532484)
            {
                printf("temp_index: %d  ", temp_index);
                printf("fr[i].index: %d  ", fr[temp_index].index);
                printf("fr[i].filename: %s  ", fr[temp_index].filename);
                printf("fr[i].flag: %d\n", fr[temp_index].flag);
            }
            op_buffer[op_index++] = min_value;
            ip_buffer[fr[temp_index].index] = INT_MAX;
            fr[temp_index].index += 1;
        }
        // write to file if output buffer is full
        if(op_index >= 1000)
        {
            op = fopen(op_filename, "ab+");
            fwrite(op_buffer, sizeof(int), 1000, op);
            fclose(op);
            ++written;
            op_index = 0;
        }
        // check if the buffer in which min_value was found
        // is full
        if(fr[temp_index].index%40 == 0)
        {
            fillBuffer(temp_index, ip_buffer, fr);
        }
    }
    if(op_index != 0)
    {
        op = fopen(op_filename, "ab+");
        fwrite(op_buffer, sizeof(int), (size_t)op_index, op);
        fclose(op);
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
    // scanf("%s", method);
	// scanf("%s", ip_filename);
	// scanf("%s", op_filename);
    strcpy(method, argv[1]);
    strcpy(ip_filename, argv[2]);
    strcpy(op_filename, argv[3]);

    if(!strcmp(method, METHODS[0]))
    {
        // basic
        createRuns(ip_filename);
        basicMergesort();

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