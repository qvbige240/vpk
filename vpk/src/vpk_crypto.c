/**
 * History:
 * ================================================================
 * 2018-9-19 qing.zou created
 *
 */
#include "openssl/aes.h"

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
