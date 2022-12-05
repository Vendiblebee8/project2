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
vector<int> tableS = {32, 128, 512, 1024};
vector<int> assocN = {2, 4, 8, 16};

void Simulator(Cache cacheI, string Output)
{
    ofstream outputS(Output);
    if (outputS.is_open()==false)
    {
        exit(2);
    }

    // Direct-Mapped Cache
    for (int i = 0; i < tableS.size(); i++)
    {
        if((i == tableS.size() - 1)){
            outputS << cacheI.dirM(tableS.at(i)) << "," << cacheI.entryC()<< ";";
        }
        else{
            outputS << cacheI.dirM(tableS.at(i)) << "," << cacheI.entryC()<< "; ";
        }
    }
    outputS << endl;

    // setP
    for (int i = 0; i < assocN.size(); i++)
    {
        if((i == tableS.size() - 1)){
            outputS << cacheI.setA(assocN.at(i)) << "," << cacheI.entryC()<< ";";
        }
        else{
            outputS << cacheI.setA(assocN.at(i)) << "," << cacheI.entryC()<< "; ";
        }
    }
    outputS << endl;

    // Fully-Associative cache
    outputS << cacheI.assocL() << "," << cacheI.entryC() << ";" << endl;
    outputS << cacheI.assocH() << "," << cacheI.entryC() << ";" << endl;

    // Set-Associative Cache with no Allocation on a Write Miss
    for (int i = 0; i < assocN.size(); i++)
    {
        if((i == tableS.size() - 1)){
            outputS << cacheI.writeM(assocN.at(i)) << "," << cacheI.entryC()<< ";";
        }
        else{
            outputS << cacheI.writeM(assocN.at(i)) << "," << cacheI.entryC()<< "; ";
        }
    }
    outputS << endl;

    // Set-Associative Cache with Next-line Prefetching
    for (int i = 0; i < assocN.size(); i++)
    {
        if((i == tableS.size() - 1)){
            outputS << cacheI.prefetchingOM(assocN.at(i)) << "," << cacheI.entryC()<< ";";
        }
        else{
            outputS << cacheI.prefetchingOM(assocN.at(i)) << "," << cacheI.entryC()<< "; ";
        }
    }
    outputS << endl;
    for (int i = 0; i < assocN.size(); i++)
    {
        if((i == tableS.size() - 1)){
            outputS << cacheI.setP(assocN.at(i)) << "," << cacheI.entryC()<< ";";
        }
        else{
            outputS << cacheI.setP(assocN.at(i)) << "," << cacheI.entryC()<< "; ";
        }
    }
    outputS << endl;
    outputS.close();
}

int main(int argc, char *argv[])
{
	vector< pair<char, int> > vectorI;

    if (argc != 3)
    {
        exit(1);
    }

    ifstream inputF(argv[1]);
	string line;

	if (inputF.is_open())
	{
		while (getline(inputF, line))
		{
			char actionL = line.at(0);
			string addressL = line.substr(2);
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

