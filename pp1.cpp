#include<iostream>
#include<fstream>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include <sstream>
#include<vector>

using namespace std;

vector<string> domains; //Vector of strings that will store an indexed list of all domains that the user has passwords with.

void instantiate_vector()
{
	//Instantiate Domain Vector
		
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
	cout<<"\nFile length in bytes: " << pass_file_length << "\n";
	
}

void check_integrity()
{
	cout<<"Integrity has been checked. (I wish it was this simple.)\n\n";
}

void register_account()
{
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
			cout<<"This domain already exists!\n"; //Go back to menu
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
		
		cout<<"Written!\n\n";
	}
	
	cout<<"\n\nDomain list looks like this:\n";
	for(int i = 0; i < domains.size(); i++)
	{
		cout<<"Domain "<<i + 1<<" : \t"<<domains[i];
		cout<<"\n";
	}
	cout<<"\n\n";
}

void delete_account()
{
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
	cout<<"in index:"<<domain_index<<"\n";
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
	
	cout<<"Unme from file:"<<pf_uname<<"\n";
	cout<<"Passwd from file:"<<pf_passwd<<"\n";
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

	cout<<"Account has been deleted. (I wish it was this simple.)\n\n";
}

void change_account()
{
	cout<<"Account has been changed. (I wish it was this simple.)\n\n";
}

void get_password()
{
	cout<<"Password has been gotten. (I wish it was this simple.)\n\n";
}

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
	string master_passwd; //To store the master password from master_passwd
	if((ifstream("passwd_file")) && (ifstream("master_passwd"))) //If both passwd_file and master_passwd exist
	{
		string password; //To store user's input password
		ifstream master; //File stream to input master_passwd
		
		//Open master_passwd, copy master password to variable password, and then close it
		master.open("master_passwd", ios::in);
		master >>  password;
		master.close();
		
		//User input for master password
		cout<<"This is the master password: "<<password<<" (Security 101: DONT do this!)\n";
		cout<<"\nEnter master password: ";
		cin >> master_passwd;
		
		//If user's input matches master password, log user in, call check_integrity(), and then call menu().
		if(master_passwd == password)
		{
			cout<<"Logged in!\n";
			check_integrity();
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
		ofstream master, passwd; //File streams to create master_passwd and passwd_file (Opening a file in the write mode creates that file if it does not exist)
		cout<<"New user - enter your preferred master password : ";
		cin >> master_passwd; //Input master password from user
		
		//Open master_passwd, store the password entered by the user in it, and then close it.
		master.open("master_passwd", ios::out);
		master<<master_passwd;
		master.close();
		
		//Create passwd_file
		passwd.open("passwd_file");
		passwd.close();
	}
	
	return 0;
}
