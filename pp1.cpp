#include<iostream>
#include<fstream>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include <sstream>
#include<vector>
#include <openssl/hmac.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#define MAC_LEN 32 //MAC length in bytes

using namespace std;

vector<string> domains; //Vector of strings that will store an indexed list of all domains that the user has passwords with.
unsigned char* key;//Global Key Variable
unsigned char* MAC = new unsigned char[MAC_LEN];


bool bool_check_integrity();

//get_mac will get the mac of an encrypted file
unsigned char* get_mac(unsigned char* pt, int length)
{
	unsigned char* digest = new unsigned char[SHA512_DIGEST_LENGTH];
	HMAC_CTX ctx;
  	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, strlen((const char*)key), EVP_sha512(), NULL);
	HMAC_Update(&ctx, pt, length);
	HMAC_Final(&ctx, digest, (unsigned int*)&length);
	HMAC_CTX_cleanup(&ctx);
	
	
	


    // Be careful of the length of string with the choosen hash engine. SHA1 produces a 20-byte hash value which rendered as 40 characters.
    // Change the length accordingly with your choosen hash engine
    unsigned char* mdString = new unsigned char[32];
	memcpy(mdString, digest, 32);
	return mdString;
         
}

unsigned char *key_gen(string password)
{
	const char *topass = password.c_str();
	int passlen = password.length();
	const unsigned char *salt  = (unsigned char*)"12345678" ;
	int saltlen = 8;
	int toIter= 1000;
	const EVP_MD *digester = EVP_sha512();
	int keylen = 16;
	unsigned char* key1 = new unsigned char[16];
	int toTest = PKCS5_PBKDF2_HMAC( topass, passlen, salt, saltlen, toIter, digester, keylen, key1);
	return key1;
}


void handleErrors(void)
{
  ERR_print_errors_fp(stderr);
  abort();
}


//decrypt function involving openssl stores result in plaintext
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
  unsigned char *iv, unsigned char *plaintext)
{
  EVP_CIPHER_CTX *ctx;

  int len;

  int plaintext_len;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the decryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, key, iv))
    handleErrors();

  /* Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    handleErrors();
  plaintext_len = len;

  /* Finalise the decryption. Further plaintext bytes may be written at
   * this stage.
   */
  if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
  plaintext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}


void decrypt_pf()
{
	//Instantiate storw  IV
	unsigned char* IV = new unsigned char[16];
	//open the passwd_file and read the IV and cryptotext
	ifstream passwd;
	passwd.open("passwd_file");
	passwd.read((char*)IV, 16);
	passwd.seekg(0, passwd.end);
	int crypto_len = (int)passwd.tellg() - 16;
	if(passwd.tellg() <= 0){ passwd.close();return; } 
	//instantiate plaintext buffer	
	unsigned char* plaintext = new unsigned char[crypto_len];
	passwd.seekg(16, passwd.beg);
	unsigned char* cryptotext = new unsigned char[crypto_len];
	passwd.read((char*)cryptotext, crypto_len);
	
	//using the global key, decrypt the crypto_text
	decrypt(cryptotext, crypto_len, key, IV, plaintext);
	
	//change passwd_file to plaintext
	passwd.close();
	remove("passwd_file");
	ofstream opass;
	opass.open("passwd_file", ios::binary);
	opass.write((char*)plaintext, crypto_len - MAC_LEN);
	//store mac	
	memcpy(MAC,&plaintext[crypto_len - MAC_LEN], MAC_LEN);


	
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
  unsigned char *iv, unsigned char *ciphertext)
{
  EVP_CIPHER_CTX *ctx;

  int len;

  int ciphertext_len;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the encryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, key, iv))
    handleErrors();

  /* Provide the message to be encrypted, and obtain the encrypted output.
   * EVP_EncryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    handleErrors();
  ciphertext_len = len;

  /* Finalise the encryption. Further ciphertext bytes may be written at
   * this stage.
   */
  if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
  ciphertext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return ciphertext_len;
}

void encrypt_pf()
{
	//Instantiate array to hold new random IV
	unsigned char* IV = new unsigned char[16];
	RAND_bytes(IV, 16);
	
	//get filesizze
	ifstream passwd; 
	passwd.open("passwd_file");
	passwd.seekg(0, passwd.end);
	int file_size = passwd.tellg();

	passwd.seekg(0, passwd.beg);
	//allocate space for plaintext and ciphertext
	unsigned char* records = new unsigned char[file_size];
	unsigned char* plaintext = new unsigned char[file_size + MAC_LEN];
	unsigned char* ciphertext = new unsigned char[file_size + MAC_LEN];
	//read from file to get plaintext
	passwd.read((char*)records, file_size);
	//update MAC [legally]
	unsigned char* nMAC = get_mac(records, file_size);
	memcpy(plaintext, records, file_size);
	memcpy(&plaintext[file_size], nMAC, MAC_LEN);
	//encrypt
	encrypt(plaintext, file_size + MAC_LEN, key, IV, ciphertext);	
	//write IV then ciphertext to file
	passwd.close();
	remove("passwd_file");
	ofstream opass;
	opass.open("passwd_file");
	opass.write((char*)IV, 16);
	opass.write((char*)ciphertext, file_size + MAC_LEN);
}

//Function that opens passwd_file and implements the domains vector, that is the datatype used to keep track of the list of domain names in the file.
void instantiate_vector()
{
	//Instantiate Domain Vector
	decrypt_pf();		
	//open passwd_file and find its size in bytes
	ifstream passwd;
	passwd.open("passwd_file", ios::binary);
	passwd.seekg(0, passwd.end);
	int pass_file_length = passwd.tellg();
	passwd.seekg(0, passwd.beg);
		
	//look through password file, and obtain the domain name
	for(int i = 0; i < pass_file_length/240; i++)//240 is the length in bytes of each entry, so loop for only the number of entries
		{
			string domain = "";
			//first bytes until null will be the domain. once domain is read in, skip to a multiple of 240
			while((char)passwd.peek() != '\0')
			{
				domain = domain + (char)passwd.get();
			}
			domains.push_back(domain);
			passwd.seekg(240*(i+1),passwd.beg); 	
		}
	passwd.close();
	encrypt_pf();

	
}

void check_integrity_startup()
{
	if(bool_check_integrity()) {}
	else {cout<<"INTEGRITY CHECK OF PASSWORD FILE FAILED\n";}
}


void check_integrity()
{
	if(bool_check_integrity()) {cout<<"PASSED\n";}
	else {cout<<"FAILED\n";}
}

bool bool_check_integrity()
{
	decrypt_pf(); //this function reads the most recent MAC in the file.
	//read in contents of passwd_file
	ifstream passwd;
	passwd.open("passwd_file");
	passwd.seekg(0, passwd.end);
	int file_size = passwd.tellg();
	if(file_size <= 0) { return true; }//no entries at all, so we assume that this is the case of a new user
	unsigned char* data = new unsigned char[file_size];
	passwd.read((char*)data, file_size);
	//obtain a new mac
	unsigned char* nMac = get_mac(data, file_size);


	//compare old mac with the mac of the newly obtained plaintext.	
	bool eq = true;	
	for(int i = 0; i < MAC_LEN; i ++)
	{
		if(nMac[i] != MAC[i]) { eq = false; }
	}
	encrypt_pf();
	if(eq)
	{
		return true;
	}
	else
	{
		return false;
	}
	

}

//Function that lets the user register an account.
void register_account()
{
	decrypt_pf();
	//Strings that will store the details of the accunt.
	string domain_name;
	string user_name;
	string password;
	
	//Flag checking variables.
	int strlength = 81;
	bool domain_exists_flag = false;
	
	//Loop to check that the domain name entered is <= 80 characters. Loops until this is the case.
	while(strlength > 80)
	{
	cout<<"Enter the domain name of the account you want to register: ";
	cin >> domain_name;
	strlength = domain_name.length();
	}
	
	//Check that the input domain doesn't already exist
	for(int i = 0; i < domains.size(); i++)
	{
		if(domain_name == domains[i])
		{
			cout<<"USER ACCOUNT ALREADY EXISTS!\n"; //Go back to menu
			domain_exists_flag = true;
		}
	}
		
	if(domain_exists_flag == false)
	{
		domains.push_back(domain_name); //Add domain to global list of domains.
		
		int unlength, pwlength, dnlength; //Variables to store lengths of username, password, and domainname.
		
		
		strlength = 81; //Reset this since it has been changed
		//Input username for the domain
		while(strlength > 80)
		{
			cout<<"Enter username for "<<domain_name<<" : ";
			cin >> user_name;
			strlength = user_name.length();
		}
		unlength = user_name.length();
				
		strlength = 81; //Reset this since it has been changed
		//Input password for the domain
		while(strlength > 80)
		{
			cout<<"Enter password for "<<domain_name<<" : ";
			cin >> password;
			strlength = password.length();
		}
		pwlength = password.length();
		dnlength = domain_name.length();
		
		//Pad domain name with null characters to length 80
		int dn_pad = 80 - dnlength; //Number of characters to pad
		for(int i = 0; i < dn_pad; i++)
		{
			domain_name = domain_name + '\0';
		}
		
		//Pad username with null characters to length 80
		int un_pad = 80 - unlength; //Number of characters to pad
		for(int i = 0; i < un_pad; i++)
		{
			user_name = user_name + '\0';
		}
		
		//Pad password with null characters to length 80
		int pw_pad = 80 - pwlength; //Number of characters to pad
		for(int i = 0; i < pw_pad; i++)
		{
			password = password + '\0';
		}
		
		//Open passwd_file in append mode and add the new entry (in this order: domain, user, password).
		ofstream register_account;
		register_account.open("passwd_file", ios::app);
		register_account<<domain_name;
		register_account<<user_name;
		register_account<<password;
		register_account.close();
		

	}
	
	
	encrypt_pf();
}

//Function that allows user to delete an account
void delete_account()
{
	decrypt_pf();
	//Strings that store account details to be deleted.  
	string domain_name;
	string user_name;
	string password;
	//Strings that store the account details from passwd_file 
	string pf_uname = "";
	string pf_passwd = "";
	//obtain domain name
	cout<<"Enter the domain name of the account to delete: ";
	cin>> domain_name;
	//check if domain name exists:
	int domain_index = -1;
	for(int i = 0; i<domains.size(); i++)
	{
		if(!domains[i].compare(domain_name))
		{
			domain_index = i;
		}
	}	
	if(domain_index < 0)
	{
		cout<<"USER ACCOUNT DOES NOT EXIST\n";
		return;
	}
	
	//take the username and password
	cout<<"Enter the user name for "<<domain_name<<": ";
	cin>> user_name;
	cout<<"Enter the password for "<<domain_name<<": ";
	cin>> password;
	
		
	//go into the file and extract the username and password
	ifstream passwd;
	passwd.open("passwd_file", ios::binary);
	passwd.seekg(240*domain_index+80, passwd.beg);

	//first bytes until null will be the domain. once domain is read in, skip to a multiple of 240
	while((char)passwd.peek() != '\0')
	{
		pf_uname = pf_uname + (char)passwd.get();
	}
	
	//We will need to seek to the password
	passwd.seekg(240*domain_index + 160, passwd.beg);
	while((char)passwd.peek() != '\0')
	{
		pf_passwd = pf_passwd + (char)passwd.get();
	}
	


	//check validity of entered strings
	if(pf_uname.compare(user_name) || pf_passwd.compare(password))
	{
		cout<<"USER ACCOUNT DOES NOT EXIST\n";
		return;
	}
	//Input has been verified, remove entry from file
	passwd.seekg(0, passwd.beg);
	//read in file up to index*240, then skip 240, read til eof
	int size_of_file = domains.size()*240;
	char* beforeDel = new char[240*domain_index];
	char* afterDel = new char[size_of_file - 240*domain_index - 240];
	passwd.seekg(0, passwd.beg);
	passwd.read(beforeDel, 240*domain_index);
	passwd.seekg(240*(domain_index+1), passwd.beg);
	passwd.read(afterDel, size_of_file - 240*domain_index - 240);
	cout << "BEFORE:DEL:" << beforeDel;
	cout<<"\nAFTER:DELL:" <<afterDel<<"\n";	 
	passwd.close();
	remove("passwd_file");
	ofstream n;
	n.open("passwd_file");
	n.write(beforeDel, 240*domain_index);
	n.write(afterDel,  size_of_file - 240*domain_index - 240);
	domains.erase(domains.begin() + domain_index);	
	encrypt_pf();

}

//Function that allows user to change account details
void change_account()
{
	decrypt_pf();
	//Strings that store account details to be changed.  
	string domain_name;
	string user_name;
	string opassword;
	string npassword;
	//Strings that store the account details from passwd_file 
	string pf_uname = "";
	string pf_passwd = "";
	//obtain domain name
	cout<<"Enter the domain name of the account to change: ";
	cin>> domain_name;
	//check if domain name exists:
	int domain_index = -1;
	for(int i = 0; i<domains.size(); i++)
	{
		if(!domains[i].compare(domain_name))
		{
			domain_index = i;
		}
	}	
	if(domain_index < 0)
	{
		cout<<"USER ACCOUNT DOES NOT EXIST\n";
		return;
	}
	
	//take the username and password
	cout<<"Enter the user name for "<<domain_name<<": ";
	cin>> user_name;
	cout<<"Enter the old password for "<<domain_name<<": ";
	cin>> opassword;
	int strlength = 81; //Reset this since it has been changed
	//Input password for the domain
	while(strlength > 80)
	{
		cout<<"Enter new password for "<<domain_name<<" : ";
		cin >> npassword;
		strlength = npassword.length();
	}
	
	
		
	//go into the file and extract the username and password
	ifstream passwd;
	passwd.open("passwd_file", ios::binary);
	passwd.seekg(240*domain_index+80, passwd.beg);

	//first bytes until null will be the proper data 
	while((char)passwd.peek() != '\0')
	{
		pf_uname = pf_uname + (char)passwd.get();
	}
	
	//We will need to seek to the password
	passwd.seekg(240*domain_index + 160, passwd.beg);
	while((char)passwd.peek() != '\0')
	{
		pf_passwd = pf_passwd + (char)passwd.get();
	}
	

	//check validity of entered strings
	if(pf_uname.compare(user_name) || pf_passwd.compare(opassword))
	{
		cout<<"USER ACCOUNT DOES NOT EXIST\n";
		return;
	}
	//Input has been verified, change entry from file
	//pad the npassword to be 80 bytes.
	
	//Pad password with null characters to length 80
	int pw_pad = 80 - npassword.length(); //Number of characters to pad
	for(int i = 0; i < pw_pad; i++)
	{
		npassword = npassword + '\0';
	}
	const char* new_pass_to_write = npassword.c_str();
	char* before_pass = new char[domain_index*240 +160];
	int size_of_domains = domains.size();
	char* after_pass = new char[size_of_domains*240 -domain_index*240 - 240];
	passwd.seekg(0, passwd.beg);
	passwd.read(before_pass, domain_index*240 + 160);
	passwd.seekg(240*(domain_index+1), passwd.beg);
	passwd.read(after_pass, size_of_domains*240 -domain_index*240 - 240);	
	passwd.close();
	remove("passwd_file");
	ofstream opass;
	opass.open("passwd_file", ios::binary);
	opass.write(before_pass, domain_index*240+160);
	opass.write(new_pass_to_write, 80);
	opass.write(after_pass, size_of_domains*240 -domain_index*240 - 240);
	opass.close();
	
	encrypt_pf();	

}

//Function to retrieve password for the user, given domain name.
void get_password()
{
	decrypt_pf();
	string domain_name; //stores domain name for password to get
	bool found = false; //flag to check if the domain name exists in the file
	int index; //index of the domain - used to index in the file
	
	//User enters domain 
	cout<<"Enter the domain name  : ";
	cin>>domain_name; 

	//Go through the domains vectors to see if domain_name exists; if it does set the flag and the index
	for(int i = 0; i < domains.size(); i++)
	{
		if(domains[i] == domain_name)
		{
			found = true;
			index = i;
		}
	}
	
	//If the flag is not set, print an error message and return
	if(found == false)
		cout<<"Domain name doesn't exist!\n";
	else //If the flag is set, find the username and password for the domain name
	{
		string username, password; //to store the corresponding username and password
		char* un = new char [160]; //char array to store username from file
		char* pw = new char [160]; //char array to stsore password from file
		index = (index * 240) + 80; //position in file where the required user-name and password is stored.
		
		//open passwd file and extract the user name and password
		ifstream passwd;
		passwd.open("passwd_file", ios::in);
		
		//Read and print username
		passwd.seekg(index, ios::beg); //Set the get pointer to where we want to start reading from
		passwd.get(un, 80);
		username = string(un);
		cout.clear();

		
		//read and print password
		passwd.seekg(index+80, ios::beg); //Set the get pointer to where we want to start reading from
		passwd.get(pw, 80);
		password = string(pw);
		cout.clear();

		passwd.close();
	cout<<"username "<<username<<" password "<<password<<"\n";
	}
	encrypt_pf();
}

//Function that outputs a menu for the user to select a functionality to implement from.
void menu()
{
	string choice; //Stores user's choice from the menu.
	while(true)
	{
		cout<<"Options: \n1. Integrity Check (Enter 'check_integrity') \n2. Register Account (Enter 'register_account') \n3. Delete Acount (Enter 'delete_account') \n4. Change Account (Enter 'change_account') \n5. Get Password (Enter 'get_password') \n6. Exit (Enter 'exit')\n";
		cout<<"Enter an option :";
		cin >> choice; //Input user's option choice
		//Check what the choice is and call the respective function
		if(choice == "check_integrity")
		{
			check_integrity();
		}
		else if(choice == "register_account")
		{
			register_account();
		}
		else if(choice == "delete_account")
		{
			delete_account();
		}
		else if(choice == "change_account")
		{
			change_account();
		}
		else if(choice == "get_password")
		{
			get_password();
		}
		else if(choice == "exit")
		{
			exit(0);
			break;
		}
		else
		{
			cout<<"Invalid entry!\n"; //User can only select choices from the menu.
		}
	}
}

//Start here
int main(int argc, char * argv[])
{
	//initialize AES
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	OPENSSL_config(NULL);
	//hardcoding key

	if((ifstream("passwd_file")) && (ifstream("master_passwd"))) //If both passwd_file and master_passwd exist
	{
		FILE * rd; //File stream to input master_passwd
		unsigned char digest[SHA512_DIGEST_LENGTH]; //Stores digest of user password
		unsigned char master_passwd[80]; //Stores user's input of password
		unsigned char retrieve_master[SHA512_DIGEST_LENGTH]; //Stores digest of master password from file
		unsigned char salt[256]; //stores a 256 byte salt
		char saltedpass[336]; //stores password prepended with salt
		int inputlength = 256 + SHA512_DIGEST_LENGTH;
		unsigned char ip[inputlength];
		
		//Open master_passwd, copy master password to variable password, and then close it
		rd = fopen("master_passwd", "r");
		fread(&ip, sizeof(char), sizeof(ip), rd);
		//master.read((char*)retrieve_master, SHA512_DIGEST_LENGTH);
		fclose(rd);
		
		for(int i = 0; i < inputlength; i++)
		{
			if(i < 256)
				salt[i] = ip[i];
			else
				retrieve_master[i-256] = ip[i];
		}
		
		//User input for master password
		cout<<"\nEnter master password: ";
		scanf("%s",master_passwd);
		for(int i = 0; i < 336; i++)
		{
			if(i < 256)
				saltedpass[i] = salt[i];
			else
				saltedpass[i] = master_passwd[i-256];
		}
		
		//SHA512 hashing of master_passwd to digest
		SHA512_CTX ctx;
		SHA512_Init(&ctx);
		SHA512_Update(&ctx, saltedpass, strlen(saltedpass));
		SHA512_Final(digest, &ctx);
		
		//Using a boolean flag variable, check that every character of hashed master password matches the corresponding character of hashed user psasword
		bool flag = true;

		for(int i = 0; i < 64; i++)
		{	
			if(digest[i] != retrieve_master[i])
				flag = false;
		}
		
		//If user's input matches master password, log user in, call check_integrity(), and then call menu().
		if(flag == true)
		{
			string password;
			stringstream s;
			s << master_passwd;
			password = s.str();
			cout<<"Logged in!\n";
			key = key_gen(password); 	//creating unique key from password using PBKDF
			check_integrity_startup();
			instantiate_vector();		
			menu();
		}
		else
		{
			//Wrong password entered by user.
			cout<<"Wrong password!";
		}
		
	}
	else
	{
		ofstream passwd;
		FILE * wrt; //File streams to create master_passwd and passwd_file (Opening a file in the write mode creates that file if it does not exist)
		unsigned char digest[SHA512_DIGEST_LENGTH]; //stores password digest
		unsigned char password[80]; //stores user entered password
		unsigned char salt[256]; //stores a 256 byte salt
		char saltedpass[336]; //stores password prepended with salt
		
		//Good salt
		for(int i = 0;i < 256; i++)
			RAND_bytes(&salt[i], 1);
				
		//Input user's new master password
		cout<<"New user - enter your preferred master password : ";
		scanf("%s", password); //Input master password from user
		
		//Create salted password
		for(int i = 0; i < 336; i++)
		{
			if(i < 256)
				saltedpass[i] = salt[i];
			else
				saltedpass[i] = password[i-256];
		}

		//Run SHA512 on salted password to create digest
		SHA512_CTX ctx2;
		SHA512_Init(&ctx2);
		SHA512_Update(&ctx2, saltedpass, strlen(saltedpass));
		SHA512_Final(digest, &ctx2);
		
		int outputlength = 256 + SHA512_DIGEST_LENGTH;
		unsigned char op[outputlength];
		for(int i = 0; i < outputlength; i++)
		{
			if(i < 256)
				op[i] = salt[i];
			else
				op[i] = digest[i-256];
		}
		//Open master_passwd, store the password digest, and then close it.
		wrt = fopen("master_passwd","w");
		fwrite(&op, sizeof(char), sizeof(op), wrt);
		fclose(wrt);
		
		//Create passwd_file
		passwd.open("passwd_file");
		passwd.close();
	}
	
	//cleanup
	EVP_cleanup();
  	ERR_free_strings();
	
	return 0;
}

