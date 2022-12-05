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
#include <cmath>
#include <climits>

#define BLOCK_SIZE 32

int hits=0;

Cache::Cache(vector<pair<char, int> > inputsA) : inputsG(inputsA){

}

int Cache::entryC(){
    return inputsG.size();
}

int Cache::dirM(int cacheE){
    hits = 0;
    int **pageT = new int*[cacheE];
    for(int i = 0; i < cacheE; i++)
    {
        pageT[i] = new int[2];
    }
    for(int i = 0; i < cacheE; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            pageT[i][j] = 0;
        }
    }
    int offset = (int)(log2(cacheE) + log2(BLOCK_SIZE));
    for(auto cur = inputsG.begin(); cur != inputsG.end(); cur++)
    {
        int blockA = floor(cur->second / BLOCK_SIZE);
        int pageL = blockA % cacheE;
        int pageTT = cur->second >> offset;
        if(pageT[pageL][0] == 1)
        {
            if(pageT[pageL][1] == pageTT)
            {
                hits++;
            }
            else
            {
                pageT[pageL][1] = pageTT;
                pageT[pageL][0] = 1;
            }
        }
        else
        {
            pageT[pageL][1] = pageTT;
            pageT[pageL][0] = 1;
        }
    }

    for(int i = 0; i < cacheE; i++)
    {
        delete[] pageT[i];
    }
    delete[] pageT;

    return hits;
}

int Cache::setA(int entriesA){
    const int cacheR = 512 / entriesA;
    const int cacheRL = 3 * entriesA;
    hits = 0;
    int **pageT = new int*[cacheR];
    for(int i = 0; i < cacheR; i++)
    {
        pageT[i] = new int[cacheRL];
    }
    for(int i = 0; i < cacheR; i++)
    {
        for(int j = 0; j < cacheRL; j++)
        {
            pageT[i][j] = 0;
        }
    }
    int offset = (int)(log2(cacheR) + log2(BLOCK_SIZE));
    for(auto cur = inputsG.begin(); cur != inputsG.end(); cur++)
    {
        int blockA = floor(cur->second / BLOCK_SIZE);
        int pageL = blockA % cacheR;
        int pageTT = cur->second >> offset;
        int *pageTR = pageT[pageL];
        bool entryE = false;
        for(int i = 0; i < cacheRL; i += cacheRL / entriesA)
        {
            if(pageTR[i] == 1)
            {
                if(pageTR[i + 1] == pageTT)
                {
                    hits++;
                    pageTR[i + 2] = distance(inputsG.begin(), cur);
                    entryE = true;
                    break;
                }
            }
        }
        if(entryE== false)
        {
            bool entryEn = false;
            for(int i = 0; i < cacheRL; i += cacheRL / entriesA)
            {
                if(pageTR[i] == 0)
                {
                    pageTR[i] = 1;
                    pageTR[i + 1] = pageTT;
                    pageTR[i + 2] = distance(inputsG.begin(), cur);
                    entryEn = true;
                    break;
                }
            }

            if(entryEn == false)
            {
                int usable = 0;
                int distM = INT32_MAX;
                for(int i = 0; i < cacheRL; i += cacheRL / entriesA)
                {
                    if(pageTR[i + 2] < distM)
                    {
                        distM = pageTR[i + 2];
                        usable = i;
                    }
                }
                pageTR[usable + 1] = pageTT;
                pageTR[usable + 2] = distance(inputsG.begin(), cur);
            }
        }
    }
    for(int i = 0; i < cacheR; i++)
    {
        delete[] pageT[i];
    }
    delete[] pageT;

    return hits;
}

int Cache::assocL()
{
    return setA(512);
}

int Cache::assocH()
{
    const int wayT = 512;
    hits = 0;
    int *pageT = new int[wayT];
    int *tableL = new int[wayT];

    for(int i = 0; i < wayT; i++)
    {
        pageT[i] = 0;
        tableL[i] = 0;
    }

    for(auto cur = inputsG.begin(); cur != inputsG.end(); cur++)
    {
        int pageTT = cur->second >> (int)(log2(BLOCK_SIZE));
        bool entryE = false;

        for(int i = 0; i < wayT; i++)
        {
            if(pageT[i] == pageTT)
            {
                hits++;
                int parent = 0;
                for(int increase = 0; increase < log2(wayT); increase++)
                {
                    int prevV = (i/(wayT/(2 << increase))) % 2;

                    if(prevV == 0)
                    {
                        tableL[parent] = 1;
                        parent = (2 * parent) + 1;
                    }
                    else
                    {
                        tableL[parent] = 0;
                        parent = 2 * (parent + 1);
                    }
                }

                entryE = true;
                break;
            }
        }
        if(entryE== false)
        {
            int replaceI = 0;

            for(int i = 0; i < log2(wayT); i++)
            {
                if(tableL[replaceI] == 0)
                {
                    tableL[replaceI] = 1;
                    replaceI = (2 * replaceI) + 1;
                }
                else
                {
                    tableL[replaceI] = 0;
                    replaceI = 2 * (replaceI + 1);
                }
            }

            replaceI = replaceI + 1 - wayT;
            pageT[replaceI] = pageTT;
        }
    }

    delete[] pageT;
    delete[] tableL;
    return hits;
}

int Cache::writeM(int entriesA)
{
    const int cacheR = 512 / entriesA;
    const int cacheRL = 3 * entriesA;
    hits = 0;
    int **pageT = new int*[cacheR];

    for(int i = 0; i < cacheR; i++)
    {
        pageT[i] = new int[cacheRL];
    }
    for(int i = 0; i < cacheR; i++)
    {
        for(int j = 0; j < cacheRL; j++)
        {
            pageT[i][j] = 0;
        }
    }

    int offset = (int)(log2(cacheR) + log2(BLOCK_SIZE));

    for(auto cur = inputsG.begin(); cur != inputsG.end(); cur++)
    {
        int blockA = floor(cur->second / BLOCK_SIZE);
        int pageL = blockA % cacheR;
        int pageTT = cur->second >> offset;
        int *pageTR = pageT[pageL];
        bool entryE = false;

        for(int i = 0; i < cacheRL; i += cacheRL / entriesA)
        {
            if(pageTR[i] == 1)
            {
                if(pageTR[i + 1] == pageTT)
                {
                    hits++;
                    pageTR[i + 2] = distance(inputsG.begin(), cur);
                    entryE = true;
                    break;
                }
            }
        }
        if(entryE== false)
        {
            bool entryEn = false;

            if(cur->first == 'S')
            {
                entryEn = true;
            }

            for(int i = 0; i < cacheRL && entryEn == false; i += cacheRL / entriesA)
            {
                if(pageTR[i] == 0)
                {
                    pageTR[i] = 1;
                    pageTR[i + 1] = pageTT;
                    pageTR[i + 2] = distance(inputsG.begin(), cur);
                    entryEn = true;
                    break;
                }
            }

            if(entryEn == false)
            {
                int usable = 0;
                int distM = INT32_MAX;

                for(int i = 0; i < cacheRL; i += cacheRL / entriesA)
                {
                    if(pageTR[i + 2] < distM)
                    {
                        distM = pageTR[i + 2];
                        usable = i;
                    }
                }
                pageTR[usable + 1] = pageTT;
                pageTR[usable + 2] = distance(inputsG.begin(), cur);
            }
        }
    }

    for(int i = 0; i < cacheR; i++)
    {
        delete[] pageT[i];
    }

    delete[] pageT;
    return hits;
}

int Cache::prefetchingOM(int entriesA){
    const int cacheR = 512 / entriesA;
    const int cacheRL = 2 * entriesA;
    hits = 0;
    int **pageT = new int*[cacheR];

    for(int i = 0; i < cacheR; i++)
    {
        pageT[i] = new int[cacheRL];
    }
    auto cur = inputsG.begin();
    for(int i = 0; i < cacheR; i++)
    {
        for(int j = 0; j < cacheRL; j += 2)
        {
            pageT[i][j] = cur->second;
            pageT[i][j+1] = -1;
            cur++;
        }
    }

    int offset = (int)(log2(cacheR) + log2(BLOCK_SIZE));
    for(auto cur = inputsG.begin(); cur != inputsG.end(); cur++)
    {
        int blockA = floor(cur->second / BLOCK_SIZE);
        int pageL = blockA % cacheR;
        int pageTT = cur->second >> offset;
        int prefetchI = (blockA + 1) % cacheR;
        int prefTT = (cur->second + BLOCK_SIZE) >> offset;
        int *pageTR = pageT[pageL];
        int *prefP = pageT[prefetchI];
        bool entryE = false;
        bool entryF = false;

        for(int i = 0; i < cacheRL; i += 2)
        {
            if(pageTR[i] == pageTT)
            {
                hits++;
                pageTR[i + 1] = distance(inputsG.begin(), cur);
                entryE = true;
                break;
            }
        }

        for(int i = 0; i < cacheRL; i += 2)
        {
            if(prefP[i] == prefTT)
            {
                prefP[i + 1] = distance(inputsG.begin(), cur);
                entryF = true;
                break;
            }
        }
        if(entryE== false)
        {
            int replaceIV = pageTR[1];
            int replaceI = 0;

            for(int i = 0; i < cacheRL; i += 2)
            {
                if(pageTR[i+1] < replaceIV)
                {
                    replaceIV = pageTR[i+1];
                    replaceI = i;
                }
            }

            pageTR[replaceI] = pageTT;
            pageTR[replaceI + 1] = distance(inputsG.begin(), cur);
        }

        if(entryF== false)
        {
            int replaceIV = prefP[1];
            int replaceI = 0;

            for(int i = 0; i < cacheRL; i += 2)
            {
                if(prefP[i+1] < replaceIV)
                {
                    replaceIV = prefP[i+1];
                    replaceI = i;
                }
            }

            prefP[replaceI] = prefTT;
            prefP[replaceI + 1] = distance(inputsG.begin(), cur);
        }
    }
    for(int i = 0; i < cacheR; i++)
    {
        delete[] pageT[i];
    }

    delete[] pageT;
    return hits;
}

int Cache::setP(int entriesA){
    const int cacheR = 512 / entriesA;
    const int cacheRL = 2 * entriesA;
    hits = 0;
    int **pageT = new int*[cacheR];

    for(int i = 0; i < cacheR; i++)
    {
        pageT[i] = new int[cacheRL];
    }
    auto cur = inputsG.begin();
    for(int i = 0; i < cacheR; i++)
    {
        for(int j = 0; j < cacheRL; j += 2)
        {
            pageT[i][j] = cur->second;
            pageT[i][j+1] = -1;
            cur++;
        }
    }

    int offset = (int)(log2(cacheR) + log2(BLOCK_SIZE));

    for(auto cur = inputsG.begin(); cur != inputsG.end(); cur++)
    {
        bool entryE = false;
        bool entryF = false;
        int blockA = floor(cur->second / BLOCK_SIZE);
        int pageL = blockA % cacheR;
        int pageTT = cur->second >> offset;
        int prefetchI = (blockA + 1) % cacheR;
        int prefTT = (cur->second + BLOCK_SIZE) >> offset;
        int *pageTR = pageT[pageL];
        int *prefP = pageT[prefetchI];

        for(int i = 0; i < cacheRL; i += 2)
        {
            if(pageTR[i] == pageTT)
            {
                hits++;
                pageTR[i + 1] = distance(inputsG.begin(), cur);
                entryE = true;
                break;
            }
        }
        if(entryE== false)
        {
            for(int i = 0; i < cacheRL; i += 2)
            {
                if(prefP[i] == prefTT)
                {
                    prefP[i + 1] = distance(inputsG.begin(), cur);
                    entryF = true;
                    break;
                }
            }

            int replaceIV = pageTR[1];
            int replaceI = 0;

            for(int i = 0; i < cacheRL; i += 2)
            {
                if(pageTR[i+1] < replaceIV)
                {
                    replaceIV = pageTR[i+1];
                    replaceI = i;
                }
            }

            pageTR[replaceI] = pageTT;
            pageTR[replaceI + 1] = distance(inputsG.begin(), cur);
        }

        if(entryF== false && entryE== false)
        {
            int replaceIV = prefP[1];
            int replaceI = 0;

            for(int i = 0; i < cacheRL; i += 2)
            {
                if(prefP[i+1] < replaceIV)
                {
                    replaceIV = prefP[i+1];
                    replaceI = i;
                }
            }

            prefP[replaceI] = prefTT;
            prefP[replaceI + 1] = distance(inputsG.begin(), cur);
        }
    }

    for(int i = 0; i < cacheR; i++)
    {
        delete[] pageT[i];
    }
    delete[] pageT;
    return hits;
}