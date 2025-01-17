#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/time.h>
#define START_TIME gettimeofday( &start, NULL )
#define END_TIME gettimeofday( &end, NULL )

int nkeys = 0;
int nseeks = 0;


struct timeval start, end;


void PRINT_DIFF()
{
    long s, us;
    s = end.tv_sec - start.tv_sec;
    us = end.tv_usec - start.tv_usec;
    if(us < 0)
    {
        us = 1000000 - (-us);
        s -= 1;
    }
    printf( "Time: %ld.%06ld\n", s, us);
}

int get_length(char*data) 
{
    FILE *fp;
    int len;

    fp = fopen(data, "rb");
    fseek(fp, 0, SEEK_END);
    len = ftell(fp)/sizeof(int);
    fclose(fp);
    return len;
}

int* read_file_to_int_array(char *data, int size)
{
    int i, index, len = size;
    int *p = (int*)malloc(len*sizeof(int));
    FILE *fp;

    fp = fopen(data, "rb");
    int buffer[2];
    index=0;

    while (fread(buffer, 1, sizeof(int), fp))
        p[index++] = buffer[0];

    fclose(fp);
    return p;
}

void mem_lin_search(char *key_file, char *seek_file)
{
    int *s, *k, *hit;
    int i, j;  

    // step 1: open and read seek file into array s
    nseeks = get_length(seek_file);
    s = read_file_to_int_array(seek_file, nseeks);
    
    // hit: used to record if seek element si exists in k
    hit = (int*)malloc(nseeks*sizeof(int));

    START_TIME;
    
    //step 2: open and read key file into array k
    nkeys = get_length(key_file);
    k = read_file_to_int_array(key_file, nkeys);
    
    // step 3: for each element in s perform sequential search on k
    for(i=0; i<nseeks; ++i)
    {
        hit[i] = 0;
        for(j=0; j<nkeys; ++j)
        {
            if(s[i] == k[j])
            {
                hit[i] = 1;
                break;
            }
        }
        if(hit[i] == 1)
            printf( "%12d: Yes\n", s[i] ); 
        else
            printf( "%12d: No\n", s[i] ); 
    }
    END_TIME;
    PRINT_DIFF();
}

void mem_bin_search(char *key_file, char *seek_file)
{
    int *s, *k, *hit;
    int i, low, mid, high;

    // step 1: open and read seek file into array s
    nseeks = get_length(seek_file);
    s = read_file_to_int_array(seek_file, nseeks);
    
    // hit: used to record if seek element si exists in k
    hit = (int*)malloc(nseeks*sizeof(int));

    START_TIME;
    
    //step 2: open and read key file into array k
    nkeys = get_length(key_file);
    k = read_file_to_int_array(key_file, nkeys);
    
    // step 3: for each element in s perform binary search on k
    for(i=0; i<nseeks; ++i)
    {
        low = 0;
        high = nkeys - 1;
        hit[i] = 0;
        while(low <= high)
        {
            mid = (low+high)/2;
            if(s[i] == k[mid])
            {
                hit[i] = 1;
                break;
            }
            else if(s[i] < k[mid])
                high = mid-1;
            else
                low = mid + 1;
        }
        if(hit[i] == 1)
            printf( "%12d: Yes\n", s[i] ); 
        else
            printf( "%12d: No\n", s[i] ); 
    }
    END_TIME;
    PRINT_DIFF();
}

void disk_lin_search(char *key_file, char *seek_file)
{
    int *s, *hit;
    int i;  
    int buffer;

    FILE *fp;

    // step 1: open and read seek file into array s
    nseeks = get_length(seek_file);
    s = read_file_to_int_array(seek_file, nseeks);
    
    // hit: used to record if seek element si exists in k
    hit = (int*)malloc(nseeks*sizeof(int));
    
    fp = fopen(key_file, "rb");
    
    START_TIME;

    for(i=0; i<nseeks; ++i)
    {
        // move seek to first location when searching for each s[i]
        fseek(fp, 0, SEEK_SET);
        hit[i] = 0;

        // keep reading file until the key is found or EOF is reached
        while (fread(&buffer, sizeof(int), 1, fp))
        {   
            if(buffer == s[i])
            {
                hit[i] = 1;
                break;
            }
        }
        if(hit[i] == 1)
            printf( "%12d: Yes\n", s[i] ); 
        else
            printf( "%12d: No\n", s[i] ); 
    }
    END_TIME;
    PRINT_DIFF();
    fclose(fp);
}

void disk_bin_search(char *key_file, char *seek_file)
{
    int *s, *hit;
    size_t low, mid, high;  
    int i, buffer;

    FILE *fp;

    // step 1: open and read seek file into array s
    nseeks = get_length(seek_file);
    s = read_file_to_int_array(seek_file, nseeks);
    
    // hit: used to record if seek element si exists in k
    hit = (int*)malloc(nseeks*sizeof(int));

    fp = fopen(key_file, "rb");

    START_TIME;
   
    for(i=0; i<nseeks; ++i)
    {
        // move seek to first location when searching for each s[i]
        fseek(fp, -sizeof(int), SEEK_END);
        high = (ftell(fp))/sizeof(int);
        fseek(fp, 0, SEEK_SET);
        low = (ftell(fp))/sizeof(int);

        hit[i] = 0;

        // keep reading file until the key is found or EOF is reached
        while (low <= high)
        {   
            mid = (low+high)/2;
            fseek(fp, mid*sizeof(int), SEEK_SET);
            fread(&buffer, sizeof(int), 1, fp);
            if(buffer == s[i])
            {
                hit[i] = 1;
                break;
            }
            else if(buffer > s[i])
                high = mid - 1;
            else
                low = mid + 1;
        }
        if(hit[i] == 1)
            printf( "%12d: Yes\n", s[i] ); 
        else
            printf( "%12d: No\n", s[i] ); 
    }
    END_TIME;
    PRINT_DIFF();
    fclose(fp);
}

int main(int argc, char *argv[])
{
    int i;
    char operations[][11]={"--mem-lin", "--mem-bin", "--disk-lin", "--disk-bin"};
    char key_file[15], seek_file[15], f_name[15];

    // store the filenames and operation to be performed
    strcpy(f_name, argv[1]);
    strcpy(key_file, argv[2]);
    strcpy(seek_file, argv[3]);

    if(strcmp(f_name, operations[0]) == 0)
    {
        // call to perform in memory linear search
        mem_lin_search(key_file, seek_file);
    }
    else if(strcmp(f_name, operations[1]) == 0)
    {
        // call to perform in memory binary search
        mem_bin_search(key_file, seek_file);
    }
    else if(strcmp(f_name, operations[2]) == 0)
    {
        // call to perform disk based linear search
        disk_lin_search(key_file, seek_file);
    }
    else if(strcmp(f_name, operations[3]) == 0)
    {
        // call to perform disk based binary search
        disk_bin_search(key_file, seek_file);
    }
    else
    {
        printf("\nWrong Input.\nPlease specify one of following operations: --mem-lin, --mem-bin, --disk-lin, --disk-bin\n");
    }
        
    return 0;
}