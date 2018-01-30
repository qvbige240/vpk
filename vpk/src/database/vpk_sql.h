/*
 * History:
 * ================================================================
 * 2017-12-21 qing.zou created
 *
 */
#ifndef VPK_SQL_H
#define VPK_SQL_H

#include "sql/sql_impl.h"

TIMA_BEGIN_DELS


/** the sql database type enum  **/
typedef enum vpk_database_type
{
	VPK_DATABASE_SQL_TYPE_NONE				= 0,
	VPK_DATABASE_SQL_TYPE_MYSQL				= 1,
	VPK_DATABASE_SQL_TYPE_SQLITE3			= 2
} vpk_database_type;

/** init sql database
 *
 * @param url                       the database url
 *                                  "/home/file.sqlite3"
 *
 * @return                          the database 
 */
vpk_database_t* vpk_database_sql_init(const char* url);

/** exit database
 *
 * @param database                  the database handle
 */
void vpk_database_sql_exit(vpk_database_t* database);

/** the database type
 *
 * @param database                  the database handle
 *
 * @return                          the database type
 */
int vpk_database_sql_type(vpk_database_t* database);

/** open database
 *
 * @code
    vpk_database_t* database = vpk_database_sql_init("/home/file.sqlite3");
    if (database)
    {
        // open it
        if (vpk_database_sql_open(database))
        {
            // done it
            // ...

            // close it
            vpk_database_sql_clos(database);
        }
        vpk_database_sql_exit(database);
    }
 * @endcode
 *
 * @param database                  the database handle
 *
 * @return                          vpk_true or vpk_false
 */
int vpk_database_sql_open(vpk_database_t* database);

/** clos database
 *
 * @param database                  the database handle
 */
void vpk_database_sql_clos(vpk_database_t* database);

/** begin transaction
 *
 * @param database                  the database handle
 *
 * @return                          vpk_true or vpk_false
 */
int vpk_database_sql_begin(vpk_database_t* database);

/** commit transaction
 *
 * @param database                  the database handle
 *
 * @return                          vpk_true or vpk_false
 */
int vpk_database_sql_commit(vpk_database_t* database);

/** rollback transaction
 *
 * @param database                  the database handle
 *
 * @return                          vpk_true or vpk_false
 */
int vpk_database_sql_rollback(vpk_database_t* database);

///** the database state
// *
// * @param database                  the database handle
// *
// * @return                          the database state
// */
//int vpk_database_sql_state(vpk_database_t* database);

/** done database
 *
 * @code
 *
 * // done sql
 * if (!vpk_database_sql_done(database, "select * from table"))
 * {
 *     // trace
 *     vpk_trace_e("done sql failed, error: %s", vpk_state_cstr(vpk_database_sql_state(database)));
 *     return ;
 * }
 *
 * // load result
 * // ..
 *
 * @endcode
 *
 * @param database                  the database handle
 * @param sql                       the sql command
 *
 * @return                          vpk_true or vpk_false
 */
int vpk_database_sql_done(vpk_database_t* database, const char* sql);

/** load the database result
 *
 * @param database                  the database handle
 * @param try_all                   try loading all result into memory
 *
 * @return                          the database result
 */
vpk_iterator_t* vpk_database_sql_result_load(vpk_database_t* database, int try_all);

/** exit the database result
 *
 * @param database                  the database handle
 * @param result                    the database result
 */
void vpk_database_sql_result_exit(vpk_database_t* database, vpk_iterator_t* result);


/** init the database statement
 *
 * @param database                  the database handle
 * @param sql                       the sql command
 *
 * @return                          the statement handle
 */
vpk_sql_statement_t* vpk_database_sql_statement_init(vpk_database_t* database, const char* sql);

/** exit the database statement
 *
 * @param database                  the database handle
 * @param statement                 the statement handle
 */
void vpk_database_sql_statement_exit(vpk_database_t* database, vpk_sql_statement_t* statement);

/** done the database statement
 *
 * @code
    vpk_sql_statement_t* statement = vpk_database_sql_statement_init(database, "select * from table where id=?");
    if (statement)
    {
        // bind arguments
        vpk_database_sql_value_t list[1] = {0};
        vpk_database_sql_value_set_int32(&list[0], 12345);
        if (vpk_database_sql_statement_bind(database, statement, list, vpk_arrayn(list)))
        {
            // done statement
            if (vpk_database_sql_statement_done(database, statement))
            {
                // load result
                // ...
            }
        }

        // exit statement
        vpk_database_sql_statement_exit(database, statement);
    }
 * @endcode
 *
 * @param database                  the database handle
 * @param statement                 the statement handle
 *
 * @return                          vpk_true or vpk_false
 */
int vpk_database_sql_statement_done(vpk_database_t* database, vpk_sql_statement_t* statement);

/** bind the database statement argument
 *
 * @param database                  the database handle
 * @param statement                 the statement handle
 * @param list                      the argument value list
 * @param size                      the argument value count
 *
 * @return                          vpk_true or vpk_false
 */
int vpk_database_sql_statement_bind(vpk_database_t* database, vpk_sql_statement_t* statement, const vpk_sql_value_t* list, int size);

TIMA_END_DELS

#endif //VPK_SQL_H
