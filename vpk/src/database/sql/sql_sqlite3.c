/*
 * History:
 * ================================================================
 * 2017-12-21 qing.zou created
 *
 */
#include <sqlite3.h>

#include "sql_sqlite3.h"
#include "../vpk_sql.h"

// the sqlite3 result row type
typedef struct sqlite3_result_row_t
{
	vpk_iterator_t				iterator;		/** iterator **/
	int							index;			/** row index **/
	int							ncol;			/** col count **/
	vpk_sql_value_t				value;			/** col value **/
} sqlite3_result_row_t;

// the sqlite3 result type
typedef struct sqlite3_result_t
{
	vpk_iterator_t				iterator;		/** iterator **/
	char**						result;			/** result **/
	sqlite3_stmt*				statement;		/** statement **/
	int							nrow;			/** row count **/
	sqlite3_result_row_t		row;			/** result row **/
} sqlite3_result_t;

// the sqlite3 type
typedef struct vpk_database_sqlite3_t
{
	vpk_database_t				base;			/** the base **/
	sqlite3*					database;		/** the database **/
	sqlite3_result_t			result;			/** the result **/
} vpk_database_sqlite3_t;


/** iterator implementation **/
static int database_sqlite3_result_row_iterator_size(vpk_iterator_t* iterator)
{
	sqlite3_result_t *result = (sqlite3_result_t*)iterator;
	return_val_if_fail(result, -1);

	return result->nrow;
}
static int database_sqlite3_result_row_iterator_head(vpk_iterator_t* iterator)
{
	return 0;
}
static int database_sqlite3_result_row_iterator_last(vpk_iterator_t* iterator)
{
	sqlite3_result_t *result = (sqlite3_result_t*)iterator;
	return_val_if_fail(result, -1);

	return result->nrow;
}
static int database_sqlite3_result_row_iterator_tail(vpk_iterator_t* iterator)
{
	sqlite3_result_t *result = (sqlite3_result_t*)iterator;
	return_val_if_fail(result, -1);

	return result->nrow;
}
static int database_sqlite3_result_row_iterator_prev(vpk_iterator_t* iterator, int index)
{
	sqlite3_result_t *result = (sqlite3_result_t*)iterator;
	return_val_if_fail(result, -1);

	return_val_if_fail(!result->statement, result->nrow);

	if (!(index && index <= result->nrow)) {
		DB_LOGE("index error");
		return result->nrow;
	}

	return index - 1;
}
static int database_sqlite3_result_row_iterator_next(vpk_iterator_t* iterator, int index)
{
	sqlite3_result_t *result = (sqlite3_result_t*)iterator;
	return_val_if_fail(result, -1);
	if (!(index < result->nrow))
		return result->nrow;

	if (result->statement) {
		if (SQLITE_ROW != sqlite3_step(result->statement))
		{
			if (SQLITE_OK != sqlite3_reset(result->statement)) {
				vpk_database_sqlite3_t* sqlite = (vpk_database_sqlite3_t*)iterator->priv;
				if (sqlite && sqlite->database)
					DB_LOGE("statement: reset failed, error[%d]: %s", sqlite3_errcode(sqlite->database), sqlite3_errmsg(sqlite->database));
			}

			return result->nrow;
		}
	}

	return index + 1;
}
static void* database_sqlite3_result_row_iterator_item(vpk_iterator_t* iterator, int index)
{
	sqlite3_result_t *result = (sqlite3_result_t*)iterator;
	return_val_if_fail(result && (result->result || result->statement) && index < result->nrow, NULL);
	
	/* save row index */
	result->row.index = index;

	return &result->row;
}

/** col **/
static int database_sqlite3_result_col_iterator_size(vpk_iterator_t* iterator)
{
	sqlite3_result_row_t *row = (sqlite3_result_row_t*)iterator;
	return_val_if_fail(row, 0);

	return row->ncol;
}
static int database_sqlite3_result_col_iterator_head(vpk_iterator_t* iterator)
{
	sqlite3_result_row_t *row = (sqlite3_result_row_t*)iterator;
	return_val_if_fail(row, 0);

	return 0;
}
static int database_sqlite3_result_col_iterator_last(vpk_iterator_t* iterator)
{
	sqlite3_result_row_t *row = (sqlite3_result_row_t*)iterator;
	return_val_if_fail(row, 0);

	return row->ncol;
}
static int database_sqlite3_result_col_iterator_tail(vpk_iterator_t* iterator)
{
	sqlite3_result_row_t *row = (sqlite3_result_row_t*)iterator;
	return_val_if_fail(row, 0);

	return row->ncol;
}
static int database_sqlite3_result_col_iterator_prev(vpk_iterator_t* iterator, int index)
{
	sqlite3_result_row_t *row = (sqlite3_result_row_t*)iterator;
	return_val_if_fail(row && index && index <= row->ncol, 0);

	return index - 1;
}
static int database_sqlite3_result_col_iterator_next(vpk_iterator_t* iterator, int index)
{
	sqlite3_result_row_t *row = (sqlite3_result_row_t*)iterator;
	return_val_if_fail(row && index < row->ncol, row->ncol);

	return index + 1;
}
static void* database_sqlite3_result_col_iterator_item(vpk_iterator_t* iterator, int index)
{
	sqlite3_result_row_t *row = (sqlite3_result_row_t*)iterator;
	return_val_if_fail(row && index < row->ncol, NULL);

	vpk_database_sqlite3_t *sqlite = (vpk_database_sqlite3_t *)iterator->priv;
	return_val_if_fail(sqlite, NULL);

	if (sqlite->result.result) {
		vpk_sql_value_name_set(&row->value, (const char*)sqlite->result.result[index]);
		//DB_LOGI("name set: %s", (char*)sqlite->result.result[index]);
		vpk_sql_value_set_text(&row->value, (const char*)sqlite->result.result[(row->index+1) * row->ncol + index], 0);
		//DB_LOGI("text set: %s", (char*)sqlite->result.result[(row->index+1) * row->ncol + index]);
		return &row->value;
	} else if (sqlite->result.statement) {
		sqlite3_stmt* statement = sqlite->result.statement;

		vpk_sql_value_name_set(&row->value, sqlite3_column_name(statement, index));

		int type = sqlite3_column_type(statement, index);
		switch (type)//...
		{
		case SQLITE_INTEGER:
			vpk_sql_value_set_int32(&row->value, sqlite3_column_int(statement, index));
			break;
		case SQLITE_TEXT:
			vpk_sql_value_set_text(&row->value, (const char *)sqlite3_column_text(statement, index), 
				sqlite3_column_bytes(statement, index));
			break;
		case SQLITE_FLOAT:
#ifdef SQL_CONFIG_TYPE_HAVE_FLOAT
			vpk_sql_value_set_double(&row->value, sqlite3_column_double(statement, index));
			break;
#else
			DB_LOGE("float type is not supported, at col: %d!", index);
			return NULL;
#endif
		case SQLITE_BLOB:	//...
			break;
		case SQLITE_NULL:
			vpk_sql_value_set_null(&row->value);
			break;
		default:
			DB_LOGE("unknown field type: %d, at col: %d", type, index);
			return NULL;
		}

		return &row->value;
	}

	return NULL;
}

/** sql implementation **/
static int database_sqlite3_open(vpk_database_t* database)
{
	vpk_database_sqlite3_t* sqlite = (vpk_database_sqlite3_t*)database;
	return_val_if_fail(sqlite, 0);
	const char* path = (const char*)database->url.path;

	//... sqlite3_initialize/sqlite3_shutdown maybe need

	// open database
	if (SQLITE_OK != sqlite3_open_v2(path, &sqlite->database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) || !sqlite->database) 
	{
		// error
		if (sqlite->database)
			DB_LOGE("open: %s failed, error[%d]: %s", path, sqlite3_errcode(sqlite->database), sqlite3_errmsg(sqlite->database));
		return 0;
	}

	DB_LOGD(("open: %s: ok", path));

	return 1;
}

static void database_sqlite3_clos(vpk_database_t* database)
{
	vpk_database_sqlite3_t* sqlite = (vpk_database_sqlite3_t*)database;
	return_if_fail(sqlite != NULL);
	
	if (sqlite->result.result)
		sqlite3_free_table(sqlite->result.result);
	sqlite->result.result = NULL;

	/* close database */
	if (sqlite->database)
		sqlite3_close(sqlite->database);
	sqlite->database = NULL;
}

static void database_sqlite3_exit(vpk_database_t* database)
{
	vpk_database_sqlite3_t* sqlite = (vpk_database_sqlite3_t*)database;
	return_if_fail(sqlite != NULL);

	database_sqlite3_clos(database);

	VPK_FREE(sqlite);
}

static int database_sqlite3_done(vpk_database_t* database, const char* sql)
{
	int ret = -1;
	vpk_database_sqlite3_t* sqlite = (vpk_database_sqlite3_t*)database;
	return_val_if_fail(sqlite && sqlite->database && sql, -1);

	do 
	{
		if (sqlite->result.result)
			sqlite3_free_table(sqlite->result.result);
		sqlite->result.result = NULL;

		sqlite->result.statement = NULL;

		sqlite->result.nrow = 0;
		sqlite->result.row.ncol = 0;

		int nrow = 0, ncol = 0;
		char *error = NULL;
		if (SQLITE_OK != sqlite3_get_table(sqlite->database, sql, &sqlite->result.result, &nrow, &ncol, &error)) {
			DB_LOGE("done: sql: %s failed, error[%d]: %s", sql, sqlite3_errcode(sqlite->database), error);
			if (error) sqlite3_free(error);
			break;
		}

		if (!nrow) {
			if (sqlite->result.result)
				sqlite3_free_table(sqlite->result.result);
			sqlite->result.result = NULL;
			DB_LOGD(("DONE: sql: %s: ok", sql));
			ret = 0;
			break;
		}

		sqlite->result.iterator.mode = VPK_ITERATOR_MODE_RACCESS | VPK_ITERATOR_MODE_READONLY;
		sqlite->result.nrow = nrow;
		sqlite->result.row.ncol = ncol;

		DB_LOGD(("DONE: sql: %s: ok", sql));
		ret = 0;

	} while (0);

	return ret;
}

static int database_sqlite3_begin(vpk_database_t* database)
{
	vpk_database_sqlite3_t* sqlite = (vpk_database_sqlite3_t*)database;
	return_val_if_fail(sqlite && sqlite->database, -1);

	if (SQLITE_OK != sqlite3_exec(sqlite->database, "begin;", NULL, NULL, NULL))
	{	
		DB_LOGE("begin: failed, error[%d]: %s", sqlite3_errcode(sqlite->database), sqlite3_errmsg(sqlite->database));
		return -1;
	}
	return 0;
}

static int database_sqlite3_commit(vpk_database_t* database)
{
	vpk_database_sqlite3_t* sqlite = (vpk_database_sqlite3_t*)database;
	return_val_if_fail(sqlite && sqlite->database, -1);

	if (SQLITE_OK != sqlite3_exec(sqlite->database, "commit;", NULL, NULL, NULL))
	{	
		DB_LOGE("commit: failed, error[%d]: %s", sqlite3_errcode(sqlite->database), sqlite3_errmsg(sqlite->database));
		return -1;
	}
	return 0;
}

static int database_sqlite3_rollback(vpk_database_t* database)
{
	vpk_database_sqlite3_t* sqlite = (vpk_database_sqlite3_t*)database;
	return_val_if_fail(sqlite && sqlite->database, -1);

	if (SQLITE_OK != sqlite3_exec(sqlite->database, "rollback;", NULL, NULL, NULL))
	{	
		DB_LOGE("rollback: failed, error[%d]: %s", sqlite3_errcode(sqlite->database), sqlite3_errmsg(sqlite->database));
		return -1;
	}
	return 0;
}

static vpk_iterator_t* database_sqlite3_result_load(vpk_database_t* database, int try_all)
{
	vpk_database_sqlite3_t* sqlite = (vpk_database_sqlite3_t*)database;
	return_val_if_fail(sqlite && sqlite->database, NULL);

	return (sqlite->result.result || sqlite->result.statement) ? (vpk_iterator_t*)&sqlite->result : NULL;
}

static void database_sqlite3_result_exit(vpk_database_t* database, vpk_iterator_t* result)
{
	sqlite3_result_t *sqlite_result = (sqlite3_result_t*)result;
	return_if_fail(sqlite_result);

	if (sqlite_result->result)
		sqlite3_free_table(sqlite_result->result);
	sqlite_result->result = NULL;

	sqlite_result->statement = NULL;

	sqlite_result->nrow = 0;
	sqlite_result->row.ncol = 0;
}

/** statement **/
static vpk_sql_statement_t* database_sqlite3_statement_init(vpk_database_t* database, const char* sql)
{
	vpk_database_sqlite3_t* sqlite = (vpk_database_sqlite3_t*)database;
	return_val_if_fail(sqlite && sqlite->database && sql, NULL);
	sqlite3_stmt* statement = NULL;

	if (SQLITE_OK != sqlite3_prepare_v2(sqlite->database, sql, -1, &statement, 0))
	{
		DB_LOGE("statement: init %s failed, error[%d]: %s", sql, sqlite3_errcode(sqlite->database), sqlite3_errmsg(sqlite->database));
	}
	return statement;
}

static void database_sqlite3_statement_exit(vpk_database_t* database, vpk_sql_statement_t* statement)
{
	if (statement) sqlite3_finalize((sqlite3_stmt*)statement);
}

static int database_sqlite3_statement_done(vpk_database_t* database, vpk_sql_statement_t* statement)
{
	int ret = -1;
	vpk_database_sqlite3_t* sqlite = (vpk_database_sqlite3_t*)database;
	return_val_if_fail(sqlite && sqlite->database && statement, -1);

	do 
	{
		if (sqlite->result.result)
			sqlite3_free_table(sqlite->result.result);
		sqlite->result.result = NULL;

		sqlite->result.statement = NULL;

		sqlite->result.nrow = 0;
		sqlite->result.row.ncol = 0;

		int result = sqlite3_step((sqlite3_stmt*)statement);
		return_val_if_fail(result == SQLITE_DONE || result == SQLITE_ROW, -1);

		if (result == SQLITE_ROW) {
			sqlite->result.iterator.mode = VPK_ITERATOR_MODE_FORWARD | VPK_ITERATOR_MODE_READONLY;
			sqlite->result.statement = (sqlite3_stmt*)statement;

			sqlite->result.nrow = -1;
			sqlite->result.row.ncol = sqlite3_column_count((sqlite3_stmt*)statement);
		} else {
			if (SQLITE_OK != sqlite3_reset((sqlite3_stmt*)statement))
				DB_LOGE("statement: reset failed, error[%d]: %s", sqlite3_errcode(sqlite->database), sqlite3_errmsg(sqlite->database));
			break;
		}

		ret = 0;
	} while (0);

	return ret;
}

static void database_sqlite3_statement_bind_exit(void *data)
{
	DB_LOGD(("bind: exit: %p", data));

	if (data) VPK_FREE(data);
}
static int database_sqlite3_statement_bind(vpk_database_t* database, vpk_sql_statement_t* statement, const vpk_sql_value_t* list, int size)
{
	vpk_database_sqlite3_t* sqlite = (vpk_database_sqlite3_t*)database;
	return_val_if_fail(sqlite && sqlite->database && statement && list && size, -1);

	int param_count = sqlite3_bind_parameter_count((sqlite3_stmt*)statement);
	if (param_count != size) {
		DB_LOGE("param_count != size");
		return -1;
	}

	int i = 0;
	for (i = 0; i < size; i++)
	{
		const vpk_sql_value_t *value = &list[i];
		int rc = SQLITE_ERROR;
		unsigned char *data = NULL;
		switch (value->type)	//...
		{
		case DATABASE_SQL_VALUE_TYPE_TEXT:
			DB_LOGD(("sqlite3: test %d %s", i, value->u.text.data));
			rc = sqlite3_bind_text((sqlite3_stmt*)statement, i + 1, value->u.text.data, vpk_sql_value_size(value), NULL);
			break;
		case DATABASE_SQL_VALUE_TYPE_INT32:
			rc = sqlite3_bind_int((sqlite3_stmt*)statement, i + 1, vpk_sql_value_int32(value));
			break;
		case DATABASE_SQL_VALUE_TYPE_INT64:
			rc = sqlite3_bind_int64((sqlite3_stmt*)statement, i + 1, vpk_sql_value_int64(value));
			break;
#ifdef SQL_CONFIG_TYPE_HAVE_FLOAT
		case DATABASE_SQL_VALUE_TYPE_FLOAT:
		case DATABASE_SQL_VALUE_TYPE_DOUBLE:
			rc = sqlite3_bind_double((sqlite3_stmt*)statement, i + 1, vpk_sql_value_double(value));
			break;
#endif
		case DATABASE_SQL_VALUE_TYPE_NULL:
			rc = sqlite3_bind_null((sqlite3_stmt*)statement, i + 1);
			break;
		default:
			DB_LOGE("statement: bind: unknown value type: %d", value->type);
			break;
		}

		if (SQLITE_OK != rc)
		{
			if (data) VPK_FREE(data);
			data = NULL;

			DB_LOGE("statement: bind value[d] failed, error[%d]: %s", i, sqlite3_errcode(sqlite->database), sqlite3_errmsg(sqlite->database));
			break;
		}
	}

	return (i == size) ? 0 : -1;
}

int database_sqlite3_probe(vpk_url_t *url)
{
	return 0;
}

vpk_database_t* database_sqlite3_init(vpk_url_t *url)
{
	return_val_if_fail(url, NULL);
	int ok = 1;
	vpk_database_sqlite3_t* sqlite = NULL;
	do 
	{
		sqlite = VPK_CALLOC(1, sizeof(vpk_database_sqlite3_t));
		return_val_if_fail(sqlite, NULL);

		sqlite->base.type	= VPK_DATABASE_SQL_TYPE_SQLITE3;
		sqlite->base.open				= database_sqlite3_open;
		sqlite->base.clos				= database_sqlite3_clos;
		sqlite->base.exit				= database_sqlite3_exit;
		sqlite->base.done				= database_sqlite3_done;
		sqlite->base.begin				= database_sqlite3_begin;
		sqlite->base.commit				= database_sqlite3_commit;
		sqlite->base.rollback			= database_sqlite3_rollback;
		sqlite->base.result_load		= database_sqlite3_result_load;
		sqlite->base.result_exit		= database_sqlite3_result_exit;
		sqlite->base.statement_init		= database_sqlite3_statement_init;
		sqlite->base.statement_exit		= database_sqlite3_statement_exit;
		sqlite->base.statement_done		= database_sqlite3_statement_done;
		sqlite->base.statement_bind		= database_sqlite3_statement_bind;
       
		/* init result row iterator */
		sqlite->result.iterator.mode	= 0;
		sqlite->result.iterator.step	= 0;
		sqlite->result.iterator.priv	= (void*)sqlite;
		sqlite->result.iterator.size	= database_sqlite3_result_row_iterator_size;
		sqlite->result.iterator.head	= database_sqlite3_result_row_iterator_head;
		sqlite->result.iterator.last	= database_sqlite3_result_row_iterator_last;
		sqlite->result.iterator.tail	= database_sqlite3_result_row_iterator_tail;
		sqlite->result.iterator.prev	= database_sqlite3_result_row_iterator_prev;
		sqlite->result.iterator.next	= database_sqlite3_result_row_iterator_next;
		sqlite->result.iterator.item	= database_sqlite3_result_row_iterator_item;
		sqlite->result.iterator.copy	= NULL;
		sqlite->result.iterator.comp	= NULL;

		/* init result col iterator */
		sqlite->result.row.iterator.mode = VPK_ITERATOR_MODE_RACCESS | VPK_ITERATOR_MODE_READONLY;
		sqlite->result.row.iterator.step = 0;
		sqlite->result.row.iterator.priv = (void*)sqlite;
		sqlite->result.row.iterator.size = database_sqlite3_result_col_iterator_size;
		sqlite->result.row.iterator.head = database_sqlite3_result_col_iterator_head;
		sqlite->result.row.iterator.last = database_sqlite3_result_col_iterator_last;
		sqlite->result.row.iterator.tail = database_sqlite3_result_col_iterator_tail;
		sqlite->result.row.iterator.prev = database_sqlite3_result_col_iterator_prev;
		sqlite->result.row.iterator.next = database_sqlite3_result_col_iterator_next;
		sqlite->result.row.iterator.item = database_sqlite3_result_col_iterator_item;
		sqlite->result.row.iterator.copy = NULL;
		sqlite->result.row.iterator.comp = NULL;

		memcpy(&sqlite->base.url, url, sizeof(*url));

		ok = 1;
	} while (0);

	if (!ok)
	{
		if (sqlite) 
			database_sqlite3_exit((vpk_database_t*)sqlite);
		sqlite = NULL;
	}

	return (vpk_database_t*)sqlite;
}
