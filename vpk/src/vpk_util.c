/**
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
#include <stdarg.h>
#include "vpk_util.h"

int vpk_hex_to_int(char c)
{
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}
	else if (c >= 'A' && c <= 'F')
	{
		return c - 'A' + 0x0A;
	}
	else if (c >= 'a' && c <= 'f')
	{
		return c - 'a' + 0x0a;
	}

	return 0;
}

char* vpk_strstrip(char* str, char c)
{
	if (str == NULL)
		return str;

	{
		char* s = str;
		char* d = str;
		while (*s != '\0')
		{
			if (*s != c) 
			{
				*d = *s;
				d++;
			}
			s++;
		}
		*d = '\0';
	}

	return str;
}

char* remove_colons(char* str)
{
	char* p = NULL;
	if (str == NULL)
		return str;

	p = str + strlen(str) - 1;

	while(p != str && *p == ':')
	{
		*p = '\0';
		p--;
	}

	p = str;
	while(*p != '\0' && *p == ':') p++;

	//if (p != str)
	{
		char* s = p;
		char* d = str;
		while (*s != '\0')
		{
			if (*s != ':') 
			{
				*d = *s;
				d++;
			}
			s++;
		}
		*d = '\0';
	}

	return str;
}

int vpk_strcntstr(const char *s1, const char *s2)
{
	int cnt = 0;
	char *p = NULL, *ptr = (char*)s1;
	unsigned int len1 = 0, len2 = 0;

	len1 = strlen(s1);
	len2 = strlen(s2);

	if (!len2)
		return 0;
	
	while (s1 < ptr + len1)
	{
		p = strstr(s1, s2);
		if (p == NULL)
			break;

		cnt++;
		s1 = p + len2;
	}

	return cnt;
}

void vpk_snprintf(char *buf, unsigned int *pos, int len, const char *format, ...)
{
	va_list va;
	va_start(va, format);
	*pos += vsnprintf(&buf[*pos], len - *pos, format, va);
	va_end(va);
	if (*pos >= len ) {
		buf[len] = 0;
		*pos = len;
	}
}
/*
static char* strcat_arg(char* str, unsigned int len, const char* first, va_list arg)
{
	unsigned int dst = 0;
	const char* iter = first;
	if (len <= 0 || iter == NULL)
	{
		return NULL;
	}

	while(iter != NULL && dst < len)
	{
		for (; *iter != NULL && dst < len; dst++,iter++)
		{
			str[dst] = *iter;
		}

		iter = va_arg(arg, char*);
	}
	va_end(arg);

	if (dst < len)
	{
		str[dst] = '\0';
	}
	else
	{
		str[len-1] = '\0';
	}

	return str;
}

char* vpk_strcat(char* str, unsigned int len, const char* first, ...)
{
	va_list args;
	return_val_if_fail(str != NULL && len > 0, NULL);

	va_start(args, first);
	strcat_arg(str, len, first, args);
	//va_end(args);

	return str;
}*/

#ifndef VPK_HAVE_GETTIMEOFDAY
int vpk_gettimeofday(struct timeval *tv, struct timezone *tz)
{
	struct _timeb tb;

	if (tv == NULL)
		return -1;

	_ftime(&tb);
	tv->tv_sec  = (long) tb.time;
	tv->tv_usec = ((int) tb.millitm) * 1000;
	return 0;
}
#endif

#if 0
int main(int argc, char* argv[])
{
	int cnt_num = 0;
	const char *ptr = "/dev/ttyUSB0  /dev/ttyUSB1  /dev/ttyUSB2  /dev/ttyUSB3";

	cnt_num = vpk_strcntstr(ptr, "ttyUSB");
	printf("cnt_num = %d\n", cnt_num);

	return 0;
}
#endif


