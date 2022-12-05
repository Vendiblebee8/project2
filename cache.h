#ifndef Cache_H
#define Cache_H

#include <vector>
#include <utility>
#include <string>
#include <iostream>

#define String std::string

using namespace std;

class Cache
{
	public:
        Cache(vector< pair<char, int> >);
        int getCountEntries();
        int directMapped(int);
        int setAssociative(int);
	    int fullAssociativeLRU();
        int fullAssociativeHCR();
		int noAllocWriteMiss(int);
        int setAssociativePrefetching(int);
		int setAssociativePreOnMiss(int);

    private:
        vector< pair<char, int> > gooInputs;
};

#endif