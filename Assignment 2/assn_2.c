#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct{ 
    int key; /* Record's key */ 
    long off; /* Record's offset in file */ 
}index_S;


typedef struct{ 
    int siz; /* Hole's size */ 
    long off; /* Hole's offset in file */ 
}avail_S;


index_S *indexes=NULL;
avail_S *avail_list=NULL;

long indexes_len=0, avail_list_len=0;

char command[4], record[100];
int id;
char commands[][5] = {"add", "find", "del", "end"};


int listComparator(const void *first, const void *second)  
{ 
    int l = ((avail_S*)first)->siz; 
    int r = ((avail_S*)second)->siz;
    if(l == r)
        return ( ((avail_S*)first)->off - ((avail_S*)second)->off);  
    return (l - r); 
}

int indexComparator(const void *first, const void *second)  
{ 
    int l = ((index_S*)first)->key; 
    int r = ((index_S*)second)->key;  
    return (l - r); 
} 

long bin_search(int id, int flag)   //flag is used for delete
{
    int low, mid, high;
    low = 0; high = indexes_len - 1;
    while(low <= high)
    {
        mid = (low+high)/2;
        if(indexes[mid].key == id)
        {
            if(flag == 1)
                indexes[mid].key = -1;
            return indexes[mid].off;
        }
        else if(indexes[mid].key > id)
            high = mid - 1;
        else
            low = mid + 1;
    }
    return -1;
}

void add_best_fit(int id, char *record, int length)
{
    if(bin_search(id, 0) != -1)
    {
        printf("Record with SID=%d exists\n", id);
        return;
    }
    int i, len;
    long offset;
    FILE *fp;

    qsort((void*)avail_list, avail_list_len, sizeof(avail_S), listComparator);
    fp = fopen("student.db", "r+b");
    fseek(fp, 0, SEEK_END);
    offset = ftell(fp);
    len = length*sizeof(record[0]);
    for(i=0; i<avail_list_len; ++i)
    {
        if(avail_list[i].siz >= len)
        {
            offset = avail_list[i].off;
            if((avail_list[i].siz - len) > 0)
            {
                avail_list_len += 1;
                avail_list = (avail_S*)realloc(avail_list, avail_list_len*sizeof(avail_S));
                avail_list[avail_list_len-1].off = avail_list[i].off + len + sizeof(int); //record and size
                avail_list[avail_list_len-1].siz = avail_list[i].siz - len - sizeof(int);
            }
            avail_list[i].siz = 0;
            break;
        }
    }
    indexes_len += 1;
    indexes = (index_S*)realloc(indexes, indexes_len*sizeof(index_S));
    indexes[indexes_len-1].key = id;
    indexes[indexes_len-1].off = offset; 
    fseek(fp, offset, SEEK_SET);
    fwrite(&len, sizeof(int), 1, fp);
    fwrite(record, sizeof(record[0]), length, fp);
    fclose(fp);
    qsort((void*)indexes, indexes_len, sizeof(index_S), indexComparator);
}



void add_first_fit(int id, char *record, int length)
{
    if(bin_search(id, 0) != -1)
    {
        printf("Record with SID=%d exists\n", id);
        return;
    }
    int i, len;
    long offset;
    FILE *fp;
    fp = fopen("student.db", "r+b");
    fseek(fp, 0, SEEK_END);
    offset = ftell(fp);
    len = length*sizeof(record[0]);
    for(i=0; i<avail_list_len; ++i)
    {
        if(avail_list[i].siz >= len)
        {
            offset = avail_list[i].off;
            avail_list_len += 1;
            avail_list = (avail_S*)realloc(avail_list, avail_list_len*sizeof(avail_S));
            avail_list[avail_list_len-1].off = avail_list[i].off + len + sizeof(int); //record and size
            avail_list[avail_list_len-1].siz = avail_list[i].siz - len - sizeof(int);
            avail_list[i].siz = 0;
            break;
        }
    }
    indexes_len += 1;
    indexes = (index_S*)realloc(indexes, indexes_len*sizeof(index_S));
    indexes[indexes_len-1].key = id;
    indexes[indexes_len-1].off = offset; 
    fseek(fp, offset, SEEK_SET);
    fwrite(&len, sizeof(int), 1, fp);
    fwrite(record, sizeof(record[0]), length, fp);
    fclose(fp);
    qsort((void*)indexes, indexes_len, sizeof(index_S), indexComparator);
}

void del_first_fit(int id)
{
    long offset = bin_search(id, 1);
    int len;
    if(offset == -1)
    {
        printf("No record with SID=%d exists\n", id);
        return;
    }
    FILE *fp = fopen("student.db", "r+b");
    fseek(fp, offset, SEEK_SET);
    fread(&len, sizeof(int), 1, fp);
    avail_list_len += 1;
    avail_list = (avail_S*)realloc(avail_list, avail_list_len*sizeof(avail_S));
    avail_list[avail_list_len-1].off = offset; //record and size
    avail_list[avail_list_len-1].siz = len + sizeof(int);
}

void find(int id)
{
    long offset = bin_search(id, 0);
    char *buf;
    int len;
    if(offset == -1)
    {
        printf("No record with SID=%d exists\n", id);
        return;
    }
    FILE *fp = fopen("student.db", "r+b");
    fseek(fp, offset, SEEK_SET);
    fread(&len, sizeof(int), 1, fp);
    buf = (char*)malloc(len);
    fread(buf, len, 1, fp);
    printf("%s\n", buf);
    fclose(fp);
}


void loadIndexes()
{
    FILE *fp;
    long len;
    if((fp = fopen("index.bin", "rb")) != NULL)
    {
        fseek(fp, 0, SEEK_END);
        indexes_len = ftell(fp)/sizeof(index_S);
        len = indexes_len * sizeof(index_S);
        indexes = (index_S*)realloc(indexes, len);
        fseek(fp, 0, SEEK_SET);
        fread(indexes, sizeof(index_S), indexes_len, fp);
        fclose(fp);
    }
}

void loadAvailList()
{
    FILE *fp;
    long len;
    if((fp = fopen("list.bin", "rb")) != NULL)
    {
        fseek(fp, 0, SEEK_END);
        avail_list_len = ftell(fp)/sizeof(avail_S);
        len = avail_list_len * sizeof(avail_S);
        avail_list = (avail_S*)realloc(avail_list, len);
        fseek(fp, 0, SEEK_SET);
        fread(avail_list, sizeof(avail_S), avail_list_len, fp);
        fclose(fp);
    }
}

int main(int argc, char* argv[])
{
    // 3 arguments to be passed
    // argv[1] -> operation to be performed
    // argv[2] -> name of student db file
    FILE *db;
    char dbname[20], mode[20];
    char modes[][15] = {"--best-fit", "--worst-fit", "--first-fit"};
    int i, hole_n=0, hole_siz=0;

    loadIndexes();
    loadAvailList();
    if((db=fopen("student.db", "r+b")) ==  NULL)
        db = fopen("student.db", "wb");
    fclose(db);
    strcpy(mode, argv[1]);
    // printf("%s\n", mode);
    strcpy(dbname, argv[2]);
    // printf("%s\n", dbname);
    do
    {  
        if(!strcmp(mode, modes[0]))
        {
            //call to best fit
            scanf("%s", command);
            if(!strcmp(command, commands[0]))
            {
                scanf("%d", &id);
                scanf("%s", record);
                add_best_fit(id, record, strlen(record));
            }
            else if(!strcmp(command, commands[1]))
            {
                scanf("%d", &id);
                find(id);
            }
            else if(!strcmp(command, commands[2]))
            {
                scanf("%d", &id);
                del_first_fit(id);
            }
            else
                break; 
        }
        else if (!strcmp(mode, modes[1]))
        {
            // call to worst fit
            // getInfo();
            if(!strcmp(command, commands[3]))
                break;
        }
        else if (!strcmp(mode, modes[2]))
        {
            // call to first fit
            scanf("%s", command);
            if(!strcmp(command, commands[0]))
            {
                scanf("%d", &id);
                scanf("%s", record);
                add_first_fit(id, record, strlen(record));
            }
            else if(!strcmp(command, commands[1]))
            {
                scanf("%d", &id);
                find(id);
            }
            else if(!strcmp(command, commands[2]))
            {
                scanf("%d", &id);
                del_first_fit(id);
            }
            else
                break;    
        }
        else
        {
            /* code */
        }
        
        // printf("Start\n%s\n", command);
        // printf("%s\n", id);
        // printf("%s\nEnd\n", record);
    } while (strcmp(command, commands[3]));
    printf("Index:\n");
    for(i=0; i<indexes_len; ++i)
    {
        if(indexes[i].key != -1)
        {
            printf( "key=%d: offset=%ld\n", indexes[i].key, indexes[i].off );
        }
    }
    printf("Availability:\n");
    for(i=0; i<avail_list_len; ++i)
    {
        if(avail_list[i].siz != 0)
        {
            ++hole_n;
            hole_siz += avail_list[i].siz;
            printf( "size=%d: offset=%ld\n", avail_list[i].siz, avail_list[i].off );
        }
    }    
    printf( "Number of holes: %d\n", hole_n );
    printf( "Hole space: %d\n", hole_siz );
    db = fopen( "index.bin", "wb" ); 
    fwrite( indexes, sizeof( index_S ), indexes_len, db ); 
    fclose( db ); 
    db = fopen( "list.bin", "wb" ); 
    fwrite( avail_list, sizeof( avail_S ), avail_list_len, db ); 
    fclose( db ); 
    return 0;
}