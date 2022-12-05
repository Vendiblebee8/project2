#ifndef CACHES_H
#define CACHES_H

#include <vector>
#include <utility>
#include <string>
#include <iostream>

#define String std::string

using namespace std;

class Caches
{
	public:
        Caches(vector< pair<char, int> >);
        int getCountEntries();
        int directMapped(int);
        int setAssociative(int);
	    int fullAssociativeLRU();
        int fullAssociativeHCR();
		int noAllocWriteMiss(int);
        int setAssociativePrefetching(int);
		int setAssociativePreOnMiss(int);
		int secondChanceClocking();

    private:
        vector< pair<char, int> > gooInputs;
};

#endif