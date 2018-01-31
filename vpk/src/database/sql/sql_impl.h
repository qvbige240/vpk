/*
 * History:
 * ================================================================
 * 2017-12-21 qing.zou created
 *
 */
#ifndef SQL_IMPL_H
#define SQL_IMPL_H

#include "../vpk_iterator.h"
#include "../sql_value.h"

TIMA_BEGIN_DELS

#if 1
#define DB_LOGD(x)	LOG_D x
#define DB_LOGI		LOG_I
#define DB_LOGW		LOG_W
#define DB_LOGE		LOG_E
#define DB_LOGF		LOG_F
#else
#define DB_LOGD(x)
#define DB_LOGI		LOG_I
#define DB_LOGW		LOG_W
#define DB_LOGE		LOG_E
#define DB_LOGF		LOG_F
#endif


typedef struct vpk_url_t
{
	unsigned int  size;
	unsigned char path[256];
} vpk_url_t;

typedef void vpk_sql_statement_t;

struct vpk_database_t;
typedef struct vpk_database_t vpk_database_t;

struct vpk_database_t
{
	vpk_url_t			url;
	unsigned int		type;
	unsigned char		opened;

	int					(*open)(vpk_database_t* database);
	void				(*clos)(vpk_database_t* database);
	void				(*exit)(vpk_database_t* database);
	int					(*done)(vpk_database_t* database, const char* sql);

	int					(*begin)(vpk_database_t* database);
	int					(*commit)(vpk_database_t* database);
	int					(*rollback)(vpk_database_t* database);
	vpk_iterator_t*		(*result_load)(vpk_database_t* database, int try_all);
	void				(*result_exit)(vpk_database_t* database, vpk_iterator_t* result);

	vpk_sql_statement_t*	(*statement_init)(vpk_database_t* database, const char* sql);
	void	(*statement_exit)(vpk_database_t* database, vpk_sql_statement_t* statement);
	int		(*statement_done)(vpk_database_t* database, vpk_sql_statement_t* statement);
	int		(*statement_bind)(vpk_database_t* database, vpk_sql_statement_t* statement, const vpk_sql_value_t* list, int size);
};


TIMA_END_DELS

#endif //SQL_IMPL_H
