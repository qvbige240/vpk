/**
 * History:
 * ================================================================
 * 2018-9-19 qing.zou created
 *
 */

#include "openssl/aes.h"
#include "openssl/sha.h"

#include "vpk_b64.h"
#include "vpk_crypto.h"
#include "vpk_logging.h"

/* pkcs5padding */
static INLINE size_t padding5(unsigned char *src, size_t len)
{
	size_t i = 0;
	size_t pad_num = AES_BLOCK_SIZE - len % AES_BLOCK_SIZE;

	for (i = 0; i < pad_num; i++)
	{
		src[len+i] = pad_num;
	}

	return (len+pad_num);
}

static INLINE void padding5_reverse(unsigned char *src, size_t len)
{
	size_t pad_num = src[len-1];

	src[len-pad_num] = '\0';
}

static char* b64_encode_new(const char* src, size_t size, size_t *outsize)
{
	size_t len1 = vpk_b64_encode(src, size, NULL, 0);
	char* dst = (char*)VPK_MALLOC(len1 + 1);
	size_t out = vpk_b64_encode(src, size, dst, len1);
	dst[out] = '\0';
	if (outsize)
		*outsize = out;
	printf("in = %d, len1 = %d, out = %d\n", (int)size, (int)len1, (int)out);
	return dst;
}

static unsigned char* b64_decode_new(const char* src, size_t size, size_t *outsize)
{
	size_t dlen1 = vpk_b64_decode(src, size, NULL, 0);
	unsigned char* dst = (unsigned char*)calloc(dlen1 + 1, sizeof(unsigned char));
	size_t dlen2 = vpk_b64_decode(src, size, dst, dlen1);
	dst[dlen2] = '\0';
	if (outsize)
		*outsize = dlen2;
	printf("in = %d, dlen1 = %d, dlen2(out) = %d\n", (int)size, (int)dlen1, (int)dlen2);
	return dst;
}

char* vpk_encrypt_aes(unsigned char *key, char* data)
{
	AES_KEY aes;
	int i, length, len = 0;
	unsigned char* data_in = NULL;
	unsigned char* data_out = NULL;
	return_val_if_fail(key && data, NULL);

	//printf("Doing encrypt AES ECB.\n");
	if (AES_set_encrypt_key(key, 128, &aes) < 0) {
		printf("AES Key error\n");
		return NULL;
	}

	length = strlen(data);
	printf("length = %d\n", length);
	data_in = VPK_CALLOC(length+AES_BLOCK_SIZE, sizeof(unsigned char));
	data_out = VPK_CALLOC(length+AES_BLOCK_SIZE, sizeof(unsigned char));

	strcpy((char*)data_in, data);
	length = padding5(data_in, length);

	while (len < length) {
		AES_encrypt(data_in + len, data_out + len, &aes);
		len += AES_BLOCK_SIZE;
	}
	printf("encrypt out string(length = %d):\n %s\n", length, data_out);
	for (i = 0; i < length; i++) {
		printf(" 0x%02x", data_out[i]);
		if ((i % 8) == 7)
			printf("\n");
	}
	printf("\n");

	size_t size = 0;
	char* encode = b64_encode_new((const char *)data_out, length, &size);
	printf("b64 encode(%d): %s\n", (int)size, encode);
	if (data_in)
		VPK_FREE(data_in);
	if (data_out)
		VPK_FREE(data_out);

	return encode;
}

char* vpk_decrypt_aes(unsigned char *key, char* data)
{
	AES_KEY aes;
	int i, length, len = 0;
	unsigned char* data_in = NULL;
	unsigned char* data_out = NULL;

	//printf("Doing decryption AES ECB.\n");
	if (AES_set_decrypt_key(key, 128, &aes) < 0) {
		printf("AES Key error\n");
		return NULL;
	}

	size_t size = strlen(data);
	unsigned char* decode = b64_decode_new(data, size, (size_t*)&length);
	//printf("b64 data[%d]: %s\n", (int)size, data);
	//printf("b64 decode[%d]: %s\n", length, decode);
	//for (i = 0; i < length; i++) {
	//	printf(" 0x%02x", decode[i]);
	//	if ((i % 8) == 7)
	//		printf("\n");
	//}
	//printf("\n");

	if (length % AES_BLOCK_SIZE != 0) {
		printf("decode data format error!!!\n");
		if (decode) VPK_FREE(decode);
		return NULL;
	}
	data_in = decode;
	data_out = VPK_CALLOC(size, sizeof(unsigned char));

	while (len < length) {
		AES_decrypt(data_in + len, data_out + len, &aes);
		len += AES_BLOCK_SIZE;
	}
	padding5_reverse(data_out, length);
	//printf("decrypt string(len = %d):\n %s\n", (int)strlen((char*)data_out), data_out);
	if (decode)
		VPK_FREE(decode);

	return (char*)data_out;
}

/** sha1 **/
static INLINE unsigned char *sha1_gen(const unsigned char *d, size_t n, unsigned char *digest)
{
    if (1) {
        SHA1(d, n, digest);
    } else {
        SHA_CTX c;

        if (!SHA1_Init(&c))
            return NULL;
        SHA1_Update(&c, d, n);
        SHA1_Final(digest, &c);
        //OPENSSL_cleanse(&c, sizeof(c));
    }
    return digest;
}

#define DATA_CHUNK_SIZE (4 * 1024 * 1024)
static int hash_data_fp(FILE *input, unsigned char *digest)
{
    int ret = 0;
    FILE *fp = input;
    size_t n = 0;
    long file_size = 0, offset = 0, size = DATA_CHUNK_SIZE;
    unsigned char *data = NULL;
    //return_val_if_fail(filename != NULL, -1);
    //fp = fopen(filename, "r");
    return_val_if_fail(fp != NULL && digest, -1);

    data = calloc(1, DATA_CHUNK_SIZE + 1);

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    //size = (offset + size) <= file_size ? size : file_size;
    //LOG_I("file size: %ld, chunk size: %d", file_size, size);

    size_t nchunk = file_size / size + 1;

    LOG_I("file size: %ld, chunk size: %d, nchunk: %d", file_size, size, nchunk);
    if (!data) {
        ret = -1;
        LOG_E("memory calloc error.");
        goto end;
    }

    SHA_CTX c;
    if (!SHA1_Init(&c)) {
        ret = -1;
        LOG_E("SHA1_Init failed.");
        goto end;
    }

    //int cnt = 0;
    while (offset < file_size)
    {
        fseek(fp, offset, SEEK_SET);
        //n = fread(data, size, 1, fp);
        n = fread(data, 1, size, fp);
        if (n < 0)
        {
            LOG_E("file read error.");
            break;
        }
        //*((char *)data + size) = '\0';

        SHA1_Update(&c, data, n);

        //LOG_D("offset: %d, n: %d, file size: %d, cnt: %d", offset, n, file_size, cnt++);
        offset += n;
    }

    SHA1_Final(digest, &c);
    OPENSSL_cleanse(&c, sizeof(c));

end:
    if (data)
        free(data);
    //fclose(fp);

    return ret;
}

static int hash_data_fp_split4m(FILE *input, size_t split, unsigned char *digest)
{
    int ret = 0;
    FILE *fp = input;
    size_t n = 0;
    long file_size = 0, offset = 0;
    long size = split > DATA_CHUNK_SIZE ? DATA_CHUNK_SIZE : split;
    unsigned char *data = NULL;
    //return_val_if_fail(filename != NULL, -1);
    //fp = fopen(filename, "r");
    return_val_if_fail(fp != NULL && digest, -1);

    data = calloc(1, DATA_CHUNK_SIZE + 1);

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    //size = (offset + size) <= file_size ? size : file_size;
    //LOG_I("file size: %ld, chunk size: %d", file_size, size);

    size_t digest_len = 0;
    size_t nchunk = file_size / size + 1;
    //unsigned char digest[SHA_DIGEST_LENGTH];
    unsigned char *digest_data = calloc(1, nchunk * SHA_DIGEST_LENGTH + 1);

    LOG_I("file size: %ld, chunk size: %d, nchunk: %d", file_size, size, nchunk);
    if (!data || !digest_data)
    {
        ret = -1;
        LOG_E("memory calloc error.");
        goto end;
    }

    while (offset < file_size)
    {
        fseek(fp, offset, SEEK_SET);
        //n = fread(data, size, 1, fp);
        n = fread(data, 1, size, fp);
        if (n < 0)
        {
            LOG_E("file read error.");
            break;
        }
        //*((char *)data + size) = '\0';

        sha1_gen((const unsigned char *)data, n, digest);
        memcpy(digest_data + digest_len, digest, SHA_DIGEST_LENGTH);
        digest_len += SHA_DIGEST_LENGTH;

        //LOG_D("offset: %d, n: %d, file size: %d, cnt: %d", offset, n, file_size, cnt++);
        offset += n;
    }

    if (nchunk > 1)
        sha1_gen((const unsigned char *)digest_data, digest_len, digest);

end:
    if (digest_data)
        free(digest_data);

    if (data)
        free(data);
    //fclose(fp);

    return ret;
}

static INLINE int sha1_data_file(const char *filename, size_t split, unsigned char *digest)
{
    int ret = 0;
    FILE *fp = 0;
    return_val_if_fail(filename != NULL, -1);

    fp = fopen(filename, "r");

    if (split > 0)
        ret = hash_data_fp_split4m(fp, split, digest);
    else
        ret = hash_data_fp(fp, digest);

    fclose(fp);

    return ret;
}

static int sha1_gen_file(const char *filename, size_t split, unsigned char *hash, int convert_hex)
{
    int i = 0;
    unsigned char digest[SHA_DIGEST_LENGTH];
    char hex_string[2 * SHA_DIGEST_LENGTH + 1];

    sha1_data_file(filename, split, digest);

    if (convert_hex)
    {
        for (i = 0; i < SHA_DIGEST_LENGTH; i++)
        {
            snprintf(&hex_string[2 * i], 3, "%02X", digest[i]);
            //printf("%02X ", digest[i]);
        }
        //printf("\n");

        LOG_D("SHA1: %s", hex_string);
        strcpy((char *)hash, hex_string);
    }
    else
    {
        memcpy((char *)hash, digest, SHA_DIGEST_LENGTH);
    }

    return 0;
}

void vpk_sha1_file_gen(unsigned char *output, const char *path, int convert_hex)
{
    return_if_fail(output && path);
    sha1_gen_file(path, 0, output, convert_hex);
}

void vpk_sha1_split_file_gen(unsigned char *output, const char *path, size_t split, int convert_hex)
{
    return_if_fail(output && path);
    sha1_gen_file(path, split, output, convert_hex);
}

static int sha1_gen_block(const unsigned char *d, size_t n, unsigned char *hash_hex)
{
    int i = 0;
    unsigned char digest[SHA_DIGEST_LENGTH];
    char hex_string[2 * SHA_DIGEST_LENGTH + 1];

    sha1_gen(d, n, digest);

    for (i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        snprintf(&hex_string[2 * i], 3, "%02X", digest[i]);
        //printf("%02X", digest[i]);
    }
    LOG_D("SHA1: %s", hex_string);
    strcpy((char *)hash_hex, hex_string);

    return 0;
}

void vpk_sha1_data_gen(unsigned char *output, const unsigned char *data, size_t len)
{
    return_if_fail(output && data && len > 0);

    sha1_gen_block(data, len, output);
}
