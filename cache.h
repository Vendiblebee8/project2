#ifndef Cache_H
#define Cache_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <chrono>
#include <algorithm>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <utility>

using namespace std;

class Cache
{
	public:
        Cache(vector< pair<char, int> >);
        int entryC();
        int dirM(int);
        int setA(int);
	    int assocL();
        int assocH();
		int writeM(int);
        int prefetchingOM(int);
		int setP(int);

    private:
        vector< pair<char, int> > inputsG;
};

#endif