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
        printf("%s/%s%c\n", db->entries[n].dir, db->entries[n].file,
            (db->entries[n].type == dir)) ?
                    '/':
                0;

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
    int8 buf[102400], tmp[64];
    char *filename;

    ret = open($c path, O_RDONLY|O_DIRECTORY);
    if (ret < 1)
        return false;
    else
        fd = $4 ret;

    do {
        memset($c buf, 0, sizeof(buf));
        ret = syscall(SYS_getdents, $i fd, buf, (sizeof(buf)-1));
        if (ret < 0) {
            close($i fd);
            return false;
        } else if (!ret)
            break;

        n = ret;

        for (p2 = buf; n; n-= p->d_reclen, p2 += p->d_reclen) {
            p = (struct linux_dirent*)p2;
            zero($1 &e, sizeof(struct s_entry));

            filename = p->d_name-1;

            if (onedot(filename) || twodots(filename))
                continue;

            if (p->d_type & DT_REG) {
                e.type = file;
                strncpy($c e.dir, $c path, 63);
                strncpy($c e.file, $c filename, 31);
                addtodb(db, e);
            }
            else if (p->d_type & DT_DIR) {
                e.type = dir;
                strncpy($c e.dir, $c path, 63);
                strncpy($c e.file, $c filename, 31);
                addtodb(db, e);

                zero(tmp, 64);
                snprintf($c tmp, 63, "%s/%s", $c path, $c e.file);
                adddir(db, tmp);
            }
        }
    } while (true);
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