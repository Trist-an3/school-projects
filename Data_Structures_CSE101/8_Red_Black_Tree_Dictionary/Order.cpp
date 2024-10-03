/*******************************************************
* Name: Tristan Garcia
* CruzID: tgarcia7
* 2024 Winter CSE101 PA8
* File: Order.cpp
* Top client program
********************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include "Dictionary.h"

using namespace std;

int main(int argc, char* argv[]) {

   ifstream infile;
   ofstream outfile;

   if(argc != 3) {
      cerr << "Usage: " << argv[0] << "<input file> <output file>" << endl;
      return(EXIT_FAILURE);
   }

   infile.open(argv[1]);
   outfile.open(argv[2]);

   if(!infile.is_open() || !outfile.is_open()) {
      cerr << "Error opening files. " << endl;
      return (EXIT_FAILURE);
   }

   Dictionary D;
   string key;
   int line_count = 1;

   while(getline(infile, key)) {
      D.setValue(key, line_count);
      line_count++;
   }
   
   outfile << D << endl;
   outfile << D.pre_string() << endl;


   infile.close();
   outfile.close();
  
   return 0;
}


