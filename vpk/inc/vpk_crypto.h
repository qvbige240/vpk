/**
 * History:
 * ================================================================
 * 2018-9-19 qing.zou created
 *
 */
#ifndef VPK_CRYPTO_H
#define VPK_CRYPTO_H

#include "vpk_typedef.h"

TIMA_BEGIN_DELS

/**
 * Encrypt data use AES algorithm and base64 encode.
 *
 * @param key	 The secret key for encryption.
 * @param data   The target data to encrypt.
 *
 * @return Encrypted string, NULL on failure.
 */
char* vpk_encrypt_aes(unsigned char *key, char* data);

/**
 * Decrypt data use base64 decode and AES algorithm.
 *
 * @param key	 The secret key for decryption.
 * @param data   The target data to decrypt.
 *
 * @return Decrypted string, NULL on failure.
 */
char* vpk_decrypt_aes(unsigned char *key, char* data);

/**
 * Generate sha1 for file.
 *
 * @param output	The sha1 hex string buffer
 * @param path		The target file
 * @param convert_hex	whether convert to hex string
 *
 */
void vpk_sha1_file_gen(unsigned char *output, const char *path, int convert_hex);

void vpk_sha1_split_file_gen(unsigned char *output, const char *path, size_t split, int convert_hex);

void vpk_sha1_data_gen(unsigned char *output, const unsigned char *data, size_t len);

TIMA_END_DELS

#endif //VPK_CRYPTO_H
