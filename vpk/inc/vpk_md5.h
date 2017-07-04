/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2010, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

#ifndef VPK_MD5_H
#define VPK_MD5_H

#include "vpk_typedef.h"

#define MD5_DIGEST_LEN  16

typedef void (*VpkMD5Init)(void *context);
typedef void (*VpkMD5Update)(void *context, const unsigned char *data, unsigned int len);
typedef void (*VpkMD5Final)(unsigned char *result, void *context);

typedef struct {
  VpkMD5Init		md5_init;		/* Initialize context procedure */
  VpkMD5Update		md5_update;		/* Update context with data */
  VpkMD5Final		md5_final;		/* Get final result procedure */
  unsigned int		md5_ctxtsize;	/* Context structure size */
  unsigned int		md5_resultlen;	/* Result length (bytes) */
} vpk_md5_t;

typedef struct {
  const vpk_md5_t	*md5_hash;		/* Hash function definition */
  void				*md5_ctx;		/* Hash function context */
} vpk_md5ctx_t;

/** The md5 init/update/final method struct var **/
VPKAPI const vpk_md5_t VPK_DIGEST_MD5[1];

VPKAPI vpk_md5ctx_t * vpk_md5_init(const vpk_md5_t *params);
VPKAPI int vpk_md5_update(vpk_md5ctx_t *context, const unsigned char *data, unsigned int len);
VPKAPI int vpk_md5_final(vpk_md5ctx_t *context, unsigned char *result);

/**
 * Generate md5 from buffer.
 *
 * @param output	The md5 hex data buffer
 * @param input		The target buffer
 *
 */
VPKAPI void vpk_md5_bgen(unsigned char *output, const unsigned char *input);

/**
 * Generate md5 from file.
 *
 * @param output	The md5 hex data buffer
 * @param input		The target file pointer
 *
 */
VPKAPI void vpk_md5_fgen(unsigned char *output, FILE *input);

/**
 * Generate md5 from file.
 *
 * @param output	The md5 hex data buffer
 * @param path		The target file
 *
 */
VPKAPI void vpk_md5_file_gen(unsigned char *output, const char *path);

#endif /* VPK_MD5_H */
