/* Cebola.c */
#include "Cebola.h"

void computeLPSArray(const int8 *pattern, int32 M, int32 *lps) {
    int len = 0;
    lps[0] = 0;
    int i = 1;
    while (i < M) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

bool KMPSearch(const int8 *pattern, const int8 *text) {
    int M = strlen(pattern);
    int N = strlen(text);
    int *lps = malloc(M * sizeof(int));
    if (lps == NULL) {
        printf("Memory allocation failed\n");
        exit(-1);
        return false;
    }
    computeLPSArray(pattern, M, lps);
    int i = 0, j = 0;
    while (i < N) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }
        if (j == M) {
            return true;
            j = lps[j - 1];
        } else if (i < N && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }
    free(lps);
    return 0;
}

State *make_state() {
    State* s;

    int32 size = sizeof(State);

    s = (State*) malloc(size);
    assert(s);
    memset(s, 0, size);
    s->stage = unscanned;
    return s;
}

bool is_executable_file_mac(Entry e) {
    struct stat sb;
    char filepath[PATH_MAX];
    if (snprintf(filepath, sizeof(filepath), "%s/%s", e.dir, e.file) >= sizeof(filepath)) {
        return false; 
    }
    if (stat(filepath, &sb) < 0)
        return false;
    if (!S_ISREG(sb.st_mode))
        return false;
    if (!(sb.st_mode & S_IXUSR || sb.st_mode & S_IXGRP || sb.st_mode & S_IXOTH))
        return false;
    FILE *fp = fopen(filepath, "rb");
    if (fp == NULL)
        return false;
    char first_two[2];
    if (fread(first_two, 1, sizeof(first_two), fp) < sizeof(first_two)) {
        fclose(fp);
        return false;
    }
    rewind(fp); 

    if (first_two[0] == '#' && first_two[1] == '!') {
        fclose(fp);
        return true;
    }
        
        
    uint32_t header;
    size_t read_bytes = fread(&header, 1, sizeof(header), fp);
    fclose(fp);
    if (read_bytes < sizeof(header))
        return false;
    if (header == MH_MAGIC || header == MH_MAGIC_64 || header == FAT_MAGIC ||
        header == MH_CIGAM || header == MH_CIGAM_64 || header == FAT_CIGAM)
        return true;
    return false;
}


Database *filter(Database *db, function f) {
    Database *ret;
    Entry ep;

    bool predicate;

    ret = make_db();
    for(int32 n = 0; n<db->size; n++) {
        ep = db->entries[n];
        predicate = f(ep);
        if(predicate)
            add_db(ret,&ep);
    }

    destroy_db(db);
    return ret;
}

Database *make_db() {
    Database *db;
    Entry *p;

    db = (Database*) malloc(DB_SIZE);
    assert(db);
    memset(db, 0, DB_SIZE);
    db->size = 0;
    db->capacity = BLOCK_SIZE;

    p = (Entry*) malloc(ENTRY_SIZE*BLOCK_SIZE);
    assert(p);
    memset(p, 0, ENTRY_SIZE);
    db->entries = p;
    
    
    return db;
}

void add_db(Database *db, Entry* e) {

    if(db->size == db->capacity) {
        int32 n_cap = db->capacity + BLOCK_SIZE;
        int32 n_size = n_cap * ENTRY_SIZE;

        void *temp = realloc(db->entries, n_size);
        if(!temp){
            destroy_db(db);
            exit(1);
        }
        db->entries = temp;
        db->capacity = n_cap;
    }
    int32 indx = db->size;
    memcpy(&db->entries[indx], e, ENTRY_SIZE);
    db->size++;
}

bool read_dir(Database *db, const int8 *path, int depth) {
    if(depth==PATH_DEPTH_MAX)
        return false;
    DIR *curr_dir;
    struct dirent *de;

    curr_dir = opendir($c path);
    if (!curr_dir) {
        perror("opendir");
        return false;
    }

    Entry *e = (Entry *) malloc(ENTRY_SIZE);
    assert(e);
    memset(e, 0, ENTRY_SIZE);
    while ((de = readdir(curr_dir)) != NULL) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
            continue;
        }
        switch (de->d_type)
        {
        case 4: {
            size_t newpath_len = strlen($c path) + 1 +strlen(de->d_name) + 1;
            int8 *newpath = malloc(newpath_len);
            assert(newpath);
            
            strcpy($c newpath,$c path);
            strcat(strcat($c newpath, "/"), de->d_name);
            if(!read_dir(db,newpath,depth+1)){
                fprintf(stderr, "unable to read: %s\n", $c newpath);
            }
            free(newpath);
            continue;
        }
        case 8:
            e->type = file;
            break;
        default:
            e->type = other;
            break;
        }
        e->lastscanned = 0;
        e->state = make_state();
        strncpy($c e->dir, $c path, sizeof(e->dir) - 1);
        strncpy($c e->file, $c de->d_name, sizeof(e->file) - 1);

        add_db(db, e);
    }

    closedir(curr_dir);
    return true;
}

void show_db(Database *db){
    printf("This database has %d entries and can contain %d in total, %d available\n",db->size,db->capacity,db->capacity-db->size);
    for(int i = 0; i < db->size; ++i) {
        printf("%s/%s%c\n", db->entries[i].dir,db->entries[i].file,(db->entries[i].type==dir)? '/':0);
    }
}

void destroy_db(Database *db){
    db->capacity = 0;
    db->size = 0;
    free(db->entries);
    free(db);
}

Database *prepare(char *startDir) {
    Database *db;

    db = make_db();
    log("Enumerating Filesystem...");
    read_dir(db, startDir,0);
    log("Done enumarating: found %d files\n",db->size);

    log("Filetering out non-excetuables...")
    db = filter(db,&is_executable_file_mac);
    show_db(db);
    log("Done filtering, %d files left\n", db->size);

    return db;

}

Database *scan(Database *db, int32 fd) {
    
    char signature[MAX_SIGNATURE_LENGTH];
    char virus_name[MAX_VIRUS_NAME_LENGTH];
    FILE *virus_file = fdopen(fd, "r");
    
    if (!virus_file) {
        perror("Failed to open virus definitions file");
        return db;
    }

    for (int32 i = 0; i < db->size; i++) {
        Entry *entry = &db->entries[i];
        char filepath[PATH_MAX];
        snprintf(filepath, PATH_MAX, "%s/%s", entry->dir, entry->file);
        
        FILE *target_file = fopen(filepath, "rb");
        if (!target_file) {
            entry->state->stage = error;
            continue;
        }

        // Get file size
        fseek(target_file, 0, SEEK_END);
        long file_size = ftell(target_file);
        rewind(target_file);

        // Read entire file into buffer
        unsigned char *file_buffer = malloc(file_size);
        if (!file_buffer) {
            fclose(target_file);
            entry->state->stage = error;
            continue;
        }
        
        fread(file_buffer, 1, file_size, target_file);
        
        rewind(virus_file);
        
        while (fscanf(virus_file, "%s %s", signature, virus_name) == 2) {
            if(KMPSearch($1 signature,file_buffer)){
                entry->state->stage = infected;
                log("FOUND FILE INFECTED: %s with %s", filepath,virus_name);
                break;
            }

        }

        free(file_buffer);
        fclose(target_file);
    }
    
    fclose(virus_file);
    return db;
}

int main(int argc, char *argv[]) {
    if(argc<2){
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return -1;
    }
    int32 fd;
    Database *db, *scandb; 
    signed int ret;
    int8 virusfile[] = "./viruses.definitions";
    
    if((ret = open($c virusfile, O_RDONLY))<1) {
        fprintf(stderr, "Unable to open Viruses definitions file: %s\n",virusfile);
        exit(EXIT_FAILURE);
    }else {
        log("Cebola AntiVirus Software v%s",VERSION);
        fd = $4 ret;
        db = prepare(argv[1]);
        log("Scanning for viruses...");
        scandb = scan(db,fd);
        log("Scanning done");
        destroy_db(db);
    }
    
    return 0;
}