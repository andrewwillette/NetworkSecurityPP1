#include <openssl/hmac.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

using namespace std;

 



//given a key and set of plaintext, calculates the HMAC
//output should be (unsigned char *output) length = 8
unsigned char* macMaker(const void *key, const unsigned char *pt, int len)
{
	
	unsigned char* digest;

	const EVP_MD *toRun = EVP_sha512();
	
    	digest = HMAC(toRun, key, strlen((const char *)key), pt, strlen((const char*)pt), NULL, NULL);    

    // Be careful of the length of string with the choosen hash engine. SHA1 produces a 20-byte hash value which rendered as 40 characters.
    // Change the length accordingly with your choosen hash engine
    char mdString[500];
    for(int i = 0; i < 20; i++)
         sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

    printf("HMAC digest: %s\n", mdString);
	
}


int main(int argc, char * argv[])
{
	//ERR_load_crypto_strings();
  	//OpenSSL_add_all_algorithms();
  	//OPENSSL_config(NULL);
	const void *key = "012345678";
	const unsigned char *pt = (const unsigned char *)"helloworld the hell is up";
	cout<<"Made it to macMaker call\n";  //FOR DEBUGGING
	unsigned char * toTest = macMaker(key, pt, 25);	
	//int testing = strlen((char*)toTest);
	
	
	return 0;
}

