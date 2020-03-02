//#include "types.h"
#include "stdlib.h"
#include "cau_api.h"
#include "aes.h"
//#include "crypto_common.h"
#include "derivative.h" 

 
/*after encrypt/decrypt we must have original input: symmetric crypto algorithm*/
/*AES in CBC mode*/


uint_8
aes_main(uint_8* dataptr, uint_32 *datalength)
{

    static unsigned char   ive[AES_BLOCK_LENGTH] = { 0, 0, 0, 0, 0, 0, 0, 0,
	                                            0, 0, 0, 0, 0, 0, 0, 0};
    /* IMPORTANT: MAKE SURE TO CHANGE THE KEY */
//    unsigned const char   key128[AES128/8] = { 0xf4, 0x0f, 0xd1, 0x79, 0x12, 0x54, 0xb7, 0xf2,
//	                                      0x2b, 0xd8, 0xcd, 0xd1, 0x05, 0xaa, 0x1d, 0x7e};

    static unsigned char   key128[AES128/8] = { 0xf4, 0x0f, 0xd1, 0x79, 0x12, 0x54, 0xb7, 0xf2,
	                                      0x2b, 0xd8, 0xcd, 0xd1, 0x05, 0xaa, 0x1d, 0x7e};
   
   
    
    static uint_8 ivInit = 0;
    uint_8* encrypteddataptr;
  
    if (ivInit)					// iv has been initialized												
    {
    	encrypteddataptr = dataptr;
    }

    else
    {
  	//		mymemcpy(ive,dataptr,AES_BLOCK_LENGTH);
      	memcpy(ive,dataptr,AES_BLOCK_LENGTH);
        	*datalength -= AES_BLOCK_LENGTH;
        	encrypteddataptr = dataptr + AES_BLOCK_LENGTH;
        	ivInit = 1;
    }
    
    
    decryptAES((unsigned char*) key128,(unsigned short) AES128,encrypteddataptr,dataptr,(unsigned short)*datalength,ive);
    return 0;
}

void mymemcpy(uint_8* desc, uint_8* source, uint_32 length)
{
	uint_32 i;
	for (i=0; i<length; i++)
	*desc++ = *source++;
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
decryptAES(unsigned char *key, \
           unsigned short mode, \
           unsigned char *input_data, \
           unsigned char *output_data, \
           unsigned short data_length, \
           unsigned char *init_vector)
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
    cau_aes_set_key((unsigned char*)key,(int) mode,(unsigned char*) key_expansion);

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
