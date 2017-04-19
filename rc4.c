#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/rand.h>


unsigned char* encrypt( unsigned char* pt, unsigned char* key)
{
	//Here I will assume that the first 4 bytes represent the number of bytes in the actual message.
	//i.e. pt = [message length]||[message to encrypt]
	//the same goesfor the key, that is the exact key length is given in the first 4 bytes of the string.
	int messageL;
	memcpy(&messageL, pt, 4);
	int keyL;
	memcpy(&keyL, key, 4);
	unsigned char* IV = malloc(32);
	unsigned char* randKey = malloc(32);
	RAND_bytes(IV, 32);
	for(int i = 0; i < 32; i++)//Initialize IV
	{
		randKey[i] = IV[i]^key[(i+4)%keyL];
	}
	unsigned char S[256];
	for(int i = 0; i< 256; i++)//Initialize state permutation
	{
		S[i] = (unsigned char)i;
	}
	//Key-Scheduling
	int j = 0;
	for(int i = 0; i < 255; i ++)
	{
		j = (j+S[i]+randKey[i%32])%256;
		unsigned char* temp = malloc(1);
		*temp = S[i];
		S[i] = S[j];
		S[j] = *temp;
		free(temp);	
	} 
	//PRGA
	j = 0;
	int i = 0;
	for(int k = 0; k <3072; k++)
	{
		//drop firs 3072 bytes generated
		i = (i+1) % 256;
		j = (j + S[i])% 256;
		unsigned char* temp = malloc(1);
		*temp = S[i];
		S[i] = S[j];
		S[j] = *temp;
		free(temp);	
	} 
	unsigned char* ciphertext= malloc(messageL);
	for(int k = 4; k <messageL+4; k++)//from index 4 to the end of the plaintext XOR with a keystream byte
	{
		i = (i+1)%256;
		j = (j+S[i])%256;
		unsigned char* temp = malloc(1);
		*temp = S[i];
		S[i] = S[j];
		S[j] = *temp;
		free(temp);
		unsigned char keychar = S[(S[i]+S[j])]%256;	
		ciphertext[k] = keychar^pt[k];
	}
	unsigned char* outp = malloc(4+32+messageL);
	strcat(outp, IV);
	int ciphL = 32+messageL;
	memcpy(outp, &ciphL, sizeof(int));//size of cipher in front
	memcpy(outp+sizeof(int), IV, 32);//concatenated with the IV
	memcpy(outp+sizeof(int)+32, ciphertext, messageL);//concatrnated with the ciphertext
	return outp;
}

unsigned char* decrypt( unsigned char* ct, unsigned char* key)
{
	//first 4 bytes represent string length+iv length
	//we know that the IV length is 32, so the length of the cipher we need to decrypt will
	//be the integer value of the first 4 bytes less 32
	int cipherL;
	memcpy(&cipherL, ct, 4);
	//the next 32 bytes are IV, the rest is the cipher
	//thus, it is safe to assume that ct has a length greater than 36
	unsigned char* IV = malloc(32);
	memcpy(IV, ct+4, 32);
	unsigned char* ciphertext = malloc(cipherL);
	memcpy(ciphertext, ct+32+4, cipherL);
	//at this point, IV and ciphertext are properly introduced. 	
	unsigned char S[256];
	for(int i = 0; i< 255; i++)
	{//init S permutation
		S[i] = (unsigned char)i;
	}
	//init rando key
	//here, we know that the key length is represented by the first 4 bytes of key
	int keyL; 
	memcpy(&keyL, key, 4); 
	unsigned char* randkey = malloc(32); 
	for(int i = 0; i < 32; i ++)
	{
		randkey[i] = IV[i]^key[(i+4)%keyL];//I use i+4 to ignore the first 4 bytes of the key, which only represent length rather than the key itself
	}		
	//KSA
	int j = 0;
	for(int i = 0; i < 255; i ++)
	{
		j = (j+S[i]+randkey[i%32])%256;
		unsigned char* temp = malloc(1);
		*temp = S[i];
		S[i] = S[j];
		S[j] = *temp;
		free(temp);	
	} 
	free(randkey);
	//PRGA
	int i = 0;
	j = 0;
	for(int k = 0; k< 3072; k++)		
	{
		//drop firs 3072 bytes generated
		i = (i+1) % 256;
		j = (j + S[i])% 256;
		unsigned char* temp = malloc(1);
		*temp = S[i];
		S[i] = S[j];
		S[j] = *temp;
		free(temp);	
	}
	unsigned char* plaintext = malloc(cipherL);
	//actual decryption in this loop
	for(int k = 4; k<cipherL+4; k++)
	{
		i = (i+1)%256;
		j = (j+S[i])%256;
		unsigned char* temp = malloc(1);
		*temp = S[i];
		S[i] = S[j];
		S[j] = *temp;
		free(temp);
		unsigned char keychar = S[(S[i]+S[j])]%256;	
		plaintext[k] = keychar^ciphertext[k];

	}
	//here I am placing the first 4 bytes to represent the length of the plaintext message
	unsigned char* outp = malloc(4+cipherL);
	memcpy(outp, &cipherL, 4);
	memcpy(outp+4, plaintext, cipherL);
	return outp;	
}

void main(){

}
