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




void swap(int *H, int i, int j)
{
    int temp;
    temp = H[i];
    H[i] = H[j];
    H[j] = temp;
}


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
        if(fr[i].flag == 0)
            ++files_read;
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
        if(fr[i].flag == 0)
            ++files_read;
        fr[i].flag = 1;
        fr[i].index = -1;
        // ++files_read;
        fclose(fp);
        return -1;
    }
    fr[i].index = n * i;
    ind = fr[i].index;

    for(j=0; j<n; ++j)
    {
        if(fread(buff, sizeof(int), 1, fp) == 0)
        {
            // fr[i].flag = 1;
            // ++files_read;
            // fr[i].index = -1;
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
    int i, written=0, flag, file_num;
    char file[30];
    FILE *op;

    file_num = file_count + runs;
    for(i=0; i<runs; ++i)
    {
        sprintf(file, "%s.%03d", format, file_count);
        ++file_count;
        fr[i].read_till = 0;
        fr[i].flag = 0;
        strcpy(fr[i].filename, file);
        fillBuffer(i, size, ip_buffer, fr, format, file_num);
    }
    op_index = 0;
    written = 0;
    flag = 0;
    
    // files_read = file_count - runs;
    files_read = 0;
    // printf("files_read: %d\n", files_read);
    // printf("files_count: %d\n", file_count);
    op = fopen(op_file, "wb");
    // while(written<runs || files_read<file_count)
    while(files_read < runs)
    {
        // printf("written %d\n", written);
        // printf("files_read %d\n", files_read);
        // printf("--------------------");
        min_value = INT_MAX;
        temp_index = -1;
        flag = 1;
        // find the minimum
        for(i=0; i<runs; ++i)
        {
            if(fr[i].index!=-1 && ip_buffer[fr[i].index]!=0 && min_value > ip_buffer[fr[i].index])
            {
                temp_index = i;
                min_value = ip_buffer[fr[i].index];
            }
        }
        // if(written == 24)
        // {
        //     printf("op %d  ", op_index);
        //     printf("i %d  ", temp_index);
        //     printf("v %d\n", min_value);
        // }
        if(temp_index != -1)
        {
            op_buffer[op_index++] = min_value;
            ip_buffer[fr[temp_index].index] = INT_MAX;
            fr[temp_index].index += 1;
            if((fr[temp_index].index)%size == 0)
            {
                fillBuffer(temp_index, size, ip_buffer, fr, format, file_num);
            }
        }

        if(op_index >= 1000)
        {
            ++written;
            // printf("in op_index: %d\n", op_index);
            fwrite(op_buffer, sizeof(int), 1000, op);
            op_index = 0;
        }
        // for(i=0; i<runs; ++i)
        // {
        //     if(fr[i].flag==1 && fr[i].index%size != 0)
        //     {
        //         flag = 0;
        //         break;
        //     }
        // }
    }
    if(op_index != 0)
    {
        fwrite(op_buffer, sizeof(int), op_index, op);
    } 
    // printf("op_index: %d", op_index);
    for(i=0; i<1000; ++i)
        if(ip_buffer[i] != INT_MAX && ip_buffer[i]!=0)
            printf("%d\n", i);
    for(i=0; i<runs; ++i)
        printf("index %d\n", fr[i].index);
    fclose(op);
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
    printf("total runs: %d\n", total_runs);
    printf("super runs: %d\n", super_runs);
    file_count = 0;
    files_read = 0;
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

void heapify(int *H, int i, int n)
{
    int j, k, temp;
    temp = i;
    j = 2*i + 1;
    k = j + 1; 
    
    if(j < n && H[j] < H[i])
        temp = j;
    if(k < n && H[k] < H[temp])
        temp = k;
    if(temp != i) 
    {
        swap(H, i, temp);
        heapify(H, temp, n);
    }
}


void heapsort(int *H, int h_size)
{
    int i;
    i = h_size / 2;
    while(i >= 0)
    {
        heapify(H, i, h_size);
        --i;
    }
}


void replacement_selection()
{
    int H[750], B[250], op_buffer[1000];
    int h_size = 0, op_index, bi, blen, s_heap;
    int i, read;
    char file[30];
    FILE *fp, *op;

    fp = fopen(ip_filename, "rb");
    
    // read 750 values into primary heap
    h_size = fread(H, sizeof(int), 750, fp);
    blen = fread(B, sizeof(int), 250, fp);
    // transform the array to heap
    heapsort(H, h_size);
    // printf("heap %d\n", h_size);
    file_count = 0;
    op_index = 0;
    bi = 0;
    s_heap = 0;
    read = 0;
    sprintf(file, "input.bin.%03d", file_count);
    ++file_count;
    op = fopen(file, "wb");
    while(1)
    {
        if(bi != blen)
        {
            if(h_size > 0)
            {
                op_buffer[op_index++] = H[0];
                if(H[0] <= B[bi])
                    H[0] = B[bi];
                else
                {
                    H[0] = H[h_size-1];
                    H[h_size-1] = B[bi];
                    --h_size;
                }
                ++bi;
                heapsort(H, h_size);
                if(bi == blen)
                {
                    bi = 0;
                    blen = fread(B, sizeof(int), 250, fp);
                }
                if(op_index == 1000)
                {
                    fwrite(op_buffer, sizeof(int), 1000, op);
                    op_index = 0;
                }
            }
            else
            {
                if(op_index > 0)
                    fwrite(op_buffer, sizeof(int), op_index, op);
                fclose(op);
                op_index = 0;
                sprintf(file, "input.bin.%03d", file_count);
                ++file_count;
                op = fopen(file, "wb");
                h_size = 750;
                heapsort(H, h_size);
            }
        }
        else
        {
            if(h_size > 0)
            {
                op_buffer[op_index++] = H[0];
                ++s_heap;
                H[0] = H[h_size-1];
                --h_size;
                heapsort(H, h_size);
                if(op_index == 1000)
                {
                    fwrite(op_buffer, sizeof(int), 1000, op);
                    op_index = 0;
                }
                
            }
            else
            {
                if(op_index > 0)
                    fwrite(op_buffer, sizeof(int), op_index, op);
                fclose(op);
                op_index = 0;
                sprintf(file, "input.bin.%03d", file_count);
                ++file_count;
                op = fopen(file, "wb");
                qsort(&H[s_heap], 750-s_heap, sizeof(int), comparator);
                fwrite(&H[s_heap], sizeof(int), 750-s_heap, op);
                fclose(op);
                break;
            }
        }
    }
    fclose(fp);
    total_runs = file_count;
    file_count = 0;
    files_read = 0;
    mergeGeneral(total_runs, "input.bin", 1000/total_runs, op_filename);
}


void printArray(int a[])
{
    int i;
    for(i=0; i<6; ++i)
        printf("%d ", a[i]);
    printf("\n");
}

int main(int argc, char *argv[])
{
    int a[] = {5,4,3,2,1,-1};

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
        replacement_selection();
        // printArray(a);
        // heapsort(a, 6);
        // printArray(a);
    }
    else
    {
        printf("Pass a valid method\n");
    }


    // for(i=0; i<argc; ++i)
    //     printf("%s\n", argv[i]);    
    return 0;
}