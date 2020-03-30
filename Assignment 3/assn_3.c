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

int file_count = 0, total_runs = 0;

char METHODS[3][20] = {"--basic", "--multistep", "--replacement"};
char method[20], ip_filename[20], op_filename[20];

int comparator(const void *first, const void *second)
{
    int l = *(int*)first;
    int r = *(int*)second;
    return (l - r);
}

int fillBuffer(int i, int n, int *ip_buffer, file_record *fr, int file_num)
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
        sprintf(file, "input.bin.%3d", file_count);
        ++file_count;
        strcpy(fr[i].filename, file);
        fr[i].flag = 0;
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
    return 0;
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
        fillBuffer(i, 40, ip_buffer, fr, total_runs);
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
            fillBuffer(temp_index, 40, ip_buffer, fr, total_runs);
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

void merge(int runs, int file_num, int start, char run_file[30],char filename[30])
{
    file_record fr[runs];
    FILE *op;
    int op_buffer[1000], ip_buffer[1000];
    int i, written_runs, op_index, temp_index, min_value;
    int r, size, num;
    char file[30];
    num = start+runs;
    size = 1000/runs;
    // initial setup
    for(i=0; i<runs; ++i)
    {
        sprintf(file, "%s.%03d", run_file, start+i);
        fr[i].read_till = 0;
        fr[i].flag = 0;
        strcpy(fr[i].filename, file);
        fillBuffer(i, size, ip_buffer, fr, num);
    }
    printf("size: %d\n", size);
    printf("initial loaded\n");
    written_runs = 0;    
    op_index = 0;
    while(written_runs < runs)
    {
        temp_index = -1;
        min_value = INT_MAX;

        for(i=0; i<runs; ++i)
        {
            if(fr[i].flag==0 && min_value > ip_buffer[fr[i].index])
            {
                temp_index = i;
                min_value = ip_buffer[fr[i].index];
            }
        }
        if(min_value != INT_MAX)
        {
            op_buffer[op_index++] = min_value;
            ip_buffer[fr[temp_index].index] = INT_MAX;
            fr[temp_index].index += 1;
        }
        if(op_index >= size*runs)
        {
            op = fopen(filename, "ab+");
            fwrite(op_buffer, sizeof(int), size*runs, op);
            fclose(op);
            ++written_runs;
            op_index = 0;
        }
        if(fr[temp_index].index%size == 0)
        {
            r = fillBuffer(temp_index, size, ip_buffer, fr, num);
            if(r == -1)
                break;
        }
    }
    if(op_index != 0)
    {
        op = fopen(filename, "ab+");
        fwrite(op_buffer, sizeof(int), (size_t)op_index, op);
        fclose(op);
    }
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
    for(i=0; i<super_runs; ++i)
    {
        sprintf(filename, "input.bin.super.%03d", i);
        printf("%s\n", filename);
        printf("temp: %d\n", temp);
        if(temp > n)
            merge(n, i, n*i, "input.bin", filename);
        else
            merge(temp, i, n*i, "input.bin",filename);
        temp -= n;
    }
    merge(super_runs, 0, 0, "input.bin.super", op_filename);
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