/**
 * History:
 * ================================================================
 * 2018-09-17 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>

#include "vpk.h"
#include "vpk_b64.h"

#if defined(_X86_)
#else
#endif

#include <errno.h>

#include "openssl/aes.h"
//#include <openssl/pkcs7.h>

#if 0
static unsigned char cbc_key[8] =
{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
//static unsigned char cbc_iv[8] =
//{ 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10 };
static unsigned char cbc_iv[8] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* static char cbc_data[40]="7654321 Now is the time for \0001"; */
//static unsigned char cbc_data[40] = {
//	0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x20,
//	0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74,
//	0x68, 0x65, 0x20, 0x74, 0x69, 0x6D, 0x65, 0x20,
//	0x66, 0x6F, 0x72, 0x20, 0x00, 0x31, 0x00, 0x00,
//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//};

//
//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//	0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38,
//static unsigned char cbc_data[100]="7654321 Now is the time for worl dsssssssssss i sfssdflovesdfdff fanxiaoqing";
//static unsigned char cbc_data[100]="tima0tg5device012345678912345";
//static unsigned char cbc_data[100]="timastar";
//static unsigned char cbc_ok[32] = {
//	0xcc, 0xd1, 0x73, 0xff, 0xab, 0x20, 0x39, 0xf4,
//	0xac, 0xd8, 0xae, 0xfd, 0xdf, 0xd8, 0xa1, 0xeb,
//	0x46, 0x8e, 0x91, 0x15, 0x78, 0x88, 0xba, 0x68,
//	0x1d, 0x26, 0x93, 0x97, 0xf7, 0xfe, 0x62, 0xb4
//};



static unsigned char aes_key[AES_BLOCK_SIZE] = {
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
	0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
};
//static unsigned char iv[AES_BLOCK_SIZE] = { 
//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
//};
static int test_aes_ncbc(unsigned char *key, char* data)
{
	AES_KEY aes;
	unsigned char iv[AES_BLOCK_SIZE] = {0};
	//unsigned char key[AES_BLOCK_SIZE];
	unsigned char* cbc_in = NULL;
	unsigned char* cbc_out = NULL;
	unsigned char* encrypt_string = NULL;
	unsigned char* decrypt_string = NULL;

	char* cbc_data = data;

	int j, err = 0;

	printf("Doing aes cbc\n");
	if (AES_set_encrypt_key(key, 128, &aes) < 0) {
		printf("AES Key error %d\n", j);
		err = 1;
	}

	//memcpy(iv3, cbc_iv, sizeof(cbc_iv));

	int len = strlen((char *)cbc_data) + 1;
	if (len % AES_BLOCK_SIZE != 0)
		len = (len / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
	printf("len = %d\n", len);
	encrypt_string = calloc(len, sizeof(unsigned char));
	decrypt_string = calloc(len, sizeof(unsigned char));
	cbc_in = calloc(len, sizeof(unsigned char));
	cbc_out = calloc(len, sizeof(unsigned char));
	//unsigned char *data = malloc(len);
	strcpy((char *)cbc_in, (char *)cbc_data);
	printf("before data(len = %d): %s\n", (int)strlen((char *)cbc_in), cbc_in);
	AES_cbc_encrypt(cbc_in, encrypt_string, len, &aes, iv, AES_ENCRYPT);

	//DES_ncbc_encrypt(data, cbc_out, len, &ks, &iv3, DES_ENCRYPT);
	printf("encrypt string(len = %d):\n %s\n", (int)strlen((char *)encrypt_string), encrypt_string);
	for (j = 0; j < len; j++)
	{
		printf(" 0x%02x", encrypt_string[j]);
		if ((j % 8) == 7)
			printf("\n");
	}
	printf("\n");

	char* encode = memory_encode((const char *)encrypt_string, len);
	//printf("encode: %s\n", encode);
	char* decode = string_decode(encode);
	printf("encode[%d]: %s\ndecode[%d]: %s\n", (int)strlen(encode), encode, (int)strlen(decode), decode);

	memset(iv, 0, AES_BLOCK_SIZE);
	if (AES_set_decrypt_key(key, 128, &aes) < 0) {
		printf("AES Key error %d\n", j);
		err = 1;
	}

	AES_cbc_encrypt(encrypt_string, decrypt_string, len, &aes, iv, AES_DECRYPT);

	//memcpy(iv3, cbc_iv, sizeof(cbc_iv));
	//DES_ncbc_encrypt(cbc_out, cbc_in, len, &ks, &iv3, DES_DECRYPT);
	//DES_ncbc_encrypt((const unsigned char *)decode, cbc_in, len, &ks, &iv3, DES_DECRYPT);
	if (memcmp(decrypt_string, cbc_data, strlen((char *)cbc_data)) != 0) {
		printf("cbc_encrypt decrypt error\n");
		err = 1;
	}
	printf("decrypt string (len = %d): %s\n", (int)strlen((char *)decrypt_string), decrypt_string);

	if (err)
		printf("have error\n");

	printf("\n");

	return 0;
}


static int test_aes_decode(unsigned char *key, char* encode)
{
	AES_KEY aes;
	unsigned char iv[AES_BLOCK_SIZE] = {0};
	unsigned char* decrypt_string[256] = {0};

	int err = 0;

	printf("Doing aes cbc\n");
	//if (AES_set_encrypt_key(key, 128, &aes) < 0) {
	//	printf("AES Key error %d\n", j);
	//	err = 1;
	//}

	////memcpy(iv3, cbc_iv, sizeof(cbc_iv));

	//int len = strlen((char *)cbc_data) + 1;
	//if (len % AES_BLOCK_SIZE != 0)
	//	len = (len / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
	//printf("len = %d\n", len);
	//encrypt_string = calloc(len, sizeof(unsigned char));
	
	//decrypt_string = calloc(len, sizeof(unsigned char));

	char* decode = string_decode(encode);
	printf("b64 encode[%d]: %s\nb64 decode[%d]: %s\n", (int)strlen(encode), encode, (int)strlen(decode), decode);

	memset(iv, 0, AES_BLOCK_SIZE);
	if (AES_set_decrypt_key(key, 128, &aes) < 0) {
		printf("AES Key error\n");
		err = 1;
	}

	AES_cbc_encrypt(decode, decrypt_string, 256, &aes, iv, AES_DECRYPT);
	printf("decrypt string (len = %d): %s\n", (int)strlen((char *)decrypt_string), decrypt_string);

	printf("\n");

	return 0;
}
#endif

static size_t padding5(unsigned char *src, size_t len)
{
	size_t i = 0;
	size_t pad_num = AES_BLOCK_SIZE - len % AES_BLOCK_SIZE;
	
	for (i = 0; i < pad_num; i++)
	{
		src[len+i] = pad_num;
	}

	return len+pad_num;
}
static void padding5_reverse(unsigned char *src, size_t len)
{
	size_t pad_num = src[len-1];

	src[len-pad_num] = '\0';
}

static char* memory_encode_b64(const char* src, size_t size, size_t *outsize)
{
	size_t len = vpk_b64_encode(src, size, NULL, 0);
	char* dst = (char*)malloc(len + 1);
	size_t len1 = vpk_b64_encode(src, size, dst, len);
	dst[len1] = '\0';
	if (outsize)
		*outsize = len1;
	printf("len = %d, len1 = %d\n", (int)len, (int)len1);
	return dst;
}
static unsigned char* memory_decode_b64(const char* src, size_t size, size_t *outsize)
{
	size_t dlen1 = vpk_b64_decode(src, size, NULL, 0);
	unsigned char* dst = (unsigned char*)calloc(dlen1 + 1, sizeof(unsigned char));
	size_t dlen2 = vpk_b64_decode((char const*)src, size, dst, dlen1);
	dst[dlen2] = '\0';
	if (outsize)
		*outsize = dlen2;
	printf("len = %d, dlen1 = %d, dlen2 = %d\n", (int)size, (int)dlen1, (int)dlen2);
	return dst;
}

static int aes_ecb_encrypt(unsigned char *key, char* data)
{
	int j, length, len = 0;
	AES_KEY aes;
	unsigned char* data_in = NULL;
	unsigned char* data_out = NULL;

	printf("Doing AES ECB encrypt\n");
	if (AES_set_encrypt_key(key, 128, &aes) < 0) {
		printf("AES Key error\n");
		return -1;
	}

	length = strlen((char *)data);
	//length = (length + AES_BLOCK_SIZE-1) & ~(AES_BLOCK_SIZE-1);
	//if (length % AES_BLOCK_SIZE != 0)
	//	length = (length / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
	printf("length = %d\n", length);
	data_in = calloc(length+AES_BLOCK_SIZE, sizeof(unsigned char));
	data_out = calloc(length+AES_BLOCK_SIZE, sizeof(unsigned char));

	strcpy((char*)data_in, data);
	length = padding5(data_in, length);

	while (len < length) {
		AES_encrypt(data_in + len, data_out + len, &aes);
		len += AES_BLOCK_SIZE;
	}
	printf("encrypt out string(length = %d):\n %s\n", length, data_out);
	for (j = 0; j < length; j++) {
		printf(" 0x%02x", data_out[j]);
		if ((j % 8) == 7)
			printf("\n");
	}
	printf("\n");

	size_t size = 0;
	char* encode = memory_encode_b64((const char *)data_out, length, &size);
	printf("b64 encode(%d): %s\n", (int)size, encode);
	return 0;
}

static int aes_ecb_decrypt(unsigned char *key, char* data)
{
	int j, length, len = 0;
	AES_KEY aes;
	unsigned char* data_in = NULL;
	unsigned char* data_out = NULL;

	printf("Doing AES ECB decrypt\n");
	if (AES_set_decrypt_key(key, 128, &aes) < 0) {
		printf("AES Key error\n");
		return -1;
	}

	printf("b64 data[%d]: %s\n", (int)strlen(data), data);
	unsigned char* decode = memory_decode_b64(data, strlen(data), (size_t*)&length);
	printf("b64 decode[%d]: %s\n", length, decode);
	for (j = 0; j < length; j++) {
		printf(" 0x%02x", decode[j]);
		if ((j % 8) == 7)
			printf("\n");
	}
	printf("\n");

	if (length % AES_BLOCK_SIZE != 0) {
		printf("decode data format error!!!\n");
		return -1;
	}
	data_in = decode;
	data_out = calloc(strlen(data), sizeof(unsigned char));

	while (len < length) {
		AES_decrypt(data_in + len, data_out + len, &aes);
		len += AES_BLOCK_SIZE;
	}
	padding5_reverse(data_out, length);
	printf("decrypt string(len = %d):\n %s\n", (int)strlen((char*)data_out), data_out);
	return 0;
}


int aes_main(int argc, char *argv[])
{
 	//int ret = 0;
 
 	//vpk_system_init(argc, argv);
 	//vpk_logging_level_set("DEBUG");

	char* type = "de-ecb";
	char* text = "timastar012345678912345";
	char* key = "952518277c3fc80c";

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
			case 'f':
				text = optarg;
				break;
			default:
				break;
		}
	}

	LOG_D("type = %s, text = %s", type, text);

	double elapsed;
	struct timeval result, prev, next;	

	gettimeofday(&prev, 0);

	if (strcasecmp(type, "encode") == 0) {
		//test_aes_ncbc(key, text);
	} else if (strcasecmp(type, "decode") == 0) {
		//test_aes_decode(key, text);
	} else if (strcasecmp(type, "en-ecb") == 0) {	
		/** ./test -d aes -t en-ecb -f timastartest **/
		aes_ecb_encrypt((unsigned char*)key, text);
		printf("\n\n");
		char* str = vpk_encrypt_aes((unsigned char*)key, text);
		free(str);
	} else if (strcasecmp(type, "de-ecb") == 0) {
		/** ./test -d aes -t de-ecb -f hluI5fxFOTRZ8rS5cfreEg== **/
		aes_ecb_decrypt((unsigned char*)key, text);
		printf("\n\n");
		char* str = vpk_decrypt_aes((unsigned char*)key, text);
		free(str);
	} else {
		//test_aes_ncbc(key, text);
	}

	gettimeofday(&next, 0);
	vpk_timersub(&next, &prev, &result);
	//time_sub(&result, &prev, &next);
	elapsed = result.tv_sec + (result.tv_usec / 1.0e6);
	LOG_D("vpk time elapsed: %.6f, %d(s) %d(us) \n", elapsed, result.tv_sec, result.tv_usec);

	return 0;
}
