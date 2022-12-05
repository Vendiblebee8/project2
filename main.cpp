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
#include "cache.h"


using namespace std;

void Simulator(Cache cacheI, String Output);

int main(int argc, char *argv[])
{
	vector< pair<char, int> > vectorI;

    if (argc != 3)
    {
        exit(1);
    }

    ifstream inputF(argv[1]);
	String lsLine;

	if (inputF.is_open())
	{
		while (getline(inputF, lsLine))
		{
			char actionL = lsLine.at(0);
			String addressL = lsLine.substr(2);
			unsigned int addressH = stoul(addressL, nullptr, 16);

			vectorI.push_back(pair<char, int>(actionL, addressH));
		}

		inputF.close();
	}
	else
	{
        exit(3);
	}
	
    Cache cacheL(vectorI);
    Simulator(cacheL, argv[2]);

    return 0;
}

void Simulator(Cache cacheI, String Output)
{
    vector<int> tableS = {32, 128, 512, 1024};
    vector<int> assocN = {2, 4, 8, 16};
    int RV;

    ofstream outputS(Output);
    if (!outputS.is_open())
    {
        exit(2);
    }

    // Direct-Mapped Cache
    for (int i = 0; i < tableS.size(); i++)
    {
        RV = cacheI.directMapped(tableS.at(i));
        outputS << RV << "," << cacheI.getCountEntries();
        outputS << ((i == tableS.size() - 1) ? ";" : "; ");
    }
    outputS << endl;

    // setAssociativePreOnMiss
    for (int i = 0; i < assocN.size(); i++)
    {
        RV = cacheI.setAssociative(assocN.at(i));
        outputS << RV << "," << cacheI.getCountEntries();
        outputS << ((i == tableS.size() - 1) ? ";" : "; ");
    }
    outputS << endl;

    // Fully-Associative cache
    RV = cacheI.fullAssociativeLRU();
    outputS << RV << "," << cacheI.getCountEntries() << ";\n";
    RV = cacheI.fullAssociativeHCR();
    outputS << RV << "," << cacheI.getCountEntries() << ";\n";

    // Set-Associative Cache with no Allocation on a Write Miss
    for (int i = 0; i < assocN.size(); i++)
    {
        RV = cacheI.noAllocWriteMiss(assocN.at(i));
        outputS << RV << "," << cacheI.getCountEntries();
        outputS << ((i == tableS.size() - 1) ? ";" : "; ");
    }
    outputS << endl;

    // Set-Associative Cache with Next-line Prefetching
    for (int i = 0; i < assocN.size(); i++)
    {
        RV = cacheI.setAssociativePrefetching(assocN.at(i));
        outputS << RV << "," << cacheI.getCountEntries();
        outputS << ((i == tableS.size() - 1) ? ";" : "; ");
    }
    outputS << endl;
    for (int i = 0; i < assocN.size(); i++)
    {
        RV = cacheI.setAssociativePreOnMiss(assocN.at(i));
        outputS << RV << "," << cacheI.getCountEntries();
        outputS << ((i == tableS.size() - 1) ? ";" : "; ");
    }
    outputS << endl;
    outputS.close();
}