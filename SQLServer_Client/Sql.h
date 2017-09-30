#ifndef SQL_H
#define SQL_H

const char*
Sql_init(const char*, char*, size_t*);

void
Sql_end();

typedef int (*Sql_callback_tfp)(void*, int, char* [], char* []);

int
Sql_readAll(void*, Sql_callback_tfp);

int
Sql_read(void*, Sql_callback_tfp, const char*);

int
Sql_update(const char*, const char*);

int
Sql_command(const char* const, Sql_callback_tfp, void*);

#endif
