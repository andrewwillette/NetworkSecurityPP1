#include<iostream>
#include<fstream>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include <sstream>

using namespace std;

void check_integrity()
{
	cout<<"Integrity has been checked. (I wish it was this simple.)\n\n";
}

void register_account()
{
	cout<<"Account has been registered. (I wish it was this simple.)\n\n";
}

void delete_account()
{
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