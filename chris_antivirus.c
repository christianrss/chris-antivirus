/* chris_antivirus.c */
#include "chris_antivirus.h"

Database *mkdatabase() {
    Database *db;
    Entry *p;
    int32 size;

    size = sizeof(struct s_database);
    db = (Database *)malloc($i size);
    assert(db);
    zero($1 db, size);

    db->num = 0;
    db->cap = Blocksize;
    size = Blocksize * sizeof(struct s_entry);
    p = (Entry *)malloc($i size);
    assert(p);
    zero($1 p, size);
    db->entries = p;

    return db;
}

void destroydb(Database *db) {
    db->cap = 0;
    db->num = 0;
    free(db->entries);
    free(db);

    return;
}

void showdb(Database *db) {
    int32 n;

    printf("cap:\t%d\nnum:\t%d\n",
        db->cap, db->num);

    for (n=0; n<db->num; n++)
        printf("%s/%s\n", db->entries[n].dir, db->entries[n].file);

    return;
}

void addtodb(Database *db, Entry e) {
    int32 size, cap, ix;

    if (db->num == db->cap) {
        cap = db->cap + Blocksize;
        size = cap * sizeof(struct s_entry);
        db->entries = realloc(db->entries, size);
        assert(db->entries);
        db->cap = cap;
    }

    ix = db->num;
    memcpy($c &db->entries[ix], $c &e, sizeof(struct s_entry));
    db->num++;

    return;
}

bool adddir(Database *db, int8 *path) {
    Entry e;
    int32 fd;
    int64 n;
    signed int ret;
    struct linux_dirent *p;
    int8 *p2;
    int8 buf[102400];

    ret = open($c path, O_RDONLY|O_DIRECTORY);
    if (ret < 1)
        return false;
    else
        fd = $4 ret;

    memset($c buf, 0, sizeof(buf));
    ret = syscall(SYS_getdents, $i fd, buf, (sizeof(buf)-1));
    if (ret < 0) {
        close($i fd);
        return false;
    }
    n = ret;

    // write(1, buf, $i n);
    // close($i fd);
    // printf("\n\n");
    // exit(0);

    for (p2 = buf; n; n-= p->d_reclen, p2 += p->d_reclen) {
        p = (struct linux_dirent*)p2;
        // printf("p->d_reclen:\t %d\n", p->d_reclen);
        // printf("p->d_off:\t %lu\n", p->d_off);
        // printf("p->d_name:\t %s\n", p->d_name);
        // printf("\n");
        zero($1 &e, sizeof(struct s_entry));
        strncpy($c e.dir, $c path, 63);
        strncpy($c e.file, $c p->d_name, 31);
        addtodb(db, e);
    }
    close($i fd);

    return true;
}

int main(int argc, char *argv[]) {
    Database *db;

    assert(argc > 1);

    db = mkdatabase();
    adddir(db, $1 argv[1]);
    showdb(db);
    destroydb(db);

    return 0;
}