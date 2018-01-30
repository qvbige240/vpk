/*
 * History:
 * ================================================================
 * 2017-12-21 qing.zou created
 * 2018-01-29 added
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
		return_if_fail(result);

		DB_LOGI("=========================================================================");
		DB_LOGI("row, size: %d", vpk_iterator_size(result));

		vpk_for_each(vpk_iterator_t*, row, result)
		{
			DB_LOGI("[row: %d, col: size: %d]: ", row_itor, vpk_iterator_size(row));

			vpk_for_each(vpk_sql_value_t*, value, result)
			{
				DB_LOGI("[%s:%s]", vpk_sql_value_name(value), vpk_sql_value_text(value));
			}

		}

		vpk_database_sql_result_exit(database, result);
		
	} while (0);
}


int sample_database_sql_main(int argc, char** argv)
{
	vpk_database_t* database = vpk_database_sql_init(argv[1]);
	if (database)
	{
		if (vpk_database_sql_open(database))
		{
			sql_database_done(database, "drop table if exists table1");
			sql_database_done(database, "create table table1(id int, name text, number int)");
			sql_database_done(database, "insert into table1 values(1, 'name1', 52642)");
			sql_database_done(database, "insert into table1 values(2, 'name2', 57127)");
			sql_database_done(database, "insert into table1 values(3, 'name3', 9000)");
			sql_database_done(database, "insert into table1 values(4, 'name4', 29000)");
			sql_database_done(database, "insert into table1 values(5, 'name5', 350000)");
			sql_database_done(database, "insert into table1 values(6, 'name6', 21000)");
			sql_database_done(database, "insert into table1 values(7, 'name7', 21600)");
			sql_database_done(database, "select * from table1");
		}
	}

	return 0;
}
