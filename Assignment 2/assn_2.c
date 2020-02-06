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


index_S *indexes;
avail_S *avail_list;

int indexes_len=0, avail_list_len=0;

char command[4], record[100];
int id;
char commands[][5] = {"add", "find", "del", "end"};


int comparator(const void *first, const void *second)  
{ 
    int l = ((index_S*)first)->key; 
    int r = ((index_S*)second)->key;  
    return (l - r); 
} 

long bin_search(int id, int flag)
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

void add_first_fit(int id, char *record, int len)
{
    if(bin_search(id, 0) != -1)
    {
        printf("\nRecord with SID=%d exists", id);
        return;
    }
    int i;
    long offset;
    FILE *fp;
    fp = fopen("student.db", "r+b");
    fseek(fp, 0, SEEK_END);
    offset = ftell(fp);
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
    fwrite(record, len, 1, fp);
    fclose(fp);
    qsort((void*)indexes, indexes_len, sizeof(index_S), comparator);
}

void del_first_fit(int id)
{
    long offset = bin_search(id, 1);
    int len;
    if(offset == -1)
    {
        printf("\nNo record with SID=%d exists", id);
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
        printf("\nNo record with SID=%d exists", id);
        return;
    }
    FILE *fp = fopen("student.db", "r+b");
    fseek(fp, offset, SEEK_SET);
    fread(&len, sizeof(int), 1, fp);
    buf = (char*)malloc(len);
    fread(buf, len, 1, fp);
    printf("\n%s", buf);
    fclose(fp);
}


void loadIndexes()
{
    FILE *fp;
    long len;
    if((fp = fopen("index.bin", "rb")) != NULL)
    {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp)/sizeof(index_S);
        indexes_len = len * sizeof(index_S);
        indexes = (index_S*)malloc(len*sizeof(index_S));
        fseek(fp, 0, SEEK_SET);
        fread(indexes, sizeof(index_S), len, fp);
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
        len = ftell(fp)/sizeof(avail_S);
        avail_list_len = len * sizeof(avail_S);
        avail_list = (avail_S*)malloc(len*sizeof(avail_S));
        fseek(fp, 0, SEEK_SET);
        fread(avail_list, sizeof(avail_S), len, fp);
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


    loadIndexes();
    loadAvailList();
    
    strcpy(mode, argv[1]);
    // printf("\n%s", mode);
    strcpy(dbname, argv[2]);
    // printf("\n%s", dbname);
    do
    {
        
        if(!strcmp(mode, modes[0]))
        {
            //call to best fit
            // getInfo();
            if(!strcmp(command, commands[3]))
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

        // printf("\nStart\n%s", command);
        // printf("\n%s", id);
        // printf("\n%s\nEnd", record);
    } while (strcmp(command, commands[3]));
    
    return 0;
}