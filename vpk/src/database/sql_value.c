/*
 * History:
 * ================================================================
 * 2017-12-21 qing.zou created
 *
 */

#include "sql_value.h"
#include "sql/sql_impl.h"

int vpk_sql_value_size(const vpk_sql_value_t* value)
{
	return_val_if_fail(value, 0);

	switch (value->type)
	{
	case DATABASE_SQL_VALUE_TYPE_INT32:
#ifdef SQL_CONFIG_TYPE_HAVE_FLOAT
	case DATABASE_SQL_VALUE_TYPE_FLOAT:
#endif
		return 4;
	case DATABASE_SQL_VALUE_TYPE_INT64:
#ifdef SQL_CONFIG_TYPE_HAVE_FLOAT
	case DATABASE_SQL_VALUE_TYPE_DOUBLE:
#endif
		return 8;
	case DATABASE_SQL_VALUE_TYPE_BLOB32:
	case DATABASE_SQL_VALUE_TYPE_BLOB16:
	case DATABASE_SQL_VALUE_TYPE_BLOB8:
		return value->u.blob.size;
	case DATABASE_SQL_VALUE_TYPE_TEXT:
		if (!value->u.text.hint && value->u.text.data)
		{
			((vpk_sql_value_t*)value)->u.text.hint = strlen(value->u.text.data);
		}
		return value->u.text.hint;
	default:	//...
		DB_LOGE("unknown type: %d", value->type);
		break;
	}

	return 0;
}

db_int32_t vpk_sql_value_int32(const vpk_sql_value_t* value)
{
	return_val_if_fail(value, 0);
	switch (value->type)
	{
	case DATABASE_SQL_VALUE_TYPE_INT32:
		return (db_int32_t)value->u.i32;
	case DATABASE_SQL_VALUE_TYPE_INT64:
		return (db_int32_t)value->u.i64;
#ifdef SQL_CONFIG_TYPE_HAVE_FLOAT
	case DATABASE_SQL_VALUE_TYPE_FLOAT:
		return (db_int32_t)value->u.f;
	case DATABASE_SQL_VALUE_TYPE_DOUBLE:
		return (db_int32_t)value->u.d;
#endif
	case DATABASE_SQL_VALUE_TYPE_TEXT:
		return value->u.text.data ? atoi(value->u.text.data) : 0;
	default:	//...
		DB_LOGE("unknown number type: %d", value->type);
		break;
	}

	return 0;
}

db_int64_t vpk_sql_value_int64(const vpk_sql_value_t* value)
{
	return_val_if_fail(value, 0);
	switch (value->type)
	{
	case DATABASE_SQL_VALUE_TYPE_INT32:
		return (db_int64_t)value->u.i32;
	case DATABASE_SQL_VALUE_TYPE_INT64:
		return (db_int64_t)value->u.i64;
#ifdef SQL_CONFIG_TYPE_HAVE_FLOAT
	case DATABASE_SQL_VALUE_TYPE_FLOAT:
		return (db_int64_t)value->u.f;
	case DATABASE_SQL_VALUE_TYPE_DOUBLE:
		return (db_int64_t)value->u.d;
#endif
	case DATABASE_SQL_VALUE_TYPE_TEXT:
		return value->u.text.data ? atoll(value->u.text.data) : 0;
	default:	//...
		DB_LOGE("unknown number type: %d", value->type);
		break;
	}

	return 0;
}

#ifdef SQL_CONFIG_TYPE_HAVE_FLOAT
float vpk_sql_value_float(const vpk_sql_value_t* value)
{
	return_val_if_fail(value, 0);
	switch (value->type)
	{
	case DATABASE_SQL_VALUE_TYPE_INT32:
		return (float)value->u.i32;
	case DATABASE_SQL_VALUE_TYPE_INT64:
		return (float)value->u.i64;
	case DATABASE_SQL_VALUE_TYPE_FLOAT:
		return value->u.f;
	case DATABASE_SQL_VALUE_TYPE_DOUBLE:
		return (float)value->u.d;
	case DATABASE_SQL_VALUE_TYPE_TEXT:
		return value->u.text.data ? atof(value->u.text.data) : 0;
	default:
		DB_LOGE("unknown number type: %d", value->type);
		break;
	}

	return 0;
}

double vpk_sql_value_double(const vpk_sql_value_t* value)
{
	return_val_if_fail(value, 0);
	switch (value->type)
	{
	case DATABASE_SQL_VALUE_TYPE_INT32:
		return (double)value->u.i32;
	case DATABASE_SQL_VALUE_TYPE_INT64:
		return (double)value->u.i64;
	case DATABASE_SQL_VALUE_TYPE_FLOAT:
		return (double)value->u.f;
	case DATABASE_SQL_VALUE_TYPE_DOUBLE:
		return value->u.d;
	case DATABASE_SQL_VALUE_TYPE_TEXT:
		return value->u.text.data ? atof(value->u.text.data) : 0;
	default:
		DB_LOGE("unknown number type: %d", value->type);
		break;
	}

	return 0;
}
#endif

void vpk_sql_value_set_null(vpk_sql_value_t* value)
{
	return_if_fail(value);
	value->type = DATABASE_SQL_VALUE_TYPE_NULL;
}

void vpk_sql_value_set_int32(vpk_sql_value_t* value, int number)
{
	return_if_fail(value);

	value->type		= DATABASE_SQL_VALUE_TYPE_INT32;
	value->u.i32	= number;
}

void vpk_sql_value_set_int64(vpk_sql_value_t* value, db_int64_t number)
{
	return_if_fail(value);

	value->type		= DATABASE_SQL_VALUE_TYPE_INT64;
	value->u.i64	= number;
}

#ifdef SQL_CONFIG_TYPE_HAVE_FLOAT
void vpk_sql_value_set_float(vpk_sql_value_t* value, float number)
{
	return_if_fail(value);

	value->type		= DATABASE_SQL_VALUE_TYPE_FLOAT;
	value->u.f		= number;
}
void vpk_sql_value_set_double(vpk_sql_value_t* value, double number)
{
	return_if_fail(value);

	value->type		= DATABASE_SQL_VALUE_TYPE_DOUBLE;
	value->u.d		= number;
}
#endif

void vpk_sql_value_set_text(vpk_sql_value_t* value, const char* text, int hint)
{
	return_if_fail(value);

	value->type			= DATABASE_SQL_VALUE_TYPE_TEXT;
	value->u.text.data	= text;
	value->u.text.hint	= hint;
}

//...
void vpk_sql_value_set_blob32(vpk_sql_value_t* value, const char* data, int size, vpk_sql_stream_t* stream)
{
}
