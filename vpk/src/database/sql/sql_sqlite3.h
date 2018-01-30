/*
 * History:
 * ================================================================
 * 2017-12-21 qing.zou created
 *
 */

#ifndef SQL_SQLITE3_H
#define SQL_SQLITE3_H

#include "sql_impl.h"

TIMA_BEGIN_DELS

/* probe sqlite3 from the url
 *
 * @param url               the database url
 *
 * @return                  the score
 */
int					database_sqlite3_probe(vpk_url_t *url);

/* init sqlite3
 *
 * @param url               the database url
 *
 * @return                  the database handle
 */
vpk_database_t*		database_sqlite3_init(vpk_url_t *url);

TIMA_END_DELS

#endif //SQL_SQLITE3_H
