/* chris_antivirus.c */
#include "chris_antivirus.h"

Database mkdatabase(int16 cap) {
    Database db;
    Entry *p;
    int16 size;

    db.num = 0;
    db.cap = cap;
    size = cap * sizeof(Entry);
    p = (Entry *)malloc($i size);
    assert(p);
    zero($1 p, size);
    db.entries = p;

    return db;
}

void destroydb(Database db) {
    db.cap = 0;
    db.num = 0;
    free(db.entries);

    return;
}

void showdb(Database db) {
    printf("cap:\t%d\nnum:\t%d\n",
        db.cap, db.num);

    return;
}

int main(int argc, char *argv[]) {
    Database db;

    db = mkdatabase(50000);
    showdb(db);
    destroydb(db);

    return 0;
}