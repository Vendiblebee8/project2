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

void Simulator(Cache cacheI, string Output);

int main(int argc, char *argv[])
{
	vector< pair<char, int> > vectorI;

    if (argc != 3)
    {
        exit(1);
    }

    ifstream inputF(argv[1]);
	string lsLine;

	if (inputF.is_open())
	{
		while (getline(inputF, lsLine))
		{
			char actionL = lsLine.at(0);
			string addressL = lsLine.substr(2);
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

void Simulator(Cache cacheI, string Output)
{
    vector<int> tableS = {32, 128, 512, 1024};
    vector<int> assocN = {2, 4, 8, 16};

    ofstream outputS(Output);
    if (outputS.is_open()==false)
    {
        exit(2);
    }

    // Direct-Mapped Cache
    for (int i = 0; i < tableS.size(); i++)
    {
        outputS << cacheI.dirM(tableS.at(i)) << "," << cacheI.entryC()<< ((i == tableS.size() - 1) ? ";" : "; ");
    }
    outputS << endl;

    // setP
    for (int i = 0; i < assocN.size(); i++)
    {
        outputS << cacheI.setA(assocN.at(i)) << "," << cacheI.entryC()<< ((i == tableS.size() - 1) ? ";" : "; ");
    }
    outputS << endl;

    // Fully-Associative cache
    outputS << cacheI.assocL() << "," << cacheI.entryC() << ";" << endl;
    outputS << cacheI.assocH() << "," << cacheI.entryC() << ";" << endl;

    // Set-Associative Cache with no Allocation on a Write Miss
    for (int i = 0; i < assocN.size(); i++)
    {
        outputS << cacheI.writeM(assocN.at(i)) << "," << cacheI.entryC()<< ((i == tableS.size() - 1) ? ";" : "; ");
    }
    outputS << endl;

    // Set-Associative Cache with Next-line Prefetching
    for (int i = 0; i < assocN.size(); i++)
    {
        outputS << cacheI.prefetchingOM(assocN.at(i)) << "," << cacheI.entryC()<< ((i == tableS.size() - 1) ? ";" : "; ");
    }
    outputS << endl;
    for (int i = 0; i < assocN.size(); i++)
    {
        outputS << cacheI.setP(assocN.at(i)) << "," << cacheI.entryC()<< ((i == tableS.size() - 1) ? ";" : "; ");
    }
    outputS << endl;
    outputS.close();
}