#include "derivative.h" 
//#include "types.h"
#include "stdlib.h"
#include "cau_api.h"
#include "aes.h"


extern void * (memcpy) (void * dst, const void * src, size_t len);


//#include "crypto_common.h"



 /*16 bytes key: "ultrapassword123"*/

 
/*after encrypt/decrypt we must have original input: symmetric crypto algorithm*/
/*AES in CBC mode*/
uint_8
aes_main(uint_8* dataptr, uint_32 *datalength)
{
	static unsigned char   ive[AES_BLOCK_LENGTH] = { 0, 0, 0, 0, 0, 0, 0, 0,
	                                            0, 0, 0, 0, 0, 0, 0, 0}; 
	
	unsigned char   key128[AES128/8] = { 0xf4, 0x0f, 0xd1, 0x79, 0x12, 0x54, 0xb7, 0xf2,
	                                      0x2b, 0xd8, 0xcd, 0xd1, 0x05, 0xaa, 0x1d, 0x7e};

	
	unsigned char aes128_encryptedtxt[] = {0x0F, 0xF0, 0x0C, 0xF9, 0x10, 0x44, 0x97, 0x3D,
	                                           0x67, 0x2B, 0x4B, 0x7F, 0xCA, 0x24, 0x20, 0x06,
	                                           0x4F, 0xD2, 0xC1, 0xDF, 0x1A, 0xB7, 0x55, 0x12,
	                                           0x8E, 0xF8, 0x8E, 0x7F, 0x5A, 0x6D, 0x19, 0x2B,
	                                           0x73, 0x55, 0xD3, 0x6F, 0x74, 0x23, 0x59, 0x40,
	                                           0x4C, 0x01, 0x2B, 0xAB, 0x6B, 0xB1, 0xF1, 0x92,
	                                           0x26, 0x3C, 0xD3, 0x71, 0xE0, 0x1A, 0x6E, 0x4D,
	                                           0xB7, 0xA3, 0x19, 0xA5, 0x06, 0x5A, 0xF2, 0x89 };
	                                  
	    /*initialization vector: 16 bytes: "mysecretpassword"*/
	static unsigned char   testive[AES_BLOCK_LENGTH] = { 0x83, 0x06, 0xDE, 0x25, 0xCD, 0xE8, 0xA6, 0x6F,
	                                              0x26, 0x16, 0x88, 0x60, 0x4A, 0xD5, 0x58, 0x92 };
	
	    /*result holding the example max length array*/
	unsigned char   testoutput[sizeof(aes128_encryptedtxt)];	
	static uint_8 ivInit = 0;
	
	uint_8* encrypteddataptr;
  
//     printf("aes_main\n");
    if (ivInit)					// iv has been initialized												
    {
    	encrypteddataptr = dataptr;
    }
    else
    {
      	memcpy(ive,dataptr,AES_BLOCK_LENGTH);
        	*datalength -= AES_BLOCK_LENGTH;
        	encrypteddataptr = dataptr + AES_BLOCK_LENGTH;
//        	printf("ive0: %2x\n",ive[0]);
        	ivInit = 1;
    }
    
    
    return (decryptAES(key128,AES128,encrypteddataptr,dataptr,*datalength,ive));
//    printf("encrypteddata0: %2x decrypteddata0: %2x\n",*encrypteddataptr, *dataptr);
      
//    printf("AES128 test completed\n");
   
}

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
encryptAES(unsigned char *key, unsigned short mode, unsigned char *input_data, unsigned char *output_data, unsigned short data_length, unsigned char *init_vector)
{
	
    unsigned char i;
    unsigned short blocks;
    unsigned short rounds;
    unsigned char temp_block[AES_BLOCK_LENGTH];
    unsigned char temp_iv[AES_BLOCK_LENGTH];
    /*
     * AES128 needs 44 longwords for expansion
     * AES192 needs 52 longwords for expansion
     * AES256 needs 60 longwords for expansion
     *    Reserving 60 longwords as the max space
     */
    unsigned char key_expansion[60*4];
    
    /*validate NULL for key, input_data, output_data or init_vector*/
    if( (key == NULL) || (input_data == NULL) || (output_data == NULL) || (init_vector == NULL) )
       return (unsigned char)-1;/*wrong pointer*/
	
    /*validate AES mode*/
    if( (mode != AES128) & (mode != AES192) & (mode != AES256) )
       return (unsigned char)-1;/*wrong AES mode*/
    
    /*validate data length*/
    if( data_length % AES_BLOCK_LENGTH )
       return (unsigned char)-1;/*wrong length*/

    /*calculate number of AES rounds*/
    if(mode == AES128)
       rounds = 10;
    else if(mode == AES192)
       rounds = 12;
    else/*AES256*/
       rounds = 14;
    
    /*expand AES key*/
    cau_aes_set_key(key, mode, key_expansion);
    
    /*execute AES in CBC mode*/
    /*http://en.wikipedia.org/wiki/Block_cipher_modes_of_operation*/
    
    /*get number of blocks*/
    blocks = data_length/AES_BLOCK_LENGTH;
    
    /*copy init vector to temp storage*/
    memcpy((void*)temp_iv,(void*)init_vector,AES_BLOCK_LENGTH);
    
    do
    {
       /*copy to temp storage*/
       memcpy((void*)temp_block,(void*)input_data,AES_BLOCK_LENGTH);
       
       /*xor for CBC*/
       for (i = 0; i < AES_BLOCK_LENGTH; i++)
           temp_block[i] ^= temp_iv[i];
           
       /*FSL: single DES round*/    
       cau_aes_encrypt(temp_block, key_expansion, rounds, output_data);
       
       /*update init_vector for next 3DES round*/
       memcpy((void*)temp_iv,(void*)output_data,AES_BLOCK_LENGTH);
       
       /*adjust pointers for next 3DES round*/
       input_data += AES_BLOCK_LENGTH;
       output_data += AES_BLOCK_LENGTH;  
    	
    }while(--blocks);
    
    return 0;//OK
}

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
unsigned char
decryptAES(unsigned char *key, unsigned short mode, unsigned char *input_data, unsigned char *output_data, unsigned short data_length, unsigned char *init_vector)
{
	
    unsigned char i;
    unsigned short blocks;
    unsigned short rounds;
    unsigned char temp_block[AES_BLOCK_LENGTH];
    unsigned char temp_iv[AES_BLOCK_LENGTH];
    /*
     * AES128 needs 44 longwords for expansion
     * AES192 needs 52 longwords for expansion
     * AES256 needs 60 longwords for expansion
     *    Reserving 60 longwords as the max space
     */
    unsigned char key_expansion[60*4];
	
    /*validate NULL for key, input_data, output_data or init_vector*/
    if( (key == NULL) || (input_data == NULL) || (output_data == NULL) || (init_vector == NULL) )
       return (unsigned char)-1;/*wrong pointer*/
	
    /*validate AES mode*/
    if( (mode != AES128) & (mode != AES192) & (mode != AES256) )
       return (unsigned char)-1;/*wrong AES mode*/
    
    /*validate data length*/
    if( data_length % AES_BLOCK_LENGTH )
       return (unsigned char)-1;/*wrong length*/

    /*calculate number of AES rounds*/
    if(mode == AES128)
       rounds = 10;
    else if(mode == AES192)
       rounds = 12;
    else/*AES256*/
       rounds = 14;
    
    /*expand AES key*/
//    mmcau_aes_set_key(key, mode, key_expansion);
    cau_aes_set_key(key, mode, key_expansion);    
    
    /*execute AES in CBC mode*/
    /*http://en.wikipedia.org/wiki/Block_cipher_modes_of_operation*/
    
    /*get number of blocks*/
    blocks = data_length/AES_BLOCK_LENGTH;
    
    /*copy init vector to temp storage*/
    memcpy((void*)temp_iv,(void*)init_vector,AES_BLOCK_LENGTH);
    
    do
    {
       /*copy to temp storage*/
       memcpy((void*)temp_block,(void*)input_data,AES_BLOCK_LENGTH);
           
       /*FSL: single AES round*/    
       cau_aes_decrypt(input_data, key_expansion, rounds, output_data);
       
       /*xor for CBC*/
       for (i = 0; i < AES_BLOCK_LENGTH; i++)
           output_data[i] ^= temp_iv[i];
       
       /*update init_vector for next AES round*/
       memcpy((void*)temp_iv,(void*)temp_block,AES_BLOCK_LENGTH);
       
       /*adjust pointers for next 3DES round*/
       input_data += AES_BLOCK_LENGTH;
       output_data += AES_BLOCK_LENGTH;  
    	
    }while(--blocks);

    
    memcpy((void*)init_vector,(void*)temp_block,AES_BLOCK_LENGTH);
    
    return 0;//OK
}
