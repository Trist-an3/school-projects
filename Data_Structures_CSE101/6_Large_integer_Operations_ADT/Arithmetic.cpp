/*******************************************************
 * Name: Tristan Garcia
 * CruzID: tgarcia7
 * 2024 Winter CSE101 PA6
 * File: Arithmetic.cpp
 * Top level client for performing operations
 ********************************************************/

#include <iostream>
#include <fstream>

#include "List.h"
#include "BigInteger.h"

using namespace std;

int main(int argc, char *argv[]) {

   ifstream infile;
   ofstream outfile;

   if(argc != 3) {
      cerr << "Usage: " << argv[0] << "<input file> <output file>" << endl;
      return (EXIT_FAILURE);
   }

   infile.open(argv[1]);
   outfile.open(argv[2]);
   
   if(!infile.is_open() || !outfile.is_open()) {
      cerr << "Error opening files. " << endl;
      return (EXIT_FAILURE);

   }

   string one;
   string two;

   infile >> one;
   infile >> two;  
 
   BigInteger A(one);
   BigInteger B(two);

   BigInteger s2("2");
   BigInteger s3("3");
   BigInteger s9("9");
   BigInteger s16("16");

   outfile << A << endl << endl;
   outfile << B << endl << endl;
   outfile << A + B << endl << endl;
   outfile << A - B << endl << endl;
   outfile << A - A << endl << endl;
   outfile << A.mult(s3) - B.mult(s2) << endl << endl;
   outfile << A * B << endl << endl;
   outfile << A * A << endl << endl;
   outfile << B * B << endl << endl;
   outfile << s9 * A * A * A * A + s16 * B * B * B * B * B << endl;


/*refernece
	f2.write(str(A)+'\n\n')
	f2.write(str(B)+'\n\n')
	f2.write(str(A+B)+'\n\n')
	f2.write(str(A-B)+'\n\n')
	f2.write(str(A-A)+'\n\n')
	f2.write(str(3*A-2*B)+'\n\n')
	f2.write(str(A*B)+'\n\n')
	f2.write(str(A**2)+'\n\n')
	f2.write(str(B**2)+'\n\n')
	f2.write(str(9*A**4+16*B**5)+'\n\n')
*/

   infile.close();
   outfile.close();

   return 0;
}

