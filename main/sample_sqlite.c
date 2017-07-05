/**
 * History:
 * ================================================================
 * 2017-07-05 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>

#include <errno.h>

#include "vpk.h"

#if defined(_X86_)
#else
#endif

//#define USE_SQLITE3
#ifdef USE_SQLITE3

#include "sqlite3.h"

// primary key is integer means it auto-increment
// const char* sql01 = "create table carnet_t(ID integer primary key autoincrement, name nvarchar(32), value text, priority INTEGER)";

const char* sql01 = "create table carnet_t1(ID INTEGER PRIMARY KEY, name nvarchar(32), value text, priority INTEGER)";
const char* sql02 = "insert into carnet_t1 values(NULL, 'king', 'kvalue', 12)";
const char* sql03 = "insert into carnet_t1 values(NULL, 'time1', 'tvalue', 13)";
const char* sql04 = "insert into carnet_t1 values(NULL, 'time2', 'tvalue', 13)";
const char* sql05 = "update carnet_t1 set value = 'tvalue1' where name='time2'";
const char* sql_select = "select * from carnet_t1";
const char* sql_tables = "select name from sqlite_master where type='table' order by name";

static int callback(void *notused, int argc, char **argv, char **colname) {
	int i = 0;
	for (i = 0; i < argc; i++) {
		LOG_D("%s = %s", colname[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

int sqlite_test(char* filename)
{
	sqlite3 *db;
	char *errmsg = NULL;
	int rc;

	rc = sqlite3_open(filename, &db);
	if (rc) {
		LOG_E("Can't open database: %s", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	rc = sqlite3_exec(db, sql01, callback, 0, &errmsg);
	if (rc != SQLITE_OK) {
		LOG_E("SQL error: %s", errmsg);
		sqlite3_free(errmsg);
	}

	rc = sqlite3_exec(db, sql02, callback, 0, &errmsg);
	if (rc != SQLITE_OK) {
		LOG_E("SQL error: %s", errmsg);
		sqlite3_free(errmsg);
	}

	rc = sqlite3_exec(db, sql03, callback, 0, &errmsg);
	if (rc != SQLITE_OK) {
		LOG_E("SQL error: %s", errmsg);
		sqlite3_free(errmsg);
	}

	rc = sqlite3_exec(db, sql04, callback, 0, &errmsg);
	if (rc != SQLITE_OK) {
		LOG_E("SQL error: %s", errmsg);
		sqlite3_free(errmsg);
	}

	rc = sqlite3_exec(db, sql_select, callback, 0, &errmsg);
	if (rc != SQLITE_OK) {
		LOG_E("SQL error: %s", errmsg);
		sqlite3_free(errmsg);
	}

	rc = sqlite3_exec(db, sql05, callback, 0, &errmsg);
	if (rc != SQLITE_OK) {
		LOG_E("SQL error: %s", errmsg);
		sqlite3_free(errmsg);
	}

	rc = sqlite3_exec(db, sql_select, callback, 0, &errmsg);
	if (rc != SQLITE_OK) {
		LOG_E("SQL error: %s", errmsg);
		sqlite3_free(errmsg);
	}

	sqlite3_close(db);
	return 0;
}
#else
int sqlite_test(char* filename)
{
	(void*)filename;
	LOG_D("without use sqlite3!");
	return 0;
}
#endif // USE_SQLITE3

#if 1

#elif 0

#elif 0

#endif

int sqlite_main(int argc, char *argv[])
{
// 	int ret = 0;
// 
// 	vpk_system_init(argc, argv);
// 	vpk_logging_level_set("DEBUG");

	char* pathname = "./mytest.db";
	//if (argc > 1)
	//{
	//	type = argv[1];
	//}

	int o;
	static const struct option long_options[] = {
		{ "help",			no_argument,			NULL, 'h' },
		{ "version",		no_argument,			NULL, 'V' },
		{ "sample",			required_argument,		NULL, 'd' },
		{ "type",			required_argument,		NULL, 't' },
		{ "keycode",		required_argument,		NULL, 'k' },
		{ "file",			required_argument,		NULL, 'f' },
		{ "url",			required_argument,		NULL, 'u' },
		{ "log",			optional_argument,		NULL, 'l' },
		{ NULL, 0, NULL, 0 }
	};

	optind = 1;
	//LOG_I("22 optind = %d, argc = %d", optind, argc);
	while ((o = getopt_long(argc, argv, "hVd:t:k:f:u:l", long_options, NULL)) >= 0) {
		//printf("opt = %c\n", o);  
		//printf("optarg = %s\n", optarg);  
		//printf("optind = %d\n", optind);  
		//printf("argv[optind - 1] = %s\n",  argv[optind - 1]);
		switch(o) {
			case 'f':
				pathname = optarg;
				break;
			default:
				break;
		}
	}

	LOG_D("db name = %s", pathname);

	double elapsed;
	struct timeval result, prev, next;	

	gettimeofday(&prev, 0);

	sqlite_test(pathname);

	gettimeofday(&next, 0);
	vpk_timersub(&next, &prev, &result);
	//time_sub(&result, &prev, &next);
	elapsed = result.tv_sec + (result.tv_usec / 1.0e6);
	LOG_D("vpk time elapsed: %.6f, %d(s) %d(us) \n", elapsed, result.tv_sec, result.tv_usec);

	return 0;
}
