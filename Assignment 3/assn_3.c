#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<limits.h>
#include<math.h>
#define BUFF_SIZE 1000



typedef struct file_record{
    FILE *fp;
    int index;
}file_record;

int total_runs;
char METHODS[3][20] = {"--basic", "--multistep", "--replacement"};
char method[20], ip_filename[20], op_filename[20];


int comparator(const void *first, const void *second)
{
    int l = *(int*)first;
    int r = *(int*)second;
    return (l - r);
}

void swap(int *H, int i, int j)
{
    int temp;
    temp = H[i];
    H[i] = H[j];
    H[j] = temp;
}

void printArray(int a[])
{
    int i;
    for(i=0; i<6; ++i)
        printf("%d ", a[i]);
    printf("\n");
}

void heapify(int *H, int i, int n)
{
    int j, k, temp, c;
    temp = i;
    j = 2*i + 1;
    k = j + 1; 
    
    if(j < n && H[j] >= H[temp])
        temp = j;
    if(k < n && H[k] >= H[temp])
        temp = k;
    if(temp != i) 
    {
        c = H[i];
        H[i] = H[temp];
        H[temp] = c;
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
    for(i=h_size-1; i>=0; --i)
    {
        swap(H, 0, i);
        heapify(H, 0, i);
    }
}
void createRuns(char *filename)
{
    int buffer[BUFF_SIZE], c;
    char file[20];
    FILE *ip, *op;

    ip = fopen(filename, "rb");
    c = fread(buffer, sizeof(int), BUFF_SIZE, ip);
    while(c != 0)
    {
        qsort(buffer, c, sizeof(int), comparator);
        sprintf(file, "input.bin.%03d", total_runs++);
        op = fopen(file, "wb");
        fwrite(buffer, sizeof(int), c, op);
        fclose(op);
        c = fread(buffer, sizeof(int), BUFF_SIZE, ip);
    }
    fclose(ip);
}

void mergeRuns(int runs, int run_size, char *run_format, char op_file[20], int file_num) 
{
    int ip_buffer[BUFF_SIZE], op_buffer[BUFF_SIZE];
    int i, min_index, min_val, op_index;
    int file_read;
    char file[25];
    file_record fr[runs];
    FILE *op;

    op = fopen(op_file, "wb");

    //load initial buffer
    for(i=0; i<runs; ++i)
    {
        sprintf(file, "%s.%03d", run_format, file_num++);
        fr[i].fp = fopen(file, "rb");
        fr[i].index = run_size * i;
        fread(&(ip_buffer[fr[i].index]), sizeof(int), run_size, fr[i].fp);
    }
    // printf("66\n");
    // printf("total runs %d\n", total_runs);
    file_read = 0;  op_index = 0;
    while (file_read < total_runs)
    {
        min_index = -1; min_val = INT_MAX;
        for(i=0; i<runs; ++i)
        {
            if(ip_buffer[fr[i].index]==0 || ip_buffer[fr[i].index]==INT_MAX)
            {
                if(fr[i].fp)
                {
                    if(feof(fr[i].fp))
                    {
                        // printf("78\n");
                        ++file_read;
                        fr[i].fp = NULL;
                        // fclose(fr[i].fp);
                        continue;
                    }
                }   
            }
            else if(min_val > ip_buffer[fr[i].index])
            {
                min_val =  ip_buffer[fr[i].index];
                min_index = i;
            }
        }
        // printf("81\n");
        if(min_index != -1)
        {
            op_buffer[op_index++] = min_val;
            ip_buffer[fr[min_index].index] = INT_MAX;
            fr[min_index].index += 1;
            // fr[min_index].index = (fr[min_index].index + 1)%run_size;
            if(fr[min_index].index%run_size == 0)
            {
                fr[min_index].index = min_index * run_size;
                if(fr[min_index].fp)
                {
                    if(feof(fr[min_index].fp))
                    {
                        // printf("78\n");
                        ++file_read;
                        fr[i].fp = NULL;
                        // fclose(fr[min_index].fp);
                    }
                    else
                    {
                        fread(&(ip_buffer[fr[min_index].index]), sizeof(int), run_size, fr[min_index].fp);
                    }
                }    
            }
            if(op_index == BUFF_SIZE)
            {
                printf("101\n");
                fwrite(op_buffer, sizeof(int), BUFF_SIZE, op);
                op_index = 0;
            }
        }
        else 
            break;
    
    }
    if(op_index != 0)
        fwrite(op_buffer, sizeof(int), op_index, op);

    fclose(op);
}

void basicMergesort()
{
    int run_size;
    
    run_size = BUFF_SIZE / total_runs;
    mergeRuns(total_runs, run_size, "input.bin", op_filename, 0);
}

void multistepMerge()
{
    int i, n, super_runs, temp, file_num;
    char file[25];
    n = 15;  // make super runs of 15
    temp = total_runs;
    super_runs = 0;
    while(temp > 0)
    {
        ++super_runs;
        temp -= n;
    }
    temp = total_runs;
    file_num = 0;
    for(i=0; i<super_runs; ++i)
    {
        sprintf(file, "input.bin.super.%03d", i);
        if(temp > n)
            mergeRuns(n, BUFF_SIZE/n, "input.bin", file, file_num);
        else
            mergeRuns(temp, BUFF_SIZE/temp, "input.bin", file, file_num);
        temp -= n;
        file_num += n;
    }
    mergeRuns(super_runs, BUFF_SIZE/super_runs, "input.bin.super", op_filename, 0);
}

void replacementSelection()
{
    int H[750], B[250], op_buffer[1000];
    int h_size, blen, op_index, bi, s_heap, read;
    char file[25];
    FILE *fp, *op;
    fp = fopen(ip_filename, "rb");
    
    // read 750 values into primary heap
    h_size = fread(H, sizeof(int), 750, fp);
    blen = fread(B, sizeof(int), 250, fp);
    // transform the array to heap
    heapsort(H, h_size);
    // printf("heap %d\n", h_size);
    total_runs = 0;
    op_index = 0;
    bi = 0;
    s_heap = 0;
    read = 0;
    sprintf(file, "input.bin.%03d", total_runs);
    ++total_runs;
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
                sprintf(file, "input.bin.%03d", total_runs);
                ++total_runs;
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
                if(op_index == BUFF_SIZE)
                {
                    fwrite(op_buffer, sizeof(int), BUFF_SIZE, op);
                    op_index = 0;
                }
                
            }
            else
            {
                if(op_index > 0)
                    fwrite(op_buffer, sizeof(int), op_index, op);
                fclose(op);
                op_index = 0;
                sprintf(file, "input.bin.%03d", total_runs);
                ++total_runs;
                op = fopen(file, "wb");
                qsort(&H[s_heap], 750-s_heap, sizeof(int), comparator);
                fwrite(&H[s_heap], sizeof(int), 750-s_heap, op);
                fclose(op);
                break;
            }
        }
    }
    fclose(fp);
    mergeRuns(total_runs, BUFF_SIZE/total_runs, "input.bin", op_filename, 0);
}

int main(int argc, char *argv[])
{
    int a[] = {6,5,4,3,2,1};
    strcpy(method, argv[1]);
    strcpy(ip_filename, argv[2]);
    strcpy(op_filename, argv[3]);

    if(!strcmp(method, METHODS[0]))
    {
        // basic
        total_runs = 0;
        createRuns(ip_filename);
        basicMergesort();
    }
    else if(!strcmp(method, METHODS[1]))
    {
        // multistep
        total_runs = 0;
        createRuns(ip_filename);
        multistepMerge();
    }
    else if(!strcmp(method, METHODS[2]))
    {
        // replacement
        replacementSelection();
        // printArray(a);
        // heapsort(a, 6);
        // printArray(a);
    }
    else
    {
        printf("Pass a valid method\n");
    }
    return 0;
}