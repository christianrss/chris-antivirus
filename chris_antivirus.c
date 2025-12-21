/* chris_antivirus.c */
#include "chris_antivirus.h"

// bool testfun(Entry e) {
//     return (e.type == dir);
// }

bool iself(Entry e) {
    int32 fd;
    signed int ret;
    int8 path[64];
    char buf[4];

    if (e.type != file)
        return false;

    zero(path, 64);
    snprintf($c path, 63, "%s/%s", $c e.dir, $c e.file);
    ret = open($c path, O_RDONLY);
    if (ret < 1)
        return false;
    else
        fd = $4 ret;

    zero($1 buf, 4);
    read($i fd, buf, 4);
    // ssize_t n = read($i fd, buf, 4);
    // if (n == -1) {
    //     perror("read");
    //     exit(1);
    // } else if (n!=4) {
    //     fprintf(stderr, "Incomplete read (%zd bytes)\n", n);
    // }
    close($i fd);

    if (
        (buf[0] == 0x7f)
        && (buf[1] == 0x45)
        && (buf[2] == 0x4c)
        && (buf[3] == 0x46)
    )
        return true;
    else
        return false;
}

Database *filter(Database *input, function f) {
    int32 n;
    Database *output;
    Entry *p;
    bool predicate;

    output = mkdatabase();
    for (n=0; n < input->num; n++) {
        p = &input->entries[n];
        predicate = f(*p);
        if (predicate)
            addtodb(output, *p);
    }
    destroydb(input);

    return output;
}

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
    unsigned char *dtype;

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

            dtype = p2 + p->d_reclen - 1;
            if (*dtype == DT_REG) {
                e.type = file;
                strncpy($c e.dir, $c path, 63);
                strncpy($c e.file, $c filename, 31);
                addtodb(db, e);
            }
            else if (*dtype == DT_DIR) {
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
    Database *db, *db2;

    assert(argc > 1);

    db = mkdatabase();
    adddir(db, $1 argv[1]);
    db2 = filter(db, &iself);
    showdb(db2);
    destroydb(db2);

    return 0;
}