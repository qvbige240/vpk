/**
 * History:
 * ================================================================
 * 2017-05-01 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "vpk.h"
//#include "curl/curl.h"
#include "vpk_md5_test.h"
#if defined(_X86_)
#include "curl_md5.h"
#else
#define MD5_context			Vpk_MD5_context
#define Curl_MD5_init		Vpk_MD5_init
#define Curl_MD5_update		Vpk_MD5_update
#define Curl_MD5_final		Vpk_MD5_final
#define Curl_DIGEST_MD5		Vpk_DIGEST_MD5
#endif

#define MAX_DATA_CHUNK_SIZE		(1024+512)

int time_sub(struct timeval *result, struct timeval *prev, struct timeval *next)
{
	if (prev->tv_sec > next->tv_sec) return -1;
	if (prev->tv_sec == next->tv_sec && prev->tv_usec > next->tv_usec) return -1;

	result->tv_sec = next->tv_sec - prev->tv_sec;
	result->tv_usec = next->tv_usec - prev->tv_usec;
	if (result->tv_usec < 0)
	{
		result->tv_sec--;
		result->tv_usec += 1000000;
	}

	return 0;
}

/* same as curl */
int hash_data(Vpk_MD5_context* ctxt, char* filename)
{
	FILE* fp = 0;
	size_t ret = 0, offset = 0, size = MAX_DATA_CHUNK_SIZE;
	int file_size = 0;
	char data[MAX_DATA_CHUNK_SIZE+1] = {0};

	return_val_if_fail(filename != NULL, -1);
	fp = fopen(filename, "r");
	return_val_if_fail(fp != NULL, -1);
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	size = (offset + size) <= file_size ? size : file_size;
	LOG_D("file size: %d, unit size: %d", file_size, size);

	//int cnt = 0;
	while (offset < file_size) {
		fseek(fp, offset, SEEK_SET);
		//ret = fread(data, size, 1, fp);
		ret = fread(data, 1, size, fp);
		if (ret < 0) {
			LOG_E("fread file error.");
			break;
		}
		//*((char *)data + size) = '\0';

		Vpk_MD5_update(ctxt, (const unsigned char *) data, ret);

		//LOG_D("offset: %d, ret: %d, file size: %d, cnt: %d", offset, ret, file_size, cnt++);
		offset += ret;
	}

	fclose(fp);

	return 0;
}

int generate_md5(char* file, char* hash_hex)
{
	int i = 0;
	Vpk_MD5_context *ctxt;
	unsigned char digest[MD5_DIGEST_LEN];
	char ha_hex[2 * MD5_DIGEST_LEN + 1];
	ctxt = Vpk_MD5_init(Vpk_DIGEST_MD5);
	if(!ctxt)
		return -1;

	//Vpk_MD5_update(ctxt, (const unsigned char *) digest, MD5_DIGEST_LEN);
	hash_data(ctxt, file);
	Vpk_MD5_final(ctxt, digest);

	printf("md5: ");
	/* Convert calculated 16 octet hex into 32 bytes string */
	for(i = 0; i < MD5_DIGEST_LEN; i++) {
		snprintf(&ha_hex[2 * i], 3, "%02X", digest[i]);
		printf("%02x", digest[i]);
	}
	printf("\n");

	LOG_D("ha_hex: %s", ha_hex);
	strcpy(hash_hex, ha_hex);
	LOG_D("hash_hex: %s", hash_hex);

	return 0;
}

/* use curl */
int curl_hash_data(MD5_context* ctxt, char* filename)
{
	FILE* fp = 0;
	size_t ret = 0, offset = 0, size = MAX_DATA_CHUNK_SIZE;
	int file_size = 0;
	char data[MAX_DATA_CHUNK_SIZE+1] = {0};

	return_val_if_fail(filename != NULL, -1);
	fp = fopen(filename, "r");
	return_val_if_fail(fp != NULL, -1);
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	size = (offset + size) <= file_size ? size : file_size;
	LOG_D("file size: %d, unit size: %d", file_size, size);

	//int cnt = 0;
	while (offset < file_size) {
		fseek(fp, offset, SEEK_SET);
		//ret = fread(data, size, 1, fp);
		ret = fread(data, 1, size, fp);
		if (ret < 0) {
			LOG_E("fread file error.");
			break;
		}
		//*((char *)data + size) = '\0';

		Curl_MD5_update(ctxt, (const unsigned char *) data, ret);

		//LOG_D("offset: %d, ret: %d, file size: %d, cnt: %d", offset, ret, file_size, cnt++);
		offset += ret;
	}

	fclose(fp);

	return 0;
}

int curl_generate_md5(char* file, char* hash_hex)
{
	int i = 0;
	MD5_context *ctxt;
	unsigned char digest[MD5_DIGEST_LEN];
	char ha_hex[2 * MD5_DIGEST_LEN + 1];
	ctxt = Curl_MD5_init(Curl_DIGEST_MD5);
	if(!ctxt)
		return -1;

	//Curl_MD5_update(ctxt, (const unsigned char *) digest, MD5_DIGEST_LEN);
	curl_hash_data(ctxt, file);
	Curl_MD5_final(ctxt, digest);

	printf("md5: ");
	/* Convert calculated 16 octet hex into 32 bytes string */
	for(i = 0; i < MD5_DIGEST_LEN; i++) {
		snprintf(&ha_hex[2 * i], 3, "%02X", digest[i]);
		printf("%02x", digest[i]);
	}
	printf("\n");

	LOG_D("ha_hex: %s", ha_hex);
	strcpy(hash_hex, ha_hex);
	LOG_D("hash_hex: %s", hash_hex);

	return 0;
}

#include "vpk_md5.h"
/* use vpk */
int vpk_hash_data(vpk_md5ctx_t* ctx, char* filename)
{
	FILE* fp = NULL;
	size_t ret = 0, offset = 0, size = MAX_DATA_CHUNK_SIZE;
	int file_size = 0;
	char data[MAX_DATA_CHUNK_SIZE+1] = {0};
	return_val_if_fail(filename != NULL, -1);

	fp = fopen(filename, "r");
	return_val_if_fail(fp != NULL, -1);

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	size = (offset + size) <= file_size ? size : file_size;
	LOG_I("file size: %d, chunk size: %d", file_size, size);

	//int cnt = 0;
	while (offset < file_size) {
		fseek(fp, offset, SEEK_SET);
		//ret = fread(data, size, 1, fp);
		ret = fread(data, 1, size, fp);
		if (ret < 0) {
			LOG_E("file read error.");
			break;
		}
		//*((char *)data + size) = '\0';
		vpk_md5_update(ctx, (const unsigned char *) data, ret);

		//LOG_D("offset: %d, ret: %d, file size: %d, cnt: %d", offset, ret, file_size, cnt++);
		offset += ret;
	}

	fclose(fp);

	return 0;
}

int vpk_generate_md5(char* file, char* hash_hex)
{
	int i = 0;
	vpk_md5ctx_t *ctx;
	unsigned char digest[MD5_DIGEST_LEN];
	char ha_hex[2 * MD5_DIGEST_LEN + 1];
	ctx = vpk_md5_init(VPK_DIGEST_MD5);
	if (!ctx) return -1;

	vpk_hash_data(ctx, file);

	vpk_md5_final(ctx, digest);
	/* Convert calculated 16 octet hex into 32 bytes string */
	for(i = 0; i < MD5_DIGEST_LEN; i++) {
		snprintf(&ha_hex[2 * i], 3, "%02X", digest[i]);
		//printf("%02X", digest[i]);
	}
	LOG_D("MD5: %s", ha_hex);
	strcpy((char*)hash_hex, ha_hex);

	return 0;
}

int md5_main(int argc, char *argv[])
{
// 	int ret = 0;
// 
// 	vpk_system_init(argc, argv);
// 	vpk_logging_level_set("DEBUG");

	char ha_hex[64] = {0};

	char* localpath = "./FW966X.crdownload.mp4";
	if (argc > 1)
	{
		localpath = argv[1];
	}

	LOG_D("file = %s", localpath);

	//time_t pre_time = time(NULL);
	//generate_md5(localpath, ha_hex);
	//time_t end_time = time(NULL);
	//LOG_D("mvpk pre: %ld, end: %ld, used: %ld", pre_time, end_time, end_time-pre_time);


	//pre_time = time(NULL);
	//curl_generate_md5(localpath, ha_hex);
	//end_time = time(NULL);
	//LOG_D("curl pre: %ld, end: %ld, used: %ld", pre_time, end_time, end_time-pre_time);


	//pre_time = time(NULL);
	//generate_md5(localpath, ha_hex);
	//end_time = time(NULL);
	//LOG_D("mvpk pre: %ld, end: %ld, used: %ld", pre_time, end_time, end_time-pre_time);


	//pre_time = time(NULL);
	//curl_generate_md5(localpath, ha_hex);
	//end_time = time(NULL);
	//LOG_D("curl pre: %ld, end: %ld, used: %ld", pre_time, end_time, end_time-pre_time);


	struct timeval result, prev, next;
	gettimeofday(&prev, 0);
	generate_md5(localpath, ha_hex);
	gettimeofday(&next, 0);
	time_sub(&result, &prev, &next);
	LOG_D("cvpk: %d(s), %d(us)\n", result.tv_sec, result.tv_usec);

	gettimeofday(&prev, 0);
	curl_generate_md5(localpath, ha_hex);
	gettimeofday(&next, 0);
	time_sub(&result, &prev, &next);
	LOG_D("curl: %d(s), %d(us)\n", result.tv_sec, result.tv_usec);

	/* use vpk packaging md5 */
	gettimeofday(&prev, 0);
	vpk_generate_md5(localpath, ha_hex);
	gettimeofday(&next, 0);
	time_sub(&result, &prev, &next);
	LOG_D("mvpk: %d(s), %d(us)\n", result.tv_sec, result.tv_usec);

	gettimeofday(&prev, 0);
	vpk_md5_file_gen((unsigned char*)ha_hex, localpath);
	gettimeofday(&next, 0);
	time_sub(&result, &prev, &next);
	LOG_D("mvpk: %d(s), %d(us)  vpk_md5_file_gen \n", result.tv_sec, result.tv_usec);

	return 0;
}

