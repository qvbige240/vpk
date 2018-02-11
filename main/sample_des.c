/**
 * History:
 * ================================================================
 * 2017-06-12 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>

#include "vpk.h"

#if defined(_X86_)
#else
#endif

#include <errno.h>

#include "openssl/des.h"
//#include <openssl/pkcs7.h>

//static int encrypt_data(const char *_key, const char *_vt, char *_raw_ptr,size_t _raw_size, char **_dst_buf, size_t *_dst_size)
//{
//	DES_key_schedule schedule;
//	unsigned char key1[8];
//	des_cblock *iv3;
//	int pading ;
//	size_t i, vt_size ;
//	char *mid_buf;
//	memset( key1, 0, 8);
//	memcpy( key1, _key, 8 );
//	DES_set_key_unchecked( (const_DES_cblock*)&key1, &schedule);
//	vt_size = strlen( _vt );
//	iv3 = (des_cblock *)malloc(vt_size * sizeof(unsigned char));
//	memcpy(iv3,_vt,vt_size);
//	pading = 8 - (_raw_size % 8);
//	*_dst_size = _raw_size + pading;
//	mid_buf = (char*)malloc(*_dst_size);
//	memcpy(mid_buf,_raw_ptr,_raw_size );
//	for (i = _raw_size ; i < *_dst_size; i++ ) {
//		mid_buf[i] = pading;
//	}
//	*_dst_buf = (char*)malloc(*_dst_size);
//	DES_cbc_encrypt( (const unsigned char*)mid_buf, (unsigned char *)*_dst_buf, *_dst_size, &schedule, iv3, DES_ENCRYPT);
//	free(iv3);
//	free(mid_buf);
//	return 1;
//}
//
//static int decrypt_data(const char *_key, const char *_vt,char *_raw_ptr,size_t _raw_size , char **_dst_buf, size_t *_dst_size ) {
//	DES_key_schedule schedule;
//	unsigned char key1[8];
//	des_cblock *iv3;
//	int pading ;
//	size_t i, vt_size ;
//	char *mid_buf;
//	memset( key1, 0, 8);
//	memcpy( key1, _key, 8);
//	DES_set_key_unchecked( (const_DES_cblock*)&key1, &schedule);
//	vt_size = strlen( _vt );
//	iv3 = (des_cblock *)malloc(vt_size * sizeof(unsigned char));
//	memcpy(iv3,_vt,vt_size);
//	*_dst_buf = (char*)malloc(_raw_size);
//	DES_cbc_encrypt( (const unsigned char*)_raw_ptr, *_dst_buf, _raw_size, &schedule, iv3, DES_DECRYPT);
//	free(iv3);
//	return 1;
//}
//
//static int test_des_1()
//{
//	char *_key = "jkl;!@#$";
//	char *_vt = "asdf!@#$";
//	char *_raw_ptr ;
//	size_t _raw_size;
//	char *_dst_buf;
//	size_t _dst_size;
//	char *_final_buf;
//	size_t _final_size;
//	_raw_ptr = (char *)malloc(sizeof(char)*5);
//	memcpy(_raw_ptr, "hello", 5);
//	_raw_size = 5;
//	encrypt_data(_key, _vt, _raw_ptr, _raw_size, &_dst_buf, &_dst_size) ;
//	decrypt_data(_key, _vt, _dst_buf, _dst_size, &_final_buf, &_final_size );
//	printf( "final: %s\n", _final_buf );
//	free(_dst_buf);
//
//	printf("\n\n\n");
//
//	return 0;
//}

static unsigned char cbc_key[8] =
{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
//static unsigned char cbc_iv[8] =
//{ 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10 };
static unsigned char cbc_iv[8] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* static char cbc_data[40]="7654321 Now is the time for \0001"; */
static unsigned char cbc_data[40] = {
	0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x20,
	0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74,
	0x68, 0x65, 0x20, 0x74, 0x69, 0x6D, 0x65, 0x20,
	0x66, 0x6F, 0x72, 0x20, 0x00, 0x31, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static unsigned char cbc_ok[32] = {
	0xcc, 0xd1, 0x73, 0xff, 0xab, 0x20, 0x39, 0xf4,
	0xac, 0xd8, 0xae, 0xfd, 0xdf, 0xd8, 0xa1, 0xeb,
	0x46, 0x8e, 0x91, 0x15, 0x78, 0x88, 0xba, 0x68,
	0x1d, 0x26, 0x93, 0x97, 0xf7, 0xfe, 0x62, 0xb4
};

static int test_des_ncbc(const_DES_cblock *key)
{
	DES_cblock iv3;
	DES_key_schedule ks;
	unsigned char cbc_in[40];
	unsigned char cbc_out[40];

	int j, err = 0;

	printf("Doing cbc\n");
	if ((j = DES_set_key_checked(key, &ks)) != 0) {
		printf("Key error %d\n", j);
		err = 1;
	}

	//DES_set_key_unchecked(&cbc_key, &ks);
	memset(cbc_out, 0, 40);
	memset(cbc_in, 0, 40);
	memcpy(iv3, cbc_iv, sizeof(cbc_iv));
	printf("before cbc_data: %s\n", cbc_data);
	DES_ncbc_encrypt(cbc_data, cbc_out, strlen((char *)cbc_data) + 1, &ks, &iv3, DES_ENCRYPT);
	if (memcmp(cbc_out, cbc_ok, 32) != 0) {
		printf("cbc_encrypt encrypt error\n");
		err = 1;
	}
	printf("encrypt cbc_out: %s\n", cbc_out);

	memcpy(iv3, cbc_iv, sizeof(cbc_iv));
	DES_ncbc_encrypt(cbc_out, cbc_in, strlen((char *)cbc_data) + 1, &ks, &iv3, DES_DECRYPT);
	if (memcmp(cbc_in, cbc_data, strlen((char *)cbc_data)) != 0) {
		printf("cbc_encrypt decrypt error\n");
		err = 1;
	}
	printf("decrypt cbc_in: %s\n", cbc_in);

	if (err)
		printf("have error\n");

	printf("\n");

	return 0;
}


static int test_des_2(void)
{
	return test_des_ncbc(&cbc_key);
}

void key_gen(void)
{
	//DES_cblock key;
	unsigned char key[8];
	const char *str = "king1";
	DES_string_to_key(str, &key);

	printf("key: ");
	int i = 0;
	for (i = 0; i < sizeof(key); i++)
	{
		printf("0x%02x ", key[i]);
	}
	printf("\n");

	DES_key_schedule ks;
	if ((i = DES_set_key_checked(&key, &ks)) != 0) {
		printf("Key error %d\n", i);
		//err = 1;
	}

	test_des_ncbc(&key);
}

int encrypt_main(int argc, char *argv[])
{
 	//int ret = 0;
 
 	//vpk_system_init(argc, argv);
 	//vpk_logging_level_set("DEBUG");

	char* type = "cbc";
	//if (argc > 1)
	//{
	//	type = argv[1];
	//}

	int o;
	static const struct option long_options[] = {
		{ "help",			no_argument,			NULL, 'h' },
		{ "version",		no_argument,			NULL, 'V' },
		{ "sample",			required_argument,		NULL, 'd' },
		{ "type",			required_argument,		NULL, 't' },
		{ "keycode",		required_argument,		NULL, 'k' },
		{ "file",			required_argument,		NULL, 'f' },
		{ "url",			required_argument,		NULL, 'u' },
		{ "log",			optional_argument,		NULL, 'l' },
		{ NULL, 0, NULL, 0 }
	};

	optind = 1;
	//LOG_I("22 optind = %d, argc = %d", optind, argc);
	while ((o = getopt_long(argc, argv, "hVd:t:k:f:u:l", long_options, NULL)) >= 0) {
		//printf("opt = %c\n", o);  
		//printf("optarg = %s\n", optarg);  
		//printf("optind = %d\n", optind);  
		//printf("argv[optind - 1] = %s\n",  argv[optind - 1]);
		switch(o) {
			case 't':
				type = optarg;
				break;
			default:
				break;
		}
	}

	LOG_D("type = %s", type);

	double elapsed;
	struct timeval result, prev, next;	

	gettimeofday(&prev, 0);

	//test_des_1();

	test_des_2();

	key_gen();


	gettimeofday(&next, 0);
	vpk_timersub(&next, &prev, &result);
	//time_sub(&result, &prev, &next);
	elapsed = result.tv_sec + (result.tv_usec / 1.0e6);
	LOG_D("vpk time elapsed: %.6f, %d(s) %d(us) \n", elapsed, result.tv_sec, result.tv_usec);

	return 0;
}
