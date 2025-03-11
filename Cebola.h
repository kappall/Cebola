/* Cebola.h */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <dirent.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;

#define BLOCK_SIZE 500000
#define VERSION "0.1"
#define PATH_MAX 128
#define PATH_DEPTH_MAX 7
#define MAX_SIGNATURE_LENGTH 256
#define MAX_VIRUS_NAME_LENGTH 64
#define DB_SIZE sizeof(Database)
#define ENTRY_SIZE sizeof(Entry)

#define $1 (int8 *)
#define $2 (int16)
#define $4 (int32)
#define $8 (int64)
#define $c (char *)
#define $i (int)

#define log(f, ...) printf(f "\n", ##__VA_ARGS__); fflush(stdout);

#define MH_MAGIC 0xfeedface
#define MH_CIGAM 0xcefaedfe
#define MH_MAGIC_64 0xfeedfacf
#define MH_CIGAM_64 0xcffaedfe
#define FAT_MAGIC 0xcafebabe
#define FAT_CIGAM 0xbebafeca

typedef int8 Dir[64]; // arbitrary, contains entire dir structure
typedef int8 File[32];
typedef unsigned long long int Timestamp;

typedef enum e_filetype {
    file = 1,
    dir = 2,
    other = 3
} Filetype;

typedef enum e_state {
    unscanned = 1,
    scanning = 2,
    infected = 3,
    healed = 4,
    clean = 5,
    error = 6,
} EState;

typedef struct s_state {
    EState stage;
    int8 virus[32];
} State;

typedef struct s_db_entry {
    Dir dir;
    File file;
    Filetype type;
    Timestamp lastscanned;
    State *state;
} Entry;

typedef struct s_db {
    Entry *entries;
    int32 capacity;
    int32 size; 
} Database;


typedef bool (*function)(Entry);

void computeLPSArray(const int8*,int32,int32*);

bool KMPSearch(const int8*, const int8*);

State *make_state();

Database *scan(Database*,int32);

Database *prepare(char*);

Database *filter(Database*,function);

Database *make_db();

void add_db(Database*,Entry*);

bool read_dir(Database*,const int8*,int);

void destroy_db(Database*);

void show_db(Database*);

int main(int,char**);