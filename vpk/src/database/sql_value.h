/*
 * History:
 * ================================================================
 * 2017-12-21 qing.zou created
 *
 */
#ifndef SQL_VALUE_H
#define SQL_VALUE_H

#include "vpk.h"

TIMA_BEGIN_DELS

typedef long long		db_int64_t;
typedef int				db_int32_t;

#define SQL_CONFIG_TYPE_HAVE_FLOAT

typedef enum vpk_database_sql_value_type
{
	DATABASE_SQL_VALUE_TYPE_NULL			= 0,	
	DATABASE_SQL_VALUE_TYPE_INT8			= 1,	
	DATABASE_SQL_VALUE_TYPE_INT16			= 2,	
	DATABASE_SQL_VALUE_TYPE_INT32			= 3,	
	DATABASE_SQL_VALUE_TYPE_INT64			= 4,
	DATABASE_SQL_VALUE_TYPE_UINT8			= 5,	
	DATABASE_SQL_VALUE_TYPE_UINT16			= 6,	
	DATABASE_SQL_VALUE_TYPE_UINT32			= 7,	
	DATABASE_SQL_VALUE_TYPE_UINT64			= 8,
#ifdef SQL_CONFIG_TYPE_HAVE_FLOAT
	DATABASE_SQL_VALUE_TYPE_FLOAT			= 13,
	DATABASE_SQL_VALUE_TYPE_DOUBLE			= 14,
#endif
	DATABASE_SQL_VALUE_TYPE_BLOB8			= 9,
	DATABASE_SQL_VALUE_TYPE_BLOB16			= 10,
	DATABASE_SQL_VALUE_TYPE_BLOB32			= 11,
	DATABASE_SQL_VALUE_TYPE_TEXT			= 12

} vpk_database_sql_value_type;


typedef void	vpk_sql_stream_t;

typedef struct vpk_sql_value_t
{
	int							type;
	const char*					name;

	/// the data
	union 
	{
		//// int
		//tb_int8_t               i8;
		//tb_int16_t              i16;
		//tb_int32_t              i32;
		//tb_int64_t              i64;

		//// uint
		//tb_uint8_t              u8;
		//tb_uint16_t             u16;
		//tb_uint32_t             u32;
		//tb_uint64_t             u64;
		db_int32_t				i32;
		db_int64_t				i64;

		// float
#ifdef SQL_CONFIG_TYPE_HAVE_FLOAT
		float					f;
		double					d;
#endif

		// blob
		struct 
		{
			const char*			data;
			int					size;

			// the stream for blob32
			//tb_stream_ref_t  stream;
			vpk_sql_stream_t*	stream;

		} blob;

		// text
		struct 
		{
			const char*			data;
			int					hint;

		} text;

	}u; 

} vpk_sql_value_t;


/** the value data size
 *
 * @param value     the value
 *
 * @return          the value data size
 */
int vpk_sql_value_size(const vpk_sql_value_t* value);

/** the int32 value
 *
 * @param value     the value
 *
 * @return          the int32 value
 */
db_int32_t vpk_sql_value_int32(const vpk_sql_value_t* value);

/** the int64 value
 *
 * @param value     the value
 *
 * @return          the int64 value
 */
db_int64_t vpk_sql_value_int64(const vpk_sql_value_t* value);

#ifdef SQL_CONFIG_TYPE_HAVE_FLOAT
/*! the float value
 *
 * @param value     the value
 *
 * @return          the float value
 */
float vpk_sql_value_float(const vpk_sql_value_t* value);

/*! the double value
 *
 * @param value     the value
 *
 * @return          the double value
 */
double	vpk_sql_value_double(const vpk_sql_value_t* value);
#endif

/*! set the null value
 *
 * @param value     the value
 * @param number    the number
 */
void	vpk_sql_value_set_null(vpk_sql_value_t* value);

/** set the int32 value
 *
 * @param value     the value
 * @param number    the number
 */
void	vpk_sql_value_set_int32(vpk_sql_value_t* value, db_int32_t number);

/** set the int64 value
 *
 * @param value     the value
 * @param number    the number
 */
void	vpk_sql_value_set_int64(vpk_sql_value_t* value, db_int64_t number);

#ifdef SQL_CONFIG_TYPE_HAVE_FLOAT
/** set the float value
 *
 * @param value     the value
 * @param number    the number
 */
void	vpk_sql_value_set_float(vpk_sql_value_t* value, float number);

/** set the double value
 *
 * @param value     the value
 * @param number    the number
 */
void	vpk_sql_value_set_double(vpk_sql_value_t* value, double number);
#endif

/** set the text value
 *
 * @param value     the value
 * @param text      the text data
 * @param hint      the text size hint
 */
void	vpk_sql_value_set_text(vpk_sql_value_t* value, const char* text, int hint);

/** set the blob32 value		//...
 *
 * @param value     the value
 * @param data      the blob data
 * @param size      the blob size
 * @param stream    the stream, using it if data == null
 */
void	vpk_sql_value_set_blob32(vpk_sql_value_t* value, const char* data, int size, vpk_sql_stream_t* stream);

/* inline */
static INLINE int vpk_sql_value_is_null(const vpk_sql_value_t* value)
{
	return (value && value->type == DATABASE_SQL_VALUE_TYPE_NULL) ? 1 : 0;
}

static INLINE int vpk_sql_value_is_text(const vpk_sql_value_t* value)
{
	return (value && value->type == DATABASE_SQL_VALUE_TYPE_TEXT) ? 1 : 0;
}

static INLINE int vpk_sql_value_is_blob(const vpk_sql_value_t* value)
{
	return (value 
		&&  (value->type == DATABASE_SQL_VALUE_TYPE_BLOB32
		||  value->type == DATABASE_SQL_VALUE_TYPE_BLOB16
		||  value->type == DATABASE_SQL_VALUE_TYPE_BLOB8)) ? 1 : 0;
}

static INLINE int vpk_sql_value_is_blob32(const vpk_sql_value_t* value)
{
	return (value && value->type == DATABASE_SQL_VALUE_TYPE_BLOB32) ? 1 : 0;
}

static INLINE int vpk_sql_value_is_integer(const vpk_sql_value_t* value)
{
	return (value && value->type >= DATABASE_SQL_VALUE_TYPE_INT8 
		&& value->type <= DATABASE_SQL_VALUE_TYPE_UINT64) ? 1 : 0;
}

#ifdef SQL_CONFIG_TYPE_HAVE_FLOAT
static INLINE int vpk_sql_value_is_float(const vpk_sql_value_t* value)
{
	return (value 
		&& (value->type == DATABASE_SQL_VALUE_TYPE_FLOAT
		||  value->type == DATABASE_SQL_VALUE_TYPE_DOUBLE)) ? 1 : 0;
}
#endif

static INLINE int vpk_sql_value_is_number(const vpk_sql_value_t* value)
{
	return (value && value->type >= DATABASE_SQL_VALUE_TYPE_INT8 && value->type < DATABASE_SQL_VALUE_TYPE_BLOB8) ? 1 : 0;
}

static INLINE int vpk_sql_value_type(const vpk_sql_value_t* value)
{
	return_val_if_fail(value, DATABASE_SQL_VALUE_TYPE_NULL);

	return value->type;
}

static INLINE const char* vpk_sql_value_name(const vpk_sql_value_t* value)
{
	return_val_if_fail(value, NULL);

	return value->name;
}

static INLINE const char* vpk_sql_value_text(const vpk_sql_value_t* value)
{
	return_val_if_fail(value, NULL);

	if (vpk_sql_value_is_text(value)) 
		return value->u.text.data;
	else if (vpk_sql_value_is_blob(value))
		return value->u.blob.data;
	else if (vpk_sql_value_is_null(value))
		return NULL;

	printf("not text value type: %d", value->type);
	return NULL;
}

static INLINE const char* vpk_sql_value_blob(const vpk_sql_value_t* value)
{
	return_val_if_fail(value, NULL);

	if (vpk_sql_value_is_blob(value)) 
		return value->u.blob.data;
	else if (vpk_sql_value_is_text(value)) 
		return value->u.text.data;
	else if (vpk_sql_value_is_null(value))
		return NULL;

	printf("not blob value type: %d", value->type);
	return NULL;
}

static INLINE vpk_sql_stream_t* vpk_sql_value_blob_stream(const vpk_sql_value_t* value)
{
	return vpk_sql_value_is_blob(value) ? value->u.blob.stream : NULL;
}

/* set the value name */
static INLINE void vpk_sql_value_name_set(vpk_sql_value_t* value, const char* name)
{
	return_if_fail(value);

	value->name = name;
}

TIMA_END_DELS

#endif //SQL_VALUE_H
