#include<iostream>
#include<fstream>
#include<string.h>
#include<stdlib.h>

using namespace std;

void encrypt(char * kf, char * ctf, char * ptf)
{
	//File pointers for files
	FILE * plntxt = fopen(ptf, "rb");
	FILE * key = fopen(kf, "rb");
	FILE * cprtxt = fopen(ctf, "wb");
	
	//Read size m of all both files and write to output file
	int m;
	fread(&m, sizeof(m), 1, plntxt);
	fread(&m, sizeof(m), 1, key);
	fwrite(&m, sizeof(m), 1, cprtxt);
	
	//Chars to read/write from/to 3 files
	unsigned char p;
	unsigned char k;
	unsigned char c;
	
	//Loop m times
	for(int i = 0; i < m; i++)
	{
		//Read 1 byte each of plaintext and key
		fread(&p, sizeof(p), 1, plntxt);
		fread(&k, sizeof(k), 1, key);
		
		//XOR and store result in the output var
		c = p ^ k;
		
		//Write output to cipher file
		fwrite(&c, sizeof(c), 1, cprtxt);
	}
	
	//Close files
	fclose(plntxt);
	fclose(key);
	fclose(cprtxt);
}

void decrypt(char * kf, char * ctf, char * ptf)
{
	//File pointers for files
	FILE * plntxt = fopen(ptf, "wb");
	FILE * key = fopen(kf, "rb");
	FILE * cprtxt = fopen(ctf, "rb");
	
	//Read size m of all both files and write to output file
	int m;
	fread(&m, sizeof(m), 1, cprtxt);
	fread(&m, sizeof(m), 1, key);
	fwrite(&m, sizeof(m), 1, plntxt);
	
	//Chars to read/write from/to 3 files
	unsigned char p;
	unsigned char k;
	unsigned char c;
	
	//Loop m times
	for(int i = 0; i < m; i++)
	{
		//Read 1 byte each of plaintext and key
		fread(&c, sizeof(c), 1, cprtxt);
		fread(&k, sizeof(k), 1, key);
		
		//XOR and store result in the output var
		p = c ^ k;
		
		//Write output to cipher file
		fwrite(&p, sizeof(p), 1, plntxt);
	}
	
	//Close files
	fclose(plntxt);
	fclose(key);
	fclose(cprtxt);
}

int main(int argc, char * argv[])
{
	if(strcmp(argv[1], "encrypt") == 0)
	{
		encrypt(argv[2], argv[3], argv[4]);
	}
	else if(strcmp(argv[1], "decrypt") == 0)
	{
		decrypt(argv[2], argv[3], argv[4]);
	}
	else
	{
		cout << "Invalid option!";
		exit(0);
	}
	return 0;
}