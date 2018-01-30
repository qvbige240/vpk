/*
 * History:
 * ================================================================
 * 2017-12-21 qing.zou created
 * 2018-01-25 added
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include "vpk_sql.h"

#include "sql/sql_impl.h"
#include "sql/sql_sqlite3.h"

vpk_database_t* vpk_database_sql_init(const char* url)
{
	int ret = -1;
	vpk_database_t *database = NULL;
	vpk_url_t database_url = {0};
	do 
	{
		static vpk_database_t* (*sql_init[])(vpk_url_t *) =
		{
			NULL,
			database_sqlite3_init
		};

		if (url) memcpy(database_url.path, url, strlen(url));

		database = sql_init[1](&database_url);

		if (database)
			ret = 0;

	} while (0);

	if (ret != 0) {
		DB_LOGE("init: %s: no", url);
		
		if (database) vpk_database_sql_exit(database);
		database = NULL;
	}

	return database;
}

void vpk_database_sql_exit(vpk_database_t* database)
{
	vpk_database_t* impl = database;
	return_if_fail(impl);

	if (impl->exit)
		impl->exit(impl);

	DB_LOGD(("exit: ok"));
}

int vpk_database_sql_type(vpk_database_t* database)
{
	vpk_database_t* impl = database;
	return_val_if_fail(impl, VPK_DATABASE_SQL_TYPE_NONE);

	return impl->type;
}

int vpk_database_sql_open(vpk_database_t* database)
{
	vpk_database_t* impl = database;
	return_val_if_fail(impl && impl->open, -1);

	return_val_if_fail(!impl->opened, -1);

	impl->opened = impl->open(impl);

	return impl->opened;
}

void vpk_database_sql_clos(vpk_database_t* database)
{
	vpk_database_t* impl = database;
	return_if_fail(impl);

	if (!impl->opened) return;

	if (impl->clos) impl->clos(impl);

	impl->opened = 0;
}

int vpk_database_sql_begin(vpk_database_t* database)
{
	vpk_database_t* impl = database;
	return_val_if_fail(impl && impl->begin, -1);

	if (!impl->opened) return -1;

	return impl->begin(impl);
}

int vpk_database_sql_commit(vpk_database_t* database)
{
	vpk_database_t* impl = database;
	return_val_if_fail(impl && impl->commit, -1);

	if (!impl->opened) return -1;

	return impl->commit(impl);
}

int vpk_database_sql_rollback(vpk_database_t* database)
{
	vpk_database_t* impl = database;
	return_val_if_fail(impl && impl->rollback, -1);

	if (!impl->opened) return -1;

	return impl->rollback(impl);
}

//int vpk_database_sql_state(vpk_database_t* database);
int vpk_database_sql_done(vpk_database_t* database, const char* sql)
{
	vpk_database_t* impl = database;
	return_val_if_fail(impl && impl->done, -1);

	if (!impl->opened) return -1;

	int ret = impl->done(impl, sql);

	DB_LOGD(("done: sql: %s: %s", sql, ret == 0 ? "ok" : "no"));

	return ret;
}

vpk_iterator_t* vpk_database_sql_result_load(vpk_database_t* database, int all)
{
	vpk_database_t* impl = database;
	return_val_if_fail(impl && impl->result_load, NULL);

	if (!impl->opened) return NULL;

	return impl->result_load(impl, all);
}

void vpk_database_sql_result_exit(vpk_database_t* database, vpk_iterator_t* result)
{
	vpk_database_t* impl = database;
	return_if_fail(impl && impl->result_exit && result);
	if (!impl->opened) return ;

	impl->result_exit(impl, result);
}

vpk_sql_statement_t* vpk_database_sql_statement_init(vpk_database_t* database, const char* sql)
{
	vpk_database_t* impl = database;
	return_val_if_fail(impl && impl->statement_init && sql, NULL);

	if (!impl->opened) return NULL;

	return impl->statement_init(impl, sql);
}

void vpk_database_sql_statement_exit(vpk_database_t* database, vpk_sql_statement_t* statement)
{
	vpk_database_t* impl = database;
	return_if_fail(impl && impl->statement_exit && statement);
	if (!impl->opened) return ;

	impl->statement_exit(impl, statement);
}

int vpk_database_sql_statement_done(vpk_database_t* database, vpk_sql_statement_t* statement)
{
	vpk_database_t* impl = database;
	return_val_if_fail(impl && impl->statement_done && statement, -1);

	if (!impl->opened) return -1;

	return impl->statement_done(impl, statement);
}

int vpk_database_sql_statement_bind(vpk_database_t* database, vpk_sql_statement_t* statement, const vpk_sql_value_t* list, int size)
{
	vpk_database_t* impl = database;
	return_val_if_fail(impl && impl->statement_bind && statement && list && size, -1);

	if (!impl->opened) return -1;

	return impl->statement_bind(impl, statement, list, size);
}
