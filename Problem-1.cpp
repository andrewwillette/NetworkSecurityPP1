#include<iostream>
#include<fstream>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include <sstream>

using namespace std;

string digitToHex(string dig)
{
	if(dig == "10")
		return "A";
	else if(dig =="11")
		return "B";
	else if(dig =="12")
		return "C";
	else if(dig =="13")
		return "D";
	else if(dig =="14")
		return "E";
	else if(dig =="15")
		return "F";
	else
		return dig;
}
string hexToBinDigit(char hex)
{
    if(hex == '0')
        return "0000";
    else if(hex == '1')
        return "0001";
    else if(hex == '2')
        return "0010";
    else if(hex == '3')
        return "0011";
    else if(hex == '4')
        return "0100";
    else if(hex == '5')
        return "0101";
    else if(hex == '6')
        return "0110";
    else if(hex == '7')
        return "0111";
    else if(hex == '8')
        return "1000";
    else if(hex == '9')
        return "1001";
    else if(hex == 'A')
        return "1010";
    else if(hex == 'B')
        return "1011";
    else if(hex == 'C')
        return "1100";
    else if(hex == 'D')
        return "1101";
    else if(hex == 'E')
        return "1110";
    else if(hex == 'F')
        return "1111";
}

string binToHexDigit(string bin)
{
    if(bin == "0000")
        return "0";
    else if(bin == "0001")
        return "1";
    else if(bin == "0010")
        return "2";
    else if(bin == "0011")
        return "3";
    else if(bin == "0100")
        return "4";
    else if(bin == "0101")
        return "5";
    else if(bin == "0110")
        return "6";
    else if(bin == "0111")
        return "7";
    else if(bin == "1000")
        return "8";
    else if(bin == "1001")
        return "9";
    else if(bin == "1010")
        return "A";
    else if(bin == "1011")
        return "B";
    else if(bin == "1100")
        return "C";
    else if(bin == "1101")
        return "D";
    else if(bin == "1110")
        return "E";
    else if(bin == "1111")
        return "F";
}

void hex2bin(char* input, char* output)
{
	//Definitions of input and output file streams
	ofstream wrt;
	ifstream rd;

	//String to store data from input file intermediately
	string line;
	
	//Open input and output files in respective streams
	rd.open(input);
	wrt.open(output);

	//Extract n from input file's first line, add it to outputs first line, and convert it to an integer
	getline(rd, line);
	wrt << line;
	int n = atoi(line.c_str());
	//cout<<"Number of words: "<<n<<"\n";
	
	//Extract n hex numbers from input file
	for(int i = 0; i < n; i++)
	{
		wrt << "\n";
		//cout<<"Line number: "<<i + 1<<"\t";
		
		//Extract the current line
		getline(rd, line);
		
		//Extract l from the line (everything before space), add it to output file, and convert it to an integer
		int ind = 0;
		string lstr = "";
		string hstr = "";
		while(line[ind] != ' ')
		{
			lstr = lstr + line[ind];
			ind++;
		}
		int l = atoi(lstr.c_str());
		//cout<<"Size of word: "<<l<<"\t\t";

		int l2 = 0;
		if(l != 0)
			l2 = (l - 2) * 4; //Subtract 2 for OX, and multiply by 4 for conversion
		wrt << l2 <<" ";
		
		//Extract b from the line (everything after space)
		int ctr = l; //Counter to determine the word
		if(l == 0)
			ind++; //Skip the space
		else
			ind = ind + 3; //Skip the space and the 0X
		while(ctr != 0)
		{
			hstr = hstr + line[ind];
			ind++;
			ctr--;
		}
		//cout<<"Word: "<<hstr<<"\t";
		
		//Convert hstr to binary, add it to output file
		int ind2 = 0;
		string bin = "";
		while(hstr[ind2] != '\0')
		{
			bin = bin + hexToBinDigit(hstr[ind2]);
			ind2++;
		}
		//cout<<"Binary form: "<<bin<<"\n";
		wrt << bin;
	}

	wrt.close();
	rd.close();
}

void bin2hex(char* input, char* output)
{
	//Definitions of input and output file streams
	ofstream wrt;
	ifstream rd;

	//String to store data from input file intermediately
	string line;
	
	//Open input and output files in respective streams
	rd.open(input);
	wrt.open(output);

	//Extract n from input file's first line, add it to outputs first line, and convert it to an integer
	getline(rd, line);
	wrt << line;
	int n = atoi(line.c_str());
	//cout<<"Number of words: "<<n<<"\n";
	
	//Extract n binary numbers from input file
	for(int i = 0; i < n; i++)
	{
		wrt << "\n";
		//cout<<"Line number: "<<i + 1<<"\t";
		
		//Extract the current line
		getline(rd, line);
		
		//Extract l from the line (everything before space), add it to output file, and convert it to an integer
		int ind = 0;
		string lstr = "";
		string bstr = "";
		while(line[ind] != ' ')
		{
			lstr = lstr + line[ind];
			//cout<<line[ind];
			ind++;
		}
		wrt << lstr <<" ";
		int l = atoi(lstr.c_str());
		//cout<<"Size of word: "<<l<<"\t";
		
		//Extract b from the line (everything after space)
		int ctr = l; //Counter to determine the word
		ind++; //Skip the space
		while(ctr != 0)
		{
			bstr = bstr + line[ind];
			ind++;
			ctr--;
		}
		//int b = atoi(bstr.c_str());
		//cout<<"Word: "<<bstr<<"\t";
		
		//Convert b to hexadecimal, 4 characters at a time, add it to output file
		string rem = "";
		string hex = "";
		while(bstr.length() > 0)
		{
			rem = bstr.substr(bstr.length() - 4, 4);
			hex = binToHexDigit(rem) + hex;
			bstr = bstr.substr(0, bstr.length() - 4);
		}
		hex = "0X" + hex;
		//cout<<"Hexadecimal form: "<<hex<<"\n";
		wrt << hex;
	}

	wrt.close();
	rd.close();
}

void dec2bin(char* input, char* output)
{
	//Definitions of input and output file streams
	ofstream wrt;
	ifstream rd;
	stringstream ss; //For converting ints to string
	
	//String to store data from input file intermediately
	string line;
	
	//Open input and output files in respective streams
	rd.open(input);
	wrt.open(output);

	//Extract n from input file's first line, add it to outputs first line, and convert it to an integer
	getline(rd, line);
	wrt << line;
	int n = atoi(line.c_str());
	//cout<<"Number of words: "<<n<<"\n";
	
	//Extract n decimal numbers from input file
	for(int i = 0; i < n; i++)
	{
		wrt << "\n";
		//cout<<"Line number: "<<i + 1<<"\t";
		
		//Extract the current line
		getline(rd, line);
		
		//Convert current decimal number to binary
		int d = atoi(line.c_str());
		//cout<<"Decimal: "<<d<<"\t";
		int rem = 0;
		string r = "";
		string bin = "";
		if(d == 0)
			bin = "0";
		while(d != 0)
		{
			rem = d % 2;
			ss << rem;
			ss >> r;
			ss.clear();
			bin = r + bin;
			d = d / 2;
		}
		//cout<<"Binary: "<<bin<<"\t";
		
		//Pad to 32 bit
		int l = bin.length();
		int padl = 0;
		if(l < 32)
			padl = 32 - l;
		while(padl != 0)
		{
			bin = "0" + bin;
			padl--;
		}
		//cout<<"Padded number: "<<bin<<"\n";
		wrt << "32 " <<bin;
	}	
	wrt.close();
	rd.close();
}

void dec2hex(char* input, char* output)
{
	//Definitions of input and output file streams
	ofstream wrt;
	ifstream rd;
	stringstream ss; //For converting ints to string
	
	//String to store data from input file intermediately
	string line;
	
	//Open input and output files in respective streams
	rd.open(input);
	wrt.open(output);

	//Extract n from input file's first line, add it to outputs first line, and convert it to an integer
	getline(rd, line);
	wrt << line;
	int n = atoi(line.c_str());
	//cout<<"Number of words: "<<n<<"\n";
	
	//Extract n decimal numbers from input file
	for(int i = 0; i < n; i++)
	{
		wrt << "\n";
		//cout<<"Line number: "<<i + 1<<"\t";
		
		//Extract the current line
		getline(rd, line);
		
		//Convert current decimal number to hex
		int d = atoi(line.c_str());
		//cout<<"Decimal: "<<d<<"\t";
		int rem = 0;
		string r = "";
		string hex = "";
		if(d == 0)
			hex = "0";
		while(d != 0)
		{
			rem = d % 16;
			ss << rem;
			ss >> r;
			ss.clear();
			hex = digitToHex(r) + hex;
			d = d / 16;
		}
		//cout<<"Hexadecimal: "<<hex<<"\t";
		
		//Pad to 8 bit
		int l = hex.length();
		int padl = 0;
		if(l < 8)
			padl = 8 - l;
		while(padl != 0)
		{
			hex = "0" + hex;
			padl--;
		}
		hex = "0X" + hex;
		//cout<<"Padded number: "<<hex<<"\n";
		wrt << "10 " <<hex;
	}	
	wrt.close();
	rd.close();
}

int main(int argc, char * argv[])
{
	//Check input option and call relevant function
	if(strcmp(argv[1], "bin2hex") == 0)
		bin2hex(argv[2], argv[3]);
	else if(strcmp(argv[1], "hex2bin") == 0)
		hex2bin(argv[2], argv[3]);
	else if(strcmp(argv[1], "dec2bin") == 0)
		dec2bin(argv[2], argv[3]);
	else if(strcmp(argv[1], "dec2hex") == 0)
		dec2hex(argv[2], argv[3]);
	else
	{
		cout<<"Invalid option entered!";
		exit(0);
	}
}


	