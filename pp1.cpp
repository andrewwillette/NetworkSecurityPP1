#include<iostream>
#include<fstream>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include <sstream>
#include<vector>

using namespace std;

vector<string> domains; //Vector of strings that will store an indexed list of all domains that the user has passwords with.

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
		
		//Pad domain name with null characters to length 80
		int dn_pad = 80 - unlength; //Number of characters to pad
		for(int i = 0; i < dn_pad; i++)
		{
			domain_name = domain_name + '\0';
		}
		dnlength = domain_name.length();
		
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
			
			//Instantiate Domain Vector
			
			//open passwd_file and find its size in bytes
			ifstream passwd;
			passwd.open("passwd_file", ios::in);
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
