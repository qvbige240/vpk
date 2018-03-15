/*
 * History:
 * ================================================================
 * 2017-12-21 qing.zou created
 * 2018-01-29 added
 *
 *  ./sql test.sqlite
 *
 */

#include "vpk_sql.h"

#define vpk_for_each_item(type, item, head, tail, iterator) \
	/* iterator */ \
	vpk_iterator_t* item##_iterator = (vpk_iterator_t*)iterator; \
	/* init */ \
	type item; \
	int item##_itor = head; \
	int item##_head = head; \
	int item##_tail = tail; \
	/* walk */ \
	if (item##_iterator && item##_head != item##_tail) \
	for (   ; \
	item##_itor != item##_tail && ((item = (type)vpk_iterator_item(item##_iterator, item##_itor)), 1); \
	item##_itor = vpk_iterator_next(item##_iterator, item##_itor)) if ((item))

#define vpk_for_each(type, item, iterator) \
	vpk_iterator_t* item##_iterator_all = (vpk_iterator_t*)iterator; \
	vpk_for_each_item(type, item, vpk_iterator_head(item##_iterator_all), vpk_iterator_tail(item##_iterator_all), item##_iterator_all)


// vpk_for_all_if (vpk_iterator_t*, row, result, row)
/*
{
	vpk_iterator_t* row_iterator_all = (vpk_iterator_t*)result;

	vpk_iterator_t* row_iterator = (vpk_iterator_t*)result;
	int head = vpk_iterator_head(row_iterator);	
	int tail = vpk_iterator_tail(row_iterator);
	vpk_iterator_t* row;
	int row_itor = head;
	int row_head = head;
	int row_tail = tail;
	if (row_iterator && row_head != row_tail)
		for ( ; row_itor != row_tail && ((row = vpk_iterator_item(row_iterator, row_itor)), 1); row_itor = vpk_iterator_next(row_iterator, row_itor))
			if (row) {
				;
			}

}
*/

static void sql_database_done(vpk_database_t* database, const char* sql)
{
	int ret = 0;
	return_if_fail(database && sql);

	do 
	{
		ret = vpk_database_sql_done(database, sql);
		if (ret != 0)
		{
			DB_LOGE("done %s failed, error: %d", sql, ret);
			break;
		}

		vpk_iterator_t* result = vpk_database_sql_result_load(database, 0);
		//return_if_fail(result);
		if (!result) break;

		DB_LOGI("=========================================================================");
		DB_LOGI("row, size: %d", vpk_iterator_size(result));

		vpk_for_each(vpk_iterator_t*, row, result)
		{
			DB_LOGI("[row: %d, col size: %d]: ", row_itor, vpk_iterator_size(row));

			vpk_for_each(vpk_sql_value_t*, value, row)
			{
				printf(" [%s:%s]", vpk_sql_value_name(value), vpk_sql_value_text(value));
			}

			//const vpk_sql_value_t* number = (const vpk_sql_value_t*)vpk_iterator_item(row, 2);
			//if (!number) break;
			//printf("[%s:%d] ", vpk_sql_value_name(number), vpk_sql_value_int32(number));
			//printf("[%s:%s] ", vpk_sql_value_name(number), vpk_sql_value_text(number));
			//printf("[%s:%f] ", vpk_sql_value_name(number), vpk_sql_value_float(number));
			printf("\n");
		}

		vpk_database_sql_result_exit(database, result);
		
	} while (0);
}

static void sql_database_statement_done(vpk_database_t* database, const char* sql)
{
	return_if_fail(database && sql);

	vpk_sql_statement_t* statement = NULL;
	do 
	{
		statement = vpk_database_sql_statement_init(database, sql);
		if (!statement) {
			DB_LOGE("statement: init %s failed, error", sql);
			break;
		}

		if (vpk_database_sql_statement_done(database, statement) != 0) {
			DB_LOGE("statement: done %s failed, error", sql);
			break;
		}

		vpk_iterator_t* result = vpk_database_sql_result_load(database, 0);
		//return_if_fail(result);
		if (!result) break;

		DB_LOGI("=========================================================================");
		DB_LOGI("row, size: %d, tail: %d", vpk_iterator_size(result), vpk_iterator_tail(result));
		vpk_for_each(vpk_iterator_t*, row, result)
		{
// 				printf("index = %d, tail = %d  \n", row_itor, row_tail);

			DB_LOGI("[row: %d, col, size: %d] ", row_itor, vpk_iterator_size(row));

			const vpk_sql_value_t* id = (const vpk_sql_value_t*)vpk_iterator_item(row, 0);
			if (!id) break;
			printf(" [%s:%d] ", vpk_sql_value_name(id), vpk_sql_value_int32(id));

#ifdef SQL_CONFIG_TYPE_HAVE_FLOAT

			const vpk_sql_value_t* float_val = (const vpk_sql_value_t*)vpk_iterator_item(row, 1);
			if (!float_val) break;
			printf("[%s:%f] ", vpk_sql_value_name(float_val), vpk_sql_value_float(float_val));
#endif

			const vpk_sql_value_t* name = (const vpk_sql_value_t*)vpk_iterator_item(row, 2);
			if (!name) break;
			printf("[%s:%s] ", vpk_sql_value_name(name), vpk_sql_value_text(name));


			const vpk_sql_value_t* number = (const vpk_sql_value_t*)vpk_iterator_item(row, 3);
			if (!number) break;
			printf("[%s:%d] ", vpk_sql_value_name(number), vpk_sql_value_int32(number));


			const vpk_sql_value_t* snumber = (const vpk_sql_value_t*)vpk_iterator_item(row, 4);
			if (!snumber) break;
			printf("[%s:%d]\n", vpk_sql_value_name(snumber), vpk_sql_value_int32(snumber));
		}

		vpk_database_sql_result_exit(database, result);
	} while (0);

	if (statement) vpk_database_sql_statement_exit(database, statement);
}

static void sql_database_statement_done_insert(vpk_database_t* database, const char* sql, const char* name, int number, unsigned short snumber)
{
	return_if_fail(database && sql);

	vpk_sql_statement_t* statement = NULL;
	do 
	{
		statement = vpk_database_sql_statement_init(database, sql);
		if (!statement) {
			DB_LOGE("statement: init %s failed, error", sql);
			break;
		}

		vpk_sql_value_t list[3];
		vpk_sql_value_set_text(&list[0], name, 0);
		vpk_sql_value_set_int32(&list[1], number);
		vpk_sql_value_set_int32(&list[2], snumber);
		if (vpk_database_sql_statement_bind(database, statement, list, _countof(list)) != 0) {
			DB_LOGE("statement: bind %s failed, error", sql);
			break;
		}

		if (vpk_database_sql_statement_done(database, statement) != 0) {
			DB_LOGE("statement: done %s failed, error", sql);
			break;
		}
	} while (0);

	if (statement) vpk_database_sql_statement_exit(database, statement);
}


int sample_database_sql_main(int argc, char** argv)
{
	vpk_database_t* database = vpk_database_sql_init(argv[1]);
	if (database)
	{
		if (vpk_database_sql_open(database))
		{
			sql_database_done(database, "select name from sqlite_master where name='table1'");
			sql_database_done(database, "drop table if exists table1");
			sql_database_done(database, "create table table1(id int, name text, number int)");
			sql_database_done(database, "select count(*) from table1 where name='name7'");
			sql_database_done(database, "insert into table1 values(NULL, 'name1', 52642)");
			sql_database_done(database, "insert into table1 values(NULL, 'name2', 57127)");
			sql_database_done(database, "insert into table1 values(NULL, 'name3', 9000)");
			sql_database_done(database, "insert into table1 values(NULL, 'name4', 29000)");
			sql_database_done(database, "insert into table1 values(NULL, 'name5', 350000)");
			sql_database_done(database, "insert into table1 values(NULL, 'name6', 21000)");
			sql_database_done(database, "insert into table1 values(NULL, 'name7', 21600)");
			sql_database_done(database, "insert into table1 values(NULL, 'name7', 11100)");
			sql_database_done(database, "select * from table1");
			sql_database_done(database, "select * from table1 where name='name7'");
			sql_database_done(database, "select count(*) from table1 where name='name7'");
			sql_database_done(database, "select number from table1 where name='name7'");

			DB_LOGI("================================ statement ================================");
			sql_database_statement_done(database, "drop table if exists table2");

			if (vpk_database_sql_begin(database) == 0)
			{
				sql_database_statement_done(database, "create table table2(id int, fval float, name text, number int, snumber smallint)");

				sql_database_statement_done_insert(database, "insert into table2 values(1, 3.0, ?, ?, ?)", "name1", 52642, 2642);
				sql_database_statement_done_insert(database, "insert into table2 values(2, 3.1, ?, ?, ?)", "name2", 57127, 7127);
				sql_database_statement_done_insert(database, "insert into table2 values(3, 3.14, ?, ?, ?)", "name3", 9000, 9000);
				sql_database_statement_done_insert(database, "insert into table2 values(4, 3.1415, ?, ?, ?)", "name4", 29000, 9000);
				sql_database_statement_done_insert(database, "insert into table2 values(5, -3.1, ?, ?, ?)", "name5", 29000, 9000);
				sql_database_statement_done_insert(database, "insert into table2 values(6, 3.454, ?, ?, ?)", "name6", 21000, 1000);
				sql_database_statement_done_insert(database, "insert into table2 values(7, 100.098, ?, ?, ?)", "name7", 21600, 1600);

				vpk_database_sql_commit(database);

				sql_database_statement_done(database, "select * from table2");
			}
		}
		else
		{
			DB_LOGE("database open failed.");
		}
	}

	vpk_database_sql_exit(database);

	return 0;
}


#ifdef USE_ZLOG
#define SAMPLE_ZLOG_FILE_PATH		"."
#define SAMPLE_ZLOG_CONF_FILE		"./zlog.conf"
int sample_zlog_init(int procname)
{
	int rc;
	//zlog_category_t *c;

	if (!vpk_exists(SAMPLE_ZLOG_FILE_PATH)) {
		int ret = 0;
		char tmp[256] = {0};
		vpk_pathname_get(SAMPLE_ZLOG_FILE_PATH, tmp);
		printf("full: %s, pathname: %s", SAMPLE_ZLOG_FILE_PATH, tmp);
		ret = vpk_mkdir_mult(SAMPLE_ZLOG_FILE_PATH);
		printf("vpk_mkdir_mult \'%s\' ret = %d\n", SAMPLE_ZLOG_FILE_PATH, ret);
	}

	rc = dzlog_init(SAMPLE_ZLOG_CONF_FILE, "sample");
	if (rc)	{
		printf("zlog init failed\n");
		return -1;
	}

	LOG_D("hello, zlog");

	return 0;
}
#endif

int main(int argc, char *argv[])
{
	//int ret = 0;

#ifdef USE_ZLOG
	sample_zlog_init(0);
#endif // USE_ZLOG

	sample_database_sql_main(argc, argv);

	return 0;
}
