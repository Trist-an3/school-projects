/*******************************************************
* Name: Tristan Garcia
* CruzID: tgarcia7
* 2024 Winter CSE101 PA8
* File: WordFrequency.cpp
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
   
   size_t begin, end, len;
   string line;
   //string tokenBuffer;
   string token;   
   string delim = " \t\\\"\',<.>/?;:[{]}|`~!@#$%^&*()-_=+0123456789";
   Dictionary D;

   while(getline(infile, line)){
      len = line.length();
    
      begin = min(line.find_first_not_of(delim, 0), len);
      end   = min(line.find_first_of(delim, begin), len);
      token = line.substr(begin, end-begin);
    
      while( token!="" ){
         for(size_t i = 0; i < token.length(); i++){
            token[i] = tolower(token[i]);
         }
    
         if(D.contains(token) == 0){
            D.setValue(token, 1);     
         }else{
            int pog = (D.getValue(token) + 1);
            D.setValue(token, pog);
         }
    
         begin = min(line.find_first_not_of(delim, end+1), len);
         end   = min(line.find_first_of(delim, begin), len);
         token = line.substr(begin, end-begin);

      }
   }
   outfile << D << endl;
   
   infile.close();
   outfile.close();
   
   return 0;
}
