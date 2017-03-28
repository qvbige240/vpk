/**
 * History:
 * ================================================================
 * 2017-03-26 qing.zou created
 *
 */
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


