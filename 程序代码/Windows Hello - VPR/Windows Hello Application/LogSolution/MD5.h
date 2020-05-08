#pragma once

#include <string>
#include <iostream>
using namespace std;

class MD5
{
private:
	void mainLoop(unsigned int M[]);
	unsigned int* add(string str);
	string changeHex(int a);
public:
	string getMD5(string source);
};