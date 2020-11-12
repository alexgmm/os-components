#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <utility>
#include <fstream>
#include <numeric>	 //iota
#include <algorithm> //random_shuffle
#include <random>
#include <iterator>
#include <functional>

#include "constants.h"

using namespace std;

string getVectorString(vector<unsigned> &v, string name)
{
	string c = "\n" + name + " = {";
	for (unsigned i = 0; i < v.size(); i++)
	{
		c += to_string(v[i]);
		if (i != v.size() - 1)
			c += ",";
	}
	c += "};\n";

	return c;
}

void fillVecRandom(vector<unsigned> &v)
{
	random_device rnd_device;
	mt19937 mersenne_engine{rnd_device()};
	iota(v.begin() + 1, v.end(), 1);
	shuffle(begin(v) + 1, end(v), mersenne_engine);
}

/////////////////////////////////////////////////////////////////////////
////////////////////////////// DEBUG ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

void c() { cout << br << "*** checkpoint ***" << br; }

int randint(int ini, int end)
{
	assert(ini >= 0);
	assert(end > ini);
	random_device rd;
	mt19937 m(rd());
	uniform_int_distribution<int> b(ini, end);
	return b(m);
	//return ini + (rand() % (end - ini + 1));
}

void printv(vector<unsigned> &v, int ini, string name)
{
	cout << "\n"
		 << name << "\n";
	for (int i = ini; i < v.size(); i++)
		cout << v[i] << " ";
	cout << "\n";
}

void printl(string s, unsigned v) { cout << s << " " << v << br; }

unsigned findIndex(vector<unsigned> &v, unsigned value)
{
	for (unsigned i = 0; i < v.size(); i++)
		if (v[i] == value)
			return i;
	return UMAX;
}

bool isShift(unsigned m)
{
	return m == SHIFT_CRITICAL || m == SHIFT_WHOLE;
}

bool isSwap(unsigned m)
{
	return m == SWAP_SUCC || m == SWAP_PRED;
}

string wrapStringInLabel(string s)
{
	string layer = "+";
	for (int i = 0; i < s.size(); i++)
		layer += "-";
	layer += "+\n";
	s = "|" + s + "|\n";
	return layer + s + layer;
}

#endif