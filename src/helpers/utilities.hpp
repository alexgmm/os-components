#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <utility>
#include <fstream>
#include <numeric>	 //iota
#include <algorithm> //random_shuffle
#include <random>
#include <iterator>
#include <functional>
#include <iostream>
#include <assert.h>
#include <vector>

#include "../constants.h"

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

void merge(vector<unsigned> &array, int l, int m, int r)
{
	int n1 = m - l + 1;
	int n2 = r - m;

	vector<unsigned> L(n1), R(n2);

	for (int i = 0; i < n1; i++)
		L[i] = array[l + i];
	for (int j = 0; j < n2; j++)
		R[j] = array[m + 1 + j];

	int i = 0, j = 0, k = l;

	while (i < n1 && j < n2)
	{
		if (L[i] <= R[j])
		{
			array[k] = L[i];
			i++;
		}
		else
		{
			array[k] = R[j];
			j++;
		}
		k++;
	}

	while (i < n1)
	{
		array[k] = L[i];
		i++;
		k++;
	}

	while (j < n2)
	{
		array[k] = R[j];
		j++;
		k++;
	}
}

void mergeSort(vector<unsigned> &array, int l, int r)
{
	if (l >= r)
	{
		return;
	}
	int m = (l + r - 1) / 2;
	mergeSort(array, l, m);
	mergeSort(array, m + 1, r);
	merge(array, l, m, r);
}

void mergeSort(vector<unsigned> &array)
{
	mergeSort(array, 0, array.size() - 1);
}

#endif