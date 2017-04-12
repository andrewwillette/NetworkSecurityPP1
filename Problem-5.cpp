#include<iostream>
#include<string.h>
#include<time.h>
#include<cstdlib>

using namespace std;

void encrypt(char * kf, char * ctf, char * ptf)
{
	//File pointers for files
	FILE * plntxt = fopen(ptf, "rb");
	FILE * keyfile = fopen(kf, "rb");
	FILE * cprtxt = fopen(ctf, "wb");
		
	//Generate random 32 byte IV
	unsigned char IV[32];
	srand(time(NULL));
	for(int i = 0; i < 32; i++)
	{
		IV[i] = (unsigned char) (rand() % 256);	
	}
	
	//Read key from kf
	int keysize;
	fread(&keysize, sizeof(keysize), 1, keyfile);
	unsigned char keychar;
	unsigned char key1[keysize];
	for(int i = 0; i < keysize; i++)
	{
		fread(&keychar, sizeof(keychar), 1, keyfile);
		key1[i] = keychar;
	}
	
	//Pad key
	unsigned char key2[32];
	int topad = 32 - keysize;
	for(int i = 0; i < topad; i++)
	{
		key2[i] = (unsigned char) 0; 			
	}
	
	int pad1 = topad, pad2 = 0;
	while(pad1 < 32)
	{
		key2[pad1] = key1[pad2];
		pad1++;
		pad2++;
	}
		
	//XOR key2 and IV to get key
	unsigned char key[32];
	for(int i = 0; i < 32; i++)
	{
		key[i] = key2[i] ^ IV[i];
	}
	
	//Generate S (all permutations of a byte)
	unsigned char S[256];
	for(int i = 0; i < 256; i++)
	{
		S[i] = (unsigned char) i;
	}
	
	//Permute S
	int perm = 0;
	unsigned char temp1;
	int imod32, Si;
	for(int i = 0; i < 256; i++)
	{
		imod32 = (int) key[i % 32];
		Si = (int) S[i];
		perm = (perm + Si + imod32) % 256;
		temp1 = S[i];
		S[i] = S[perm];
		S[perm] = temp1;
	}
	
	//Extract plaintext
	int ptsize;
	fread(&ptsize, sizeof(ptsize), 1, plntxt);
	unsigned char plaintext[ptsize];
	unsigned char ptread;
	for(int i = 0; i < ptsize; i++)
	{
		fread(&ptread, sizeof(ptread), 1, plntxt);
		plaintext[i] = ptread;
	}
	
	//Generate G
	int opsize = 3072 + ptsize;
	unsigned char G[opsize];
	unsigned char temp2;
	int g1 = 0, g2 = 0;
	int Sg1, Sg2;
	for(int i = 0; i < opsize; i++)
	{
		g1 = (g1 + 1) % 256;
		g2 = (g2 + S[g1]) % 256;
		temp2 = S[g1];
		S[g1] = S[g2];
		S[g2] = temp2;
		Sg1 = (int) S[g1];
		Sg2 = (int) S[g2];
		G[i] = S[(Sg1 + Sg2) % 256];
	}
	
	//Drop 3072 bytes of G
	int cyphersize = opsize - 3072;
	unsigned char dropbytes[cyphersize];
	int cyp1 = 3072, cyp2 = 0;
	while(cyp1 < opsize)
	{
		dropbytes[cyp2] = G[cyp1];
		cyp1++;
		cyp2++;
	}
	
	//Generate cypertext
	unsigned char cyphertext[cyphersize];
	for(int i = 0; i < cyphersize; i++)
	{
		cyphertext[i] = plaintext[i] ^ dropbytes[i];
	}

	//Write cyphertext
	int ctsize = 32 + cyphersize;
	fwrite(&ctsize, sizeof(ctsize), 1, cprtxt); //Number of bytes
	
	//Write IV
	for(int i = 0; i < 32; i++)
	{
		fwrite(&IV[i], sizeof(unsigned char), 1, cprtxt);
	}
	
	//Write cypher
	for(int i = 0; i < cyphersize; i++)
	{
		fwrite(&cyphertext[i], sizeof(unsigned char), 1, cprtxt);
	}
	
	//Close pointers
	fclose(cprtxt);
	fclose(keyfile);
	fclose(plntxt);
}

void decrypt(char * kf, char * ctf, char * ptf)
{
	//File pointers for files
	FILE * plntxt = fopen(ptf, "wb");
	FILE * keyfile = fopen(kf, "rb");
	FILE * cprtxt = fopen(ctf, "rb");
	
	//Extract cyphertext file size
	int ctfsize;
	fread(&ctfsize, sizeof(ctfsize), 1, cprtxt);
	
	//Extract IV
	unsigned char IV[32];
	unsigned char IVread;
	for(int i = 0; i < 32; i++)
	{
		fread(&IVread, sizeof(IVread), 1, cprtxt);
		IV[i] = IVread;
	}
	
	//Extract cyphertext
	int cyphersize = ctfsize - 32;
	unsigned char cyphertext[cyphersize];
	unsigned char cyphertextread;
	for(int i = 0; i < cyphersize; i++)
	{
		fread(&cyphertextread, sizeof(cyphertextread), 1, cprtxt);
		cyphertext[i] = cyphertextread;
	}
	
	//Read key from kf
	int keysize;
	fread(&keysize, sizeof(keysize), 1, keyfile);
	unsigned char keychar;
	unsigned char key1[keysize];
	for(int i = 0; i < keysize; i++)
	{
		fread(&keychar, sizeof(keychar), 1, keyfile);
		key1[i] = keychar;
	}
	
	//Pad key
	unsigned char key2[32];
	int topad = 32 - keysize;
	for(int i = 0; i < topad; i++)
	{
		key2[i] = (unsigned char) 0; 			
	}
	
	int pad1 = topad, pad2 = 0;
	while(pad1 < 32)
	{
		key2[pad1] = key1[pad2];
		pad1++;
		pad2++;
	}
		
	//XOR key2 and IV to get key
	unsigned char key[32];
	for(int i = 0; i < 32; i++)
	{
		key[i] = key2[i] ^ IV[i];
	}
	
	//Generate S (all permutations of a byte)
	unsigned char S[256];
	for(int i = 0; i < 256; i++)
	{
		S[i] = (unsigned char) i;
	}
	
	//Permute S
	int perm = 0;
	unsigned char temp1;
	int imod32, Si;
	for(int i = 0; i < 256; i++)
	{
		imod32 = (int) key[i % 32];
		Si = (int) S[i];
		perm = (perm + Si + imod32) % 256;
		temp1 = S[i];
		S[i] = S[perm];
		S[perm] = temp1;
	}
	
	//Generate G
	int opsize = 3072 + cyphersize;
	unsigned char G[opsize];
	unsigned char temp2;
	int g1 = 0, g2 = 0;
	int Sg1, Sg2;
	for(int i = 0; i < opsize; i++)
	{
		g1 = (g1 + 1) % 256;
		g2 = (g2 + S[g1]) % 256;
		temp2 = S[g1];
		S[g1] = S[g2];
		S[g2] = temp2;
		Sg1 = (int) S[g1];
		Sg2 = (int) S[g2];
		G[i] = S[(Sg1 + Sg2) % 256];
	}
	
	//Drop 3072 bytes of G
	unsigned char dropbytes[cyphersize];
	int cyp1 = 3072, cyp2 = 0;
	while(cyp1 < opsize)
	{
		dropbytes[cyp2] = G[cyp1];
		cyp1++;
		cyp2++;
	}
	
	//Generate plaintext
	unsigned char plaintext[cyphersize];
	for(int i = 0; i < cyphersize; i++)
	{
		plaintext[i] = cyphertext[i] ^ dropbytes[i];
	}

	//Write plaintext size
	fwrite(&cyphersize, sizeof(cyphersize), 1, plntxt); 
		
	//Write plaintext
	for(int i = 0; i < cyphersize; i++)
	{
		fwrite(&plaintext[i], sizeof(unsigned char), 1, plntxt);
	}
	
	//Close pointers
	fclose(plntxt);
	fclose(keyfile);
	fclose(cprtxt);
}
//Command line arguments: option, key_file, ciphertext_file, plaintext_file
int main(int argc, char * argv [])
{
	if(strcmp(argv[1], "encrypt") == 0)
	{
		encrypt(argv[2], argv[3], argv[4]);
	}
	else if (strcmp(argv[1], "decrypt") == 0)
	{
		decrypt(argv[2], argv[3], argv[4]);
	}
	else
	{
		cout << "Invalid option!";
		exit(1);
	}
	return 0;
}
