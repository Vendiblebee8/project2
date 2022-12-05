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
    const int lnCacheRowLength = 3 * entriesA;
    hits = 0;
    int **pageT = new int*[cacheR];
    for(int i = 0; i < cacheR; i++)
    {
        pageT[i] = new int[lnCacheRowLength];
    }
    for(int i = 0; i < cacheR; i++)
    {
        for(int j = 0; j < lnCacheRowLength; j++)
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
        int *pageTRow = pageT[pageL];
        bool lbEntryFound = false;
        for(int i = 0; i < lnCacheRowLength; i += lnCacheRowLength / entriesA)
        {
            if(pageTRow[i] == 1)
            {
                if(pageTRow[i + 1] == pageTT)
                {
                    hits++;
                    pageTRow[i + 2] = distance(inputsG.begin(), cur);
                    lbEntryFound = true;
                    break; // Exit the for() as we have a hit
                }
            }
        }
        if(lbEntryFound== false)
        {
            bool lbEntryInputted = false;
            for(int i = 0; i < lnCacheRowLength; i += lnCacheRowLength / entriesA)
            {
                if(pageTRow[i] == 0)
                {
                    pageTRow[i] = 1;
                    pageTRow[i + 1] = pageTT;
                    pageTRow[i + 2] = distance(inputsG.begin(), cur);
                    lbEntryInputted = true;
                    break;
                }
            }

            if(lbEntryInputted == false)
            {
                int lnLeastRecentlyUsedIndex = 0;
                int lnMinDistance = INT32_MAX;
                for(int i = 0; i < lnCacheRowLength; i += lnCacheRowLength / entriesA)
                {
                    if(pageTRow[i + 2] < lnMinDistance)
                    {
                        lnMinDistance = pageTRow[i + 2];
                        lnLeastRecentlyUsedIndex = i;
                    }
                }
                pageTRow[lnLeastRecentlyUsedIndex + 1] = pageTT;
                pageTRow[lnLeastRecentlyUsedIndex + 2] = distance(inputsG.begin(), cur);
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
    const int lnWayCount = 512;
    hits = 0;
    int *pageT = new int[lnWayCount];
    int *lanHotColdTable = new int[lnWayCount];

    for(int i = 0; i < lnWayCount; i++)
    {
        pageT[i] = 0;
        lanHotColdTable[i] = 0;
    }

    for(auto cur = inputsG.begin(); cur != inputsG.end(); cur++)
    {
        int pageTT = cur->second >> (int)(log2(BLOCK_SIZE));
        bool lbEntryFound = false;

        for(int i = 0; i < lnWayCount; i++)
        {
            if(pageT[i] == pageTT)
            {
                hits++;
                int lnParent = 0;
                for(int lnRaise = 0; lnRaise < log2(lnWayCount); lnRaise++)
                {
                    int lnOldValue = (i/(lnWayCount/(2 << lnRaise))) % 2;

                    if(lnOldValue == 0)
                    {
                        lanHotColdTable[lnParent] = 1;
                        lnParent = (2 * lnParent) + 1;
                    }
                    else
                    {
                        lanHotColdTable[lnParent] = 0;
                        lnParent = 2 * (lnParent + 1);
                    }
                }

                lbEntryFound = true;
                break;
            }
        }
        if(lbEntryFound== false)
        {
            int lnIndexToReplace = 0;

            for(int i = 0; i < log2(lnWayCount); i++)
            {
                if(lanHotColdTable[lnIndexToReplace] == 0)
                {
                    lanHotColdTable[lnIndexToReplace] = 1;
                    lnIndexToReplace = (2 * lnIndexToReplace) + 1;
                }
                else
                {
                    lanHotColdTable[lnIndexToReplace] = 0;
                    lnIndexToReplace = 2 * (lnIndexToReplace + 1);
                }
            }

            lnIndexToReplace = lnIndexToReplace + 1 - lnWayCount;
            pageT[lnIndexToReplace] = pageTT;
        }
    }

    delete[] pageT;
    delete[] lanHotColdTable;
    return hits;
}

int Cache::writeM(int entriesA)
{
    const int cacheR = 512 / entriesA;
    const int lnCacheRowLength = 3 * entriesA;
    hits = 0;
    int **pageT = new int*[cacheR];

    for(int i = 0; i < cacheR; i++)
    {
        pageT[i] = new int[lnCacheRowLength];
    }
    for(int i = 0; i < cacheR; i++)
    {
        for(int j = 0; j < lnCacheRowLength; j++)
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
        int *pageTRow = pageT[pageL];
        bool lbEntryFound = false;

        for(int i = 0; i < lnCacheRowLength; i += lnCacheRowLength / entriesA)
        {
            if(pageTRow[i] == 1)
            {
                if(pageTRow[i + 1] == pageTT)
                {
                    hits++;
                    pageTRow[i + 2] = distance(inputsG.begin(), cur);
                    lbEntryFound = true;
                    break; // Exit the for() as we have a hit
                }
            }
        }
        if(lbEntryFound== false)
        {
            bool lbEntryInputted = false;

            if(cur->first == 'S')
            {
                lbEntryInputted = true;
            }

            for(int i = 0; i < lnCacheRowLength && lbEntryInputted == false; i += lnCacheRowLength / entriesA)
            {
                if(pageTRow[i] == 0)
                {
                    pageTRow[i] = 1;
                    pageTRow[i + 1] = pageTT;
                    pageTRow[i + 2] = distance(inputsG.begin(), cur);
                    lbEntryInputted = true;
                    break;
                }
            }

            if(lbEntryInputted == false)
            {
                int lnLeastRecentlyUsedIndex = 0;
                int lnMinDistance = INT32_MAX;

                for(int i = 0; i < lnCacheRowLength; i += lnCacheRowLength / entriesA)
                {
                    if(pageTRow[i + 2] < lnMinDistance)
                    {
                        lnMinDistance = pageTRow[i + 2];
                        lnLeastRecentlyUsedIndex = i;
                    }
                }
                pageTRow[lnLeastRecentlyUsedIndex + 1] = pageTT;
                pageTRow[lnLeastRecentlyUsedIndex + 2] = distance(inputsG.begin(), cur);
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
    const int lnCacheRowLength = 2 * entriesA;
    hits = 0;
    int **pageT = new int*[cacheR];

    for(int i = 0; i < cacheR; i++)
    {
        pageT[i] = new int[lnCacheRowLength];
    }
    auto cur = inputsG.begin();
    for(int i = 0; i < cacheR; i++)
    {
        for(int j = 0; j < lnCacheRowLength; j += 2)
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
        int lnPreFetchPTIndex = (blockA + 1) % cacheR;
        int lnPreFetchPTTag = (cur->second + BLOCK_SIZE) >> offset;
        int *pageTRow = pageT[pageL];
        int *lanPreFetchPTR = pageT[lnPreFetchPTIndex];
        bool lbEntryFound = false;
        bool lbPreFetchEntryFound = false;

        for(int i = 0; i < lnCacheRowLength; i += 2)
        {
            if(pageTRow[i] == pageTT)
            {
                hits++;
                pageTRow[i + 1] = distance(inputsG.begin(), cur);
                lbEntryFound = true;
                break; // Exit the for() as we have a hit
            }
        }

        for(int i = 0; i < lnCacheRowLength; i += 2)
        {
            if(lanPreFetchPTR[i] == lnPreFetchPTTag)
            {
                lanPreFetchPTR[i + 1] = distance(inputsG.begin(), cur);
                lbPreFetchEntryFound = true;
                break;
            }
        }
        if(lbEntryFound== false)
        {
            int lnIndexToReplaceValue = pageTRow[1];
            int lnIndexToReplace = 0;

            for(int i = 0; i < lnCacheRowLength; i += 2)
            {
                if(pageTRow[i+1] < lnIndexToReplaceValue)
                {
                    lnIndexToReplaceValue = pageTRow[i+1];
                    lnIndexToReplace = i;
                }
            }

            pageTRow[lnIndexToReplace] = pageTT;
            pageTRow[lnIndexToReplace + 1] = distance(inputsG.begin(), cur);
        }

        if(lbPreFetchEntryFound== false)
        {
            int lnIndexToReplaceValue = lanPreFetchPTR[1];
            int lnIndexToReplace = 0;

            for(int i = 0; i < lnCacheRowLength; i += 2)
            {
                if(lanPreFetchPTR[i+1] < lnIndexToReplaceValue)
                {
                    lnIndexToReplaceValue = lanPreFetchPTR[i+1];
                    lnIndexToReplace = i;
                }
            }

            lanPreFetchPTR[lnIndexToReplace] = lnPreFetchPTTag;
            lanPreFetchPTR[lnIndexToReplace + 1] = distance(inputsG.begin(), cur);
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
    const int lnCacheRowLength = 2 * entriesA;
    hits = 0;
    int **pageT = new int*[cacheR];

    for(int i = 0; i < cacheR; i++)
    {
        pageT[i] = new int[lnCacheRowLength];
    }
    auto cur = inputsG.begin();
    for(int i = 0; i < cacheR; i++)
    {
        for(int j = 0; j < lnCacheRowLength; j += 2)
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
        int lnPreFetchPTIndex = (blockA + 1) % cacheR;
        int lnPreFetchPTTag = (cur->second + BLOCK_SIZE) >> offset;
        int *pageTRow = pageT[pageL];
        int *lanPreFetchPTR = pageT[lnPreFetchPTIndex];
        bool lbEntryFound = false;
        bool lbPreFetchEntryFound = false;

        for(int i = 0; i < lnCacheRowLength; i += 2)
        {
            if(pageTRow[i] == pageTT)
            {
                hits++;
                pageTRow[i + 1] = distance(inputsG.begin(), cur);
                lbEntryFound = true;
                break; // Exit the for() as we have a hit
            }
        }
        if(lbEntryFound== false)
        {
            for(int i = 0; i < lnCacheRowLength; i += 2)
            {
                if(lanPreFetchPTR[i] == lnPreFetchPTTag)
                {
                    lanPreFetchPTR[i + 1] = distance(inputsG.begin(), cur);
                    lbPreFetchEntryFound = true;
                    break;
                }
            }

            int lnIndexToReplaceValue = pageTRow[1];
            int lnIndexToReplace = 0;

            for(int i = 0; i < lnCacheRowLength; i += 2)
            {
                if(pageTRow[i+1] < lnIndexToReplaceValue)
                {
                    lnIndexToReplaceValue = pageTRow[i+1];
                    lnIndexToReplace = i;
                }
            }

            pageTRow[lnIndexToReplace] = pageTT;
            pageTRow[lnIndexToReplace + 1] = distance(inputsG.begin(), cur);
        }

        if(lbPreFetchEntryFound== false && lbEntryFound== false)
        {
            int lnIndexToReplaceValue = lanPreFetchPTR[1];
            int lnIndexToReplace = 0;

            for(int i = 0; i < lnCacheRowLength; i += 2)
            {
                if(lanPreFetchPTR[i+1] < lnIndexToReplaceValue)
                {
                    lnIndexToReplaceValue = lanPreFetchPTR[i+1];
                    lnIndexToReplace = i;
                }
            }

            lanPreFetchPTR[lnIndexToReplace] = lnPreFetchPTTag;
            lanPreFetchPTR[lnIndexToReplace + 1] = distance(inputsG.begin(), cur);
        }
    }

    for(int i = 0; i < cacheR; i++)
    {
        delete[] pageT[i];
    }
    delete[] pageT;
    return hits;
}