#include<iostream>
#include<fstream>
#include<string>
#include<stdlib.h>

using namespace std;

int main(int argc, char * argv[])
{
	//Definitions of input and output file streams
	ofstream wrt;
	FILE * rd = fopen(argv[1], "rb");
	
	//Open input and output files in respective streams
	wrt.open(argv[2], ios::out);
	
	int n;
	fread(&n, sizeof(n), 1, rd);
	wrt << n << "\n";
	
	int m;
	unsigned char c;
	const unsigned char hexDigits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	for(int i = 0; i < n; i++)
	{
		fread(&m, sizeof(m), 1, rd);
		wrt << (2 * m + 2) << " 0X";	
			
		for(int j = 0; j < m; j++)
		{
			fread(&c, sizeof(c), 1, rd);
			wrt << hexDigits[((c >> 4) & 0xF)];
			wrt << hexDigits[(c & 0xF)];
		}
		
		wrt << "\n";
	}
	
	fclose(rd);
	wrt.close();
	return 0;
}