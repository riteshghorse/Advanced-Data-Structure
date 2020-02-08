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

int listComparatorDec(const void *first, const void *second)  
{ 
    int l = ((avail_S*)first)->siz; 
    int r = ((avail_S*)second)->siz;
    if(l == r)
        return ( ((avail_S*)first)->off - ((avail_S*)second)->off);  
    return -1*(l - r); 
}

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

void loadIndexes()  //loads the indexes
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

void loadAvailList()    // loads the availability list
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

long bin_search(int id, int flag)   //flag is used for delete
{
    /*
        return: offset if record found
                -1 if not found
    */
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

void add(char dbname[20], int id, char *record, int length)
{
    if(bin_search(id, 0) != -1) // search for the id if it exists
    {
        printf("Record with SID=%d exists\n", id);
        return;
    }

    int i, len;
    long offset;
    FILE *fp;

    fp = fopen(dbname, "r+b");
    fseek(fp, 0, SEEK_END);
    offset = ftell(fp);
    len = length*sizeof(record[0]);
    // search the availability list if any hole exists that accomodate the record
    for(i=0; i<avail_list_len; ++i)
    {
        if(avail_list[i].siz >= len)
        {
            offset = avail_list[i].off;
            // append a new hole with leftover part after inserting the record
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
    // create an entry in the indexes array for new record to be inserted with offset
    indexes_len += 1;
    indexes = (index_S*)realloc(indexes, indexes_len*sizeof(index_S));
    indexes[indexes_len-1].key = id;
    indexes[indexes_len-1].off = offset; 

    fseek(fp, offset, SEEK_SET);
    fwrite(&len, sizeof(int), 1, fp);
    fwrite(record, sizeof(record[0]), length, fp);
    fclose(fp);
    // sort the indexes to maintain the primary keys in ascending order
    qsort((void*)indexes, indexes_len, sizeof(index_S), indexComparator);
}

void del(char dbname[20], int id)
{
    long offset = bin_search(id, 1);    // search for the id
    int len;
    if(offset == -1)     //id doesn't exist
    {
        printf("No record with SID=%d exists\n", id);
        return;
    }
    FILE *fp = fopen(dbname, "r+b");
    fseek(fp, offset, SEEK_SET);
    fread(&len, sizeof(int), 1, fp);
    avail_list_len += 1;
    avail_list = (avail_S*)realloc(avail_list, avail_list_len*sizeof(avail_S));
    // record the offset returned by bin_search (i.e. the offset of record deleted)
    avail_list[avail_list_len-1].off = offset; 
    avail_list[avail_list_len-1].siz = len + sizeof(int);
}

void find(char dbname[20], int id)
{
    long offset = bin_search(id, 0);    // search for the id
    char *buf;
    int len;
    if(offset == -1)    //id doesn't exist
    {
        printf("No record with SID=%d exists\n", id);
        return;
    }
    FILE *fp = fopen(dbname, "r+b");
    fseek(fp, offset, SEEK_SET);
    // read the length of record first
    fread(&len, sizeof(int), 1, fp);    
    buf = (char*)malloc(len);
    // read the record
    fread(buf, len, 1, fp);
    printf("%s\n", buf);
    fclose(fp);
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

    //load indexes in index_S array from the index file
    loadIndexes();  
    //load availability list in avail_S array from the list file
    loadAvailList();

    //get the mode which we want to operate in
    strcpy(mode, argv[1]);
    strcpy(dbname, argv[2]);

    //create a student.db file if it doesn't already exist
    if((db=fopen(dbname, "r+b")) ==  NULL)
        db = fopen(dbname, "wb");
    fclose(db);

    

    do
    {  
        if(!strcmp(mode, modes[0])) //call to best fit
        {
            scanf("%s", command);
            if(!strcmp(command, commands[0]))
            {
                scanf("%d", &id);
                scanf("%s", record);
                //sort the availability list in ascending order of the hole size
                qsort((void*)avail_list, avail_list_len, sizeof(avail_S), listComparator);
                add(dbname, id, record, strlen(record));
            }
            else if(!strcmp(command, commands[1]))
            {
                scanf("%d", &id);
                find(dbname, id);
            }
            else if(!strcmp(command, commands[2]))
            {
                scanf("%d", &id);
                del(dbname, id);
                //sort the availability list in ascending order of the hole size
                qsort((void*)avail_list, avail_list_len, sizeof(avail_S), listComparator);
            }
            else
                break; 
        }
        else if (!strcmp(mode, modes[1]))    // call to worst fit
        {
            scanf("%s", command);
            if(!strcmp(command, commands[0]))
            {
                scanf("%d", &id);
                scanf("%s", record);
                //sort the availability list in descending order of the hole size
                qsort((void*)avail_list, avail_list_len, sizeof(avail_S), listComparatorDec);
                add(dbname, id, record, strlen(record));
            }
            else if(!strcmp(command, commands[1]))
            {
                scanf("%d", &id);
                find(dbname, id);
            }
            else if(!strcmp(command, commands[2]))
            {
                scanf("%d", &id);
                del(dbname, id);
                //sort the availability list in descending order of the hole size
                qsort((void*)avail_list, avail_list_len, sizeof(avail_S), listComparatorDec);
            }
            else
                break; 
        }
        else if (!strcmp(mode, modes[2]))   // call to first fit
        {
            scanf("%s", command);
            if(!strcmp(command, commands[0]))
            {
                scanf("%d", &id);
                scanf("%s", record);
                add(dbname, id, record, strlen(record));
            }
            else if(!strcmp(command, commands[1]))
            {
                scanf("%d", &id);
                find(dbname, id);
            }
            else if(!strcmp(command, commands[2]))
            {
                scanf("%d", &id);
                del(dbname, id);
            }
            else
                break;    
        }
        else
        {
            break;
        }
        
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

    // write the indexes and availability file to the disk
    db = fopen( "index.bin", "wb" ); 
    fwrite( indexes, sizeof( index_S ), indexes_len, db ); 
    fclose( db ); 
    db = fopen( "list.bin", "wb" ); 
    fwrite( avail_list, sizeof( avail_S ), avail_list_len, db ); 
    fclose( db ); 
    return 0;
}