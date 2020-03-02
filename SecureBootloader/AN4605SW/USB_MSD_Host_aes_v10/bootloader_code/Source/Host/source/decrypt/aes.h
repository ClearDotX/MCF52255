#include "types.h"
#ifndef _AES_H_
#define _AES_H_

#define AES128                    128
#define AES192                    192
#define AES256                    256
#define AES_BLOCK_LENGTH          16

/*after encrypt/decrypt we must have original input: symmetric crypto algorithm*/
/*AES in CBC mode*/

uint_8
aes_main(uint_8* dataptr, uint_32 *datalength);

/*
 * encryptAES: AES encryption function
 *
 * Parameters:
 *   [in] key: key of 8 bytes
 *   [in] mode: 128, 192 or 256 AES mode
 *   [in] input_data: pointer to in data
 *   [out] output_data: pointer to out data
 *   [in] data_length: number of bytes of input data. Must be divisible by 8 (DES block)
 *   [in] init_vector: init_vector to use during xor
 * Return:
 *   0 if OK, otherwise error
 *
 */
unsigned char
encryptAES(unsigned char *key, unsigned short mode, unsigned char *input_data, unsigned char *output_data, unsigned short data_length, unsigned char *init_vector);

/*
 * decryptAES: AES decryption function
 *
 * Parameters:
 *   [in] key: key of 8 bytes
 *   [in] mode: 128, 192 or 256 AES mode
 *   [in] input_data: pointer to in data
 *   [out] output_data: pointer to out data
 *   [in] data_length: number of bytes of input data. Must be divisible by 8 (DES block)
 *   [in] init_vector: init_vector to use during xor
 * Return:
 *   0 if OK, otherwise error
 *
 */

void mymemcpy(uint_8* desc, uint_8* source, uint_32 length);
unsigned char decryptAES(unsigned char *key, \
                         unsigned short mode, \
                         unsigned char *input_data, \
                         unsigned char *output_data, \
                         unsigned short data_length, \
                         unsigned char *init_vector);

#endif
