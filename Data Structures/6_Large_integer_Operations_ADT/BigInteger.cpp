/*******************************************************
 * Name: Tristan Garcia
 * CruzID: tgarcia7
 * 2024 Winter CSE101 PA6
 * File: BigInteger.cpp
 * ADT file for the BigInteger Functions
 *******************************************************/

#include <iostream> 
#include <string>
#include <stdexcept> 
#include <ctype.h>

#include "List.h"
#include "BigInteger.h"

using namespace std;

#define POWER 9
#define BASE 1000000000

/*********** Constructors & Destructors ***********/
BigInteger::BigInteger() {
    signum = 0;
    digits = List();
}

BigInteger::BigInteger(long x) {
    if (x < 0){
        signum = -1;
    }
    else if (x > 0){
        signum = 1;
    }
    while(x) {
        digits.insertAfter(x % BASE);
        x /= BASE;
    }
}


BigInteger::BigInteger(std::string s) {
    if(s.length() == 0) {
        throw std::invalid_argument("BigInteger: Constructor: empty string");
    }

    if(s[0] == '-') {
        signum = -1;
    }
    else if(s[0] == '+') {
        signum = 1;
    } else {
        signum = 1;
    }

    if (s[0] == '+' || s[0] == '-') {
        s = s.substr(1);
    }

    for(char c: s) {
        if(!isdigit(c)) {
            throw std::invalid_argument("BigInteger: Constructor: non-numeric string");
        }
    }

    int index = s.length() % POWER;
    if(index) {
        digits.insertBefore(stol(s.substr(0, index)));
    }

    while (index < (int)s.length()) {
        digits.insertBefore(stol(s.substr(index, POWER))); 
        index += POWER;
    }
}

BigInteger::BigInteger(const BigInteger& N) {
    signum = N.signum;
    digits = N.digits;
}
/*********** Access Functions ***********/
int BigInteger::sign() const {
    return signum;
}

int BigInteger::compare(const BigInteger& N) const {
    if (signum == 0 && N.signum == 0) {
        return 0; 
    } 
    else if (signum > N.signum) {
        return 1; 
    }
    else if (signum < N.signum) {
        return -1; 
    }
    
    List original = digits;
    List temp = N.digits;
    original.moveFront();
    temp.moveFront();

    while (original.position() < original.length() && temp.position() < temp.length()) {
        if (original.peekNext() > temp.peekNext()) {
            if (signum == 1) {
                return 1; 
            } else {
                return -1; 
            }
        } else if (original.peekNext() < temp.peekNext()) {
            if (signum == 1) {
                return -1; 
            } else {
                return 1; 
            }
        }

        original.moveNext();
        temp.moveNext();
    }
    return 0;
}

/*********** Manipulation procedures ***********/
void BigInteger::makeZero() {
    digits.clear();
    signum = 0;
}

void BigInteger::negate() {
    if(signum == 0) {
        return;
    } else {
        signum *= -1;
    }
}

/*********** Helper Functions ***********/
void negateList(List& L) {
    if(L.length() > 0) {
        L.moveFront();
        do {
            L.setAfter(-1 * L.peekNext());
            L.moveNext();
        } while(L.position() < L.length());
    }
}

void sumList(List& S, List A, List B, int sign) {
    S.clear();

    for (B.moveFront(); B.position() < B.length(); B.moveNext()) {
        B.setAfter(sign * B.peekNext());
    }

    for (A.moveBack(), B.moveBack(); A.position() > 0 && B.position() > 0; A.movePrev(), B.movePrev()) {
        S.insertAfter(A.peekPrev() + B.peekPrev());
    }

    for (; A.position() > 0; A.movePrev()) {
        S.insertAfter(A.peekPrev());
    }

    for (; B.position() > 0; B.movePrev()) {
        S.insertAfter(B.peekPrev());
    }

    for (S.moveFront(); S.front() == 0 && S.length() > 1; S.eraseAfter()) {}
}

int normalizeList(List& L) {
    if (L.front() == 0) {
        return 0;
    }

    int sign = 1;
    if (L.front() < 0) {
        sign = -1;
        negateList(L);
    }

    L.moveBack();
    int carry = 0;
    ListElement sum = 0;

    if (L.position() > 0) {
        do {
            sum = L.peekPrev() + carry;
            carry = 0;
            if (sum < 0) {
                carry = -1;
                sum += BASE;
            }
            L.setBefore(sum % BASE);
            carry += sum / BASE;
            L.movePrev();
        } while (L.position() > 0);
    }

    if (carry > 0) {
        L.moveFront();
        L.insertAfter(carry);
    }

    return sign;
}

void shiftList(List& L, int p) {
    if (p > 0) {
        L.moveBack();
        int i = 0;
        do {
            L.insertAfter(0);
            i++;
        } while (i < p);
    }
}

void scalarMultList(List& L, ListElement m) {
    if (L.length() > 0) {
        L.moveFront();
        do {
            L.setAfter(L.peekNext() * m);
            L.moveNext();
        } while (L.position() < L.length());
    }
}


/*********** BigInteger Arithmetic operations ***********/
BigInteger BigInteger::add(const BigInteger& N) const {
    BigInteger result;

    List sum;
    List digitsA = digits;
    List digitsB = N.digits;

    signum < 0 ? negateList(digitsA) : void();
    N.signum < 0 ? negateList(digitsB) : void();

    sumList(sum, digitsA, digitsB, 1);

    result.signum = normalizeList(sum);

    result.digits = sum;

    return result;
}

BigInteger BigInteger::sub(const BigInteger& N) const {
    BigInteger diff;
    BigInteger negatedN = N;
    negateList(negatedN.digits);
    diff = add(negatedN);
    return diff;
}

BigInteger BigInteger::mult(const BigInteger& N) const {
    BigInteger result;
    List sumL;
    List Ndigits = N.digits;
    List tempProduct;
    int shift = 0;

    if (signum == 0 || N.signum == 0) {
        return result;
    }

    do {
        tempProduct = digits;
	// Debug output
	// cout << "tempProduct before scalarMultList: " << tempProduct << endl;
        
	scalarMultList(tempProduct, Ndigits.peekPrev());
        // Debug output
        // cout << "tempProduct after scalarMultList: " << tempProduct << endl;
       
	shiftList(tempProduct, shift);
        // Debug output
        // cout << "tempProduct after shiftList: " << tempProduct << endl;
        
	List sumTemp = sumL;
        // Debug output
        // cout << "sumTemp before sumList: " << sumTemp << endl;

        sumList(sumL, sumTemp, tempProduct, 1);
       // Debug output
       // cout << "sumList after sumList: " << sumList << endl;
       
	normalizeList(sumL);
       // Debug output
       // cout << "sumList after normalizeList: " << sumList << endl;

        shift++;
        Ndigits.movePrev();
    } while (Ndigits.position() > 0);

    result.digits = sumL;
    if (signum == N.signum) {
        result.signum = 1;
    } else {
        result.signum = -1;
    }
    return result;
}
       
/*********** Other Functions ***********/
std::string BigInteger::to_string() {
    std::string resultString;

    if (signum == -1) {
        resultString = "-";
    } else {
        resultString = "";
    }

    digits.moveFront();

    while (digits.front() == 0 && digits.length() > 1) {
        digits.eraseAfter();
    }

    digits.moveFront();
    while (digits.position() < digits.length()) {
        std::string currentDigitString = std::to_string(digits.peekNext());
        std::string leadingZeros = "";

        while ((int)leadingZeros.length() + (int)currentDigitString.length() < POWER && digits.position() > 0) {
            leadingZeros += '0';
        }

        resultString += leadingZeros + currentDigitString;
        digits.moveNext();
    }

    return resultString;
}

/*********** Overriden Operators ***********/
std::ostream& operator<<(std::ostream& stream, BigInteger N) {
    return stream << N.to_string();
}

bool operator==( const BigInteger& A, const BigInteger& B ){
    if(A.compare(B) == 0) {
        return true;
    } 
    return false;
}

bool operator<( const BigInteger& A, const BigInteger& B ) {
    if(A.compare(B) < 0) {
        return true;
    }
    return false;
}

bool operator<=( const BigInteger& A, const BigInteger& B ) {
    if(A.compare(B) <= 0) {
        return true;
    }
    return false;
}

bool operator>( const BigInteger& A, const BigInteger& B ) {
    if(A.compare(B) > 0) {
        return true;
    }
    return false;
}

bool operator>=( const BigInteger& A, const BigInteger& B ) {
    if(A.compare(B) >= 0) {
        return true;
    }
    return false;
}

BigInteger operator+( const BigInteger& A, const BigInteger& B ) {
    return A.add(B);
}

BigInteger operator+=( BigInteger& A, const BigInteger& B ) {
    A = A.add(B);
    return A;
}

BigInteger operator-( const BigInteger& A, const BigInteger& B ) {
    return A.sub(B);
}

BigInteger operator-=( BigInteger& A, const BigInteger& B ) {
    A = A.sub(B);
    return A;
}

BigInteger operator*( const BigInteger& A, const BigInteger& B ) {
    return A.mult(B);
}

BigInteger operator*=( BigInteger& A, const BigInteger& B ) {
    A = A.mult(B);
    return A;
}

