#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int nkeys = 0;
int nseeks = 0;
int length = 0;

int get_length(char *data)
{
    FILE *fp;
    int i;
    int buffer[2];
    fp = fopen(data, "rb");
    
    i=0;
    while (fread(buffer, 1, sizeof(int), fp))
    {   
        ++i;
    }
    return i;
}

int* read_file_to_int_array(char *data)
{
    int i, index, len = get_length(data);
    int *p = (int*)malloc(len*sizeof(int));
    length = len;

    FILE *fp;
    fp = fopen(data, "rb");
    int buffer[2];
    index=0;
    while (fread(buffer, 1, sizeof(int), fp))
    {   
        p[index++] = buffer[0];
    }
    // printf("%d", i);
    fclose(fp);
    
    return p;
}

void mem_lin_search(char *key_file, char *seek_file)
{
    int *s, *k, *hit;
    int i;  
    k = read_file_to_int_array(key_file);
    nkeys = length;
    s = read_file_to_int_array(seek_file);
    nseeks = length;
    hit = (int*)malloc(nseeks*sizeof(int));



    // printf("\n%d\n",nseeks);
    // for(i=0; i<nseeks; ++i)
    //     printf("%d ", s[i]);
}

void mem_bin_search(char *key_file, char *seek_file)
{
    int *s, *k, *hit;
    int i;  
    k = read_file_to_int_array(key_file);
    nkeys = length;
    s = read_file_to_int_array(seek_file);
    nseeks = length;
    hit = (int*)malloc(nseeks*sizeof(int));

}

void disk_lin_search(char *key_file, char *seek_file)
{
    int *s, *hit;
    int i;  
    s = read_file_to_int_array(seek_file);
    nseeks = length;
    hit = (int*)malloc(nseeks*sizeof(int));
}

void disk_lin_search(char *key_file, char *seek_file)
{
    int *s, *hit;
    int i;  
    s = read_file_to_int_array(seek_file);
    nseeks = length;
    hit = (int*)malloc(nseeks*sizeof(int));
}

int main(int argc, char *argv[])
{
    int i;
    char operations[][11]={"--mem-lin", "--mem-bin", "--disk-lin", "--disk-bin"};
    char key_file[15], seek_file[15], f_name[15];

    strcpy(f_name, argv[1]);
    // printf("in first ");
    strcpy(key_file, argv[2]);
    strcpy(seek_file, argv[3]);

    if(strcmp(f_name, operations[0]) == 0)
    {
        // call to perform in memory linear search
        // printf("in first if");
        mem_lin_search(key_file, seek_file);
    }
    if(strcmp(f_name, operations[1]) == 0)
    {
        // call to perform in memory binary search
        mem_bin_search(key_file, seek_file);
    }
    if(strcmp(f_name, operations[2]) == 0)
    {
        // call to perform disk based linear search
        disk_lin_search(key_file, seek_file);
    }
    if(strcmp(f_name, operations[3]) == 0)
    {
        // call to perform disk based binary search
        disk_bin_search(key_file, seek_file);
    }    
    return 0;
}

 // for(i=0; i<4; ++i)
    // {
    //     printf("\n%s", operations[i]);
    // }
