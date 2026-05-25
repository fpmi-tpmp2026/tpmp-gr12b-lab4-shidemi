// Fallback minimal sqlite3 declarations (used if libsqlite3-dev not installed)
#ifdef __cplusplus
extern "C" {
#endif
typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;
typedef long long sqlite3_int64;
#define SQLITE_OK      0
#define SQLITE_ROW     100
#define SQLITE_DONE    101
#define SQLITE_STATIC  ((void(*)(void*))0)
int  sqlite3_open(const char*, sqlite3**);
int  sqlite3_close(sqlite3*);
int  sqlite3_exec(sqlite3*, const char*, int(*)(void*,int,char**,char**), void*, char**);
void sqlite3_free(void*);
const char* sqlite3_errmsg(sqlite3*);
int  sqlite3_prepare_v2(sqlite3*, const char*, int, sqlite3_stmt**, const char**);
int  sqlite3_step(sqlite3_stmt*);
int  sqlite3_finalize(sqlite3_stmt*);
int  sqlite3_reset(sqlite3_stmt*);
int  sqlite3_column_count(sqlite3_stmt*);
const char* sqlite3_column_name(sqlite3_stmt*, int);
const unsigned char* sqlite3_column_text(sqlite3_stmt*, int);
int  sqlite3_column_int(sqlite3_stmt*, int);
double sqlite3_column_double(sqlite3_stmt*, int);
int  sqlite3_bind_text(sqlite3_stmt*, int, const char*, int, void(*)(void*));
int  sqlite3_bind_int(sqlite3_stmt*, int, int);
int  sqlite3_bind_double(sqlite3_stmt*, int, double);
sqlite3_int64 sqlite3_last_insert_rowid(sqlite3*);
#ifdef __cplusplus
}
#endif
