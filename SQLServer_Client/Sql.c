#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

#include "Sql.h"

static sqlite3* db;
static char* _table;
static size_t _nRows;
static const char* _dbName;

static int
init_callback(void *NotUsed, int argc, char **argv, char **azColName) {
	_nRows = (size_t)atoi(argv[0]);
	return 0;
}

const char*
Sql_init(const char* dbName, char* table, size_t* nRows) {
	_dbName = dbName;
	int rc = sqlite3_open(_dbName, &db);
	if(rc) {
		return sqlite3_errmsg(db);
	}

	if(table) {
		_table = table;

		char* cntCmd = "SELECT Count(*) FROM ";
		char* sql = (char*)calloc(
					strlen(cntCmd) + strlen(_table) + 1, sizeof(char));
		strcpy(sql, cntCmd);
		strcat(sql, _table);

		char* err = NULL;
		rc = sqlite3_exec(db, sql, init_callback, 0, &err);
		if(rc != SQLITE_OK) {
			fprintf(stderr, "SQL error: %s\n", err);
			sqlite3_free(err);
		}

		*nRows = _nRows;

		free(sql);
	}

	return NULL;
}

void
Sql_end() {
	if(sqlite3_close(db) != SQLITE_OK)
		fprintf(stderr, "%s not closed ok\n", _dbName);
}

int
Sql_readAll(void* data_ptr, Sql_callback_tfp cb) {
	char* const sql = (char*)calloc(strlen(_table) + 15, sizeof(char));
	strcat(sql, "select * from ");
	strcat(sql, _table);

	char* err = NULL;
	int rc = sqlite3_exec(db, sql, cb, data_ptr, &err);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", err);
		sqlite3_free(err);
	}

	free(sql);
	return rc;
}

int
Sql_read(void* data_ptr, Sql_callback_tfp cb, const char* select) {
	char* const sql = (char*)calloc(strlen(_table) + strlen(select) + 22,
											sizeof(char));
	strcat(sql, "select * from ");
	strcat(sql, _table);
	strcat(sql, " where ");
	strcat(sql, select);

	char* err = NULL;
	int rc = sqlite3_exec(db, sql, cb, data_ptr, &err);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", err);
		sqlite3_free(err);
	}

	free(sql);
	return rc;
}

int
Sql_update(const char* cmd, const char* pred) {
	size_t lcmd = strlen(cmd);
	size_t lpred = strlen(pred);
	size_t ltbl = strlen(_table);

	char* const sql = (char*)calloc(ltbl + lcmd + lpred + 21, sizeof(char));
	strcat(sql, "update ");
	strcat(sql, _table);
	strcat(sql, " set ");
	strcat(sql, cmd);
	strcat(sql, " where ");
	strcat(sql, pred);
	strcat(sql, ";");

	char* err = NULL;
	int rc = sqlite3_exec(db, sql, NULL, NULL, &err);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", err);
		sqlite3_free(err);
	}

	free(sql);
	return rc;
}

int
Sql_command(const char* const cmd, Sql_callback_tfp cb, void* args) {
	char* err = NULL;
	int rc = sqlite3_exec(db, cmd, cb, args, &err);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "SQL error %d: %s\n", rc, err);
		sqlite3_free(err);
	}

	return rc;
}
