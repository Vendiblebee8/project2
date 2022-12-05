#include "cache.h"
#include <cmath>
#include <climits>

#define BLOCK_SIZE 32

Cache::Cache(vector<pair<char, int> > aooInputs) : gooInputs(aooInputs)
{

}

int Cache::getCountEntries()
{
    return gooInputs.size();
}

int Cache::directMapped(int anCacheEntries)
{
    /*
     * Size = Indexes * Data.Size = anCacheEntries * 2^5 bytes
     * 2^5 entries with 1KB (2^10) as data size is 2^5
     * 2^7 entries with 4KB (2^12) as data size is 2^5
     * 2^9 entries with 16KB (2^14) as data size is 2^5
     * 2^10 entries with 32KB (2^15) as data size is 2^5
     */

    int lnHit = 0;

    // Dynamic allocation of 2D array
    int **lanPageTable = new int*[anCacheEntries];
    for (int i = 0; i < anCacheEntries; i++)
    {
        lanPageTable[i] = new int[2];
    }

    // Zero out new 2D array
    for (int i = 0; i < anCacheEntries; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            lanPageTable[i][j] = 0;
        }
    }

    // Index.size + Offset.size
    int lnBitsToOffset = (int)(log2(anCacheEntries) + log2(BLOCK_SIZE));
    for (auto loIterator = gooInputs.begin(); loIterator != gooInputs.end(); loIterator++)
    {
        int lnBlockAddress = floor(loIterator->second / BLOCK_SIZE);
        int lnPTIndex = lnBlockAddress % anCacheEntries;
        int lnPTTag = loIterator->second >> lnBitsToOffset;

        // Page is Valid
        if (lanPageTable[lnPTIndex][0] == 1)
        {
            // Tag is a match
            if (lanPageTable[lnPTIndex][1] == lnPTTag)
            {
                lnHit++;
            }
            else
            {
                lanPageTable[lnPTIndex][1] = lnPTTag;
                lanPageTable[lnPTIndex][0] = 1;
            }
        }
        else
        {
            lanPageTable[lnPTIndex][1] = lnPTTag;
            lanPageTable[lnPTIndex][0] = 1;
        }
    }

    // Free dynamic 2D array
    for (int i = 0; i < anCacheEntries; i++)
    {
        delete[] lanPageTable[i];
    }
    delete[] lanPageTable;

    return lnHit;
}

int Cache::setAssociative(int anAssociativityEntries)
{
    /*
     * Set-Associative Cache.
     * Again, assume that the cache line size is 32 bytes and
     * model a 16KB cache with
     * associativity of 2, 4, 8 and 16
     * Assume that the least recently used (LRU) replacement policy is implemented.
     */

    // 2^9 entries w/ 2^5 line size = 16KB cache
    const int lnCacheRowEntries = 512 / anAssociativityEntries;

    // Row will be [VALID | TAG | timestamp | VALID | TAG | timestamp | ...]
    const int lnCacheRowLength = 3 * anAssociativityEntries;

    int lnHit = 0;

    // Dynamic allocation of 2D array
    int **lanPageTable = new int*[lnCacheRowEntries];
    for (int i = 0; i < lnCacheRowEntries; i++)
    {
        lanPageTable[i] = new int[lnCacheRowLength];
    }

    // Zero out new 2D array
    for (int i = 0; i < lnCacheRowEntries; i++)
    {
        for (int j = 0; j < lnCacheRowLength; j++)
        {
            lanPageTable[i][j] = 0;
        }
    }

    int lnBitsToOffset = (int)(log2(lnCacheRowEntries) + log2(BLOCK_SIZE));
    for (auto loIterator = gooInputs.begin(); loIterator != gooInputs.end(); loIterator++)
    {
        int lnBlockAddress = floor(loIterator->second / BLOCK_SIZE);
        int lnPTIndex = lnBlockAddress % lnCacheRowEntries;

        // Index.size + Offset.size
        int lnPTTag = loIterator->second >> lnBitsToOffset;

        // Iterate through each "way" in the cache row
        int *lanPageTableRow = lanPageTable[lnPTIndex];
        bool lbEntryFound = false;
        for (int i = 0; i < lnCacheRowLength; i += lnCacheRowLength / anAssociativityEntries)
        {
            if (lanPageTableRow[i] == 1)
            {
                if (lanPageTableRow[i + 1] == lnPTTag)
                {
                    lnHit++;
                    lanPageTableRow[i + 2] = distance(gooInputs.begin(), loIterator);
                    lbEntryFound = true;
                    break; // Exit the for() as we have a hit
                }
            }
        }

        // If entry not found, will need to import it to the cache
        if (! lbEntryFound)
        {
            // We will need to now implement the least recently used policy
            bool lbEntryInputted = false;

            // If there's an invalid entry, can write in that location
            for (int i = 0; i < lnCacheRowLength; i += lnCacheRowLength / anAssociativityEntries)
            {
                // Page is invalid, so we can write here
                if (lanPageTableRow[i] == 0)
                {
                    lanPageTableRow[i] = 1;
                    lanPageTableRow[i + 1] = lnPTTag;
                    lanPageTableRow[i + 2] = distance(gooInputs.begin(), loIterator);
                    lbEntryInputted = true;
                    break;
                }
            }

            if (! lbEntryInputted)
            {
                // Find the least recently used and overwrite it
                int lnLeastRecentlyUsedIndex = 0;
                int lnMinDistance = INT32_MAX;
                for (int i = 0; i < lnCacheRowLength; i += lnCacheRowLength / anAssociativityEntries)
                {
                    if (lanPageTableRow[i + 2] < lnMinDistance)
                    {
                        lnMinDistance = lanPageTableRow[i + 2];
                        lnLeastRecentlyUsedIndex = i;
                    }
                }

                //lanPageTableRow[lnLeastRecentlyUsedIndex] = 1; // Already Valid
                lanPageTableRow[lnLeastRecentlyUsedIndex + 1] = lnPTTag;
                lanPageTableRow[lnLeastRecentlyUsedIndex + 2] = distance(gooInputs.begin(), loIterator);
            }
        }
    }

    // Free dynamic 2D array
    for (int i = 0; i < lnCacheRowEntries; i++)
    {
        delete[] lanPageTable[i];
    }
    delete[] lanPageTable;

    return lnHit;
}

int Cache::fullAssociativeLRU()
{
    /*
     * Fully-Associative cache.
     * Assume that each cache line is 32 bytes
     * the total cache size is 16KB.
     * Implement Least Recently Used (LRU)
     */

    return setAssociative(512);
}

int Cache::fullAssociativeHCR()
{
    /*
     * Fully-Associative cache.
     * Assume that each cache line is 32 bytes
     * the total cache size is 16KB.
     * Hot-cold LRU approximation policies.
     */

    // 2^9 entries w/ 2^5 line size = 16KB cache
    const int lnWayCount = 512;

    int lnHit = 0;

    // Dynamic allocation of arrays
    int *lanPageTable = new int[lnWayCount];
    int *lanHotColdTable = new int[lnWayCount];

    for (int i = 0; i < lnWayCount; i++)
    {
        lanPageTable[i] = 0;
        lanHotColdTable[i] = 0;
    }

    for (auto loIterator = gooInputs.begin(); loIterator != gooInputs.end(); loIterator++)
    {
        int lnPTTag = loIterator->second >> (int)(log2(BLOCK_SIZE));

        // Iterate through each "way" in the cache
        bool lbEntryFound = false;
        for (int i = 0; i < lnWayCount; i++)
        {
            if (lanPageTable[i] == lnPTTag)
            {
                lnHit++;

                int lnParent = 0;
                for (int lnRaise = 0; lnRaise < log2(lnWayCount); lnRaise++)
                {
                    int lnOldValue = (i/(lnWayCount/(2 << lnRaise))) % 2;

                    if (lnOldValue == 0)
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
                break; // Exit the for() as we have a hit
            }
        }

        // If entry not found, will need to import it to the cache
        if (! lbEntryFound)
        {
            int lnIndexToReplace = 0;
            for (int i = 0; i < log2(lnWayCount); i++)
            {
                if (lanHotColdTable[lnIndexToReplace] == 0)
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
            lanPageTable[lnIndexToReplace] = lnPTTag;
        }
    }

    // Free dynamic arrays
    delete[] lanPageTable;
    delete[] lanHotColdTable;

    return lnHit;
}

int Cache::noAllocWriteMiss(int anAssociativityEntries)
{
    // In this design,
    // if a store instruction misses into the cache,
    // then the missing line is not written into the cache,
    // but instead is written directly to memory.
    // Evaluate this design for the same configurations as in question (2) above.

    // 2^9 entries w/ 2^5 line size = 16KB cache
    const int lnCacheRowEntries = 512 / anAssociativityEntries;

    // Row will be [VALID | TAG | timestamp | VALID | TAG | timestamp | ...]
    const int lnCacheRowLength = 3 * anAssociativityEntries;

    int lnHit = 0;

    // Dynamic allocation of 2D array
    int **lanPageTable = new int*[lnCacheRowEntries];
    for (int i = 0; i < lnCacheRowEntries; i++)
    {
        lanPageTable[i] = new int[lnCacheRowLength];
    }

    // Zero out new 2D array
    for (int i = 0; i < lnCacheRowEntries; i++)
    {
        for (int j = 0; j < lnCacheRowLength; j++)
        {
            lanPageTable[i][j] = 0;
        }
    }

    int lnBitsToOffset = (int)(log2(lnCacheRowEntries) + log2(BLOCK_SIZE));
    for (auto loIterator = gooInputs.begin(); loIterator != gooInputs.end(); loIterator++)
    {
        int lnBlockAddress = floor(loIterator->second / BLOCK_SIZE);
        int lnPTIndex = lnBlockAddress % lnCacheRowEntries;

        // Index.size + Offset.size
        int lnPTTag = loIterator->second >> lnBitsToOffset;

        // Iterate through each "way" in the cache row
        int *lanPageTableRow = lanPageTable[lnPTIndex];
        bool lbEntryFound = false;
        for (int i = 0; i < lnCacheRowLength; i += lnCacheRowLength / anAssociativityEntries)
        {
            if (lanPageTableRow[i] == 1)
            {
                if (lanPageTableRow[i + 1] == lnPTTag)
                {
                    lnHit++;
                    lanPageTableRow[i + 2] = distance(gooInputs.begin(), loIterator);
                    lbEntryFound = true;
                    break; // Exit the for() as we have a hit
                }
            }
        }

        // If entry not found, will need to import it to the cache
        if (! lbEntryFound)
        {
            // We will need to now implement the least recently used policy
            bool lbEntryInputted = false;

            // We will check if it's a store instruction. If it is, skip
            if (loIterator->first == 'S')
            {
                // Store instruction, so write to memory and skip cache
                lbEntryInputted = true;
            }

            // If there's an invalid entry, can write in that location
            for (int i = 0; i < lnCacheRowLength && !lbEntryInputted; i += lnCacheRowLength / anAssociativityEntries)
            {
                // Page is invalid, so we can write here
                if (lanPageTableRow[i] == 0)
                {
                    lanPageTableRow[i] = 1;
                    lanPageTableRow[i + 1] = lnPTTag;
                    lanPageTableRow[i + 2] = distance(gooInputs.begin(), loIterator);
                    lbEntryInputted = true;
                    break;
                }
            }

            if (! lbEntryInputted)
            {
                // Find the least recently used and overwrite it
                int lnLeastRecentlyUsedIndex = 0;
                int lnMinDistance = INT32_MAX;
                for (int i = 0; i < lnCacheRowLength; i += lnCacheRowLength / anAssociativityEntries)
                {
                    if (lanPageTableRow[i + 2] < lnMinDistance)
                    {
                        lnMinDistance = lanPageTableRow[i + 2];
                        lnLeastRecentlyUsedIndex = i;
                    }
                }

                //lanPageTableRow[lnLeastRecentlyUsedIndex] = 1; // Already Valid
                lanPageTableRow[lnLeastRecentlyUsedIndex + 1] = lnPTTag;
                lanPageTableRow[lnLeastRecentlyUsedIndex + 2] = distance(gooInputs.begin(), loIterator);
            }
        }
    }

    // Free dynamic 2D array
    for (int i = 0; i < lnCacheRowEntries; i++)
    {
        delete[] lanPageTable[i];
    }
    delete[] lanPageTable;

    return lnHit;
}

int Cache::setAssociativePrefetching(int anAssociativityEntries)
{
    // In this design,
    // the next memory line will be brought into the cache with every cache access.
    // For example, if current access is to line X,
    // then line (x+1) is also brought into the cache,
    // replacing the cache’s previous content.
    // Evaluate this design for the same configurations as in question (2) above.

    // 2^9 entries w/ 2^5 line size = 16KB cache
    const int lnCacheRowEntries = 512 / anAssociativityEntries;

    // Row will be [TAG | timestamp | TAG | timestamp | ...]
    const int lnCacheRowLength = 2 * anAssociativityEntries;

    int lnHit = 0;

    // Dynamic allocation of 2D array
    int **lanPageTable = new int*[lnCacheRowEntries];
    for (int i = 0; i < lnCacheRowEntries; i++)
    {
        lanPageTable[i] = new int[lnCacheRowLength];
    }

    // Fill in new 2D Array
    auto loIterator = gooInputs.begin();
    for (int i = 0; i < lnCacheRowEntries; i++)
    {
        for (int j = 0; j < lnCacheRowLength; j += 2)
        {
            lanPageTable[i][j] = loIterator->second;
            lanPageTable[i][j+1] = -1;
            loIterator++;
        }
    }

    int lnBitsToOffset = (int)(log2(lnCacheRowEntries) + log2(BLOCK_SIZE));
    for (auto loIterator = gooInputs.begin(); loIterator != gooInputs.end(); loIterator++)
    {
        int lnBlockAddress = floor(loIterator->second / BLOCK_SIZE);
        int lnPTIndex = lnBlockAddress % lnCacheRowEntries;
        int lnPTTag = loIterator->second >> lnBitsToOffset;

        int lnPreFetchPTIndex = (lnBlockAddress + 1) % lnCacheRowEntries;
        int lnPreFetchPTTag = (loIterator->second + BLOCK_SIZE) >> lnBitsToOffset;

        // Iterate through each "way" in the cache row
        int *lanPageTableRow = lanPageTable[lnPTIndex];
        int *lanPreFetchPTR = lanPageTable[lnPreFetchPTIndex];

        bool lbEntryFound = false;
        bool lbPreFetchEntryFound = false;

        for (int i = 0; i < lnCacheRowLength; i += 2)
        {
            if (lanPageTableRow[i] == lnPTTag)
            {
                lnHit++;
                lanPageTableRow[i + 1] = distance(gooInputs.begin(), loIterator);
                lbEntryFound = true;
                break; // Exit the for() as we have a hit
            }
        }

        for (int i = 0; i < lnCacheRowLength; i += 2)
        {
            if (lanPreFetchPTR[i] == lnPreFetchPTTag)
            {
                lanPreFetchPTR[i + 1] = distance(gooInputs.begin(), loIterator);
                lbPreFetchEntryFound = true;
                break;
            }
        }

        // If entry not found, will need to import it to the cache
        if (! lbEntryFound)
        {
            int lnIndexToReplaceValue = lanPageTableRow[1];
            int lnIndexToReplace = 0;

            for (int i = 0; i < lnCacheRowLength; i += 2)
            {
                if (lanPageTableRow[i+1] < lnIndexToReplaceValue)
                {
                    lnIndexToReplaceValue = lanPageTableRow[i+1];
                    lnIndexToReplace = i;
                }
            }

            lanPageTableRow[lnIndexToReplace] = lnPTTag;
            lanPageTableRow[lnIndexToReplace + 1] = distance(gooInputs.begin(), loIterator);
        }

        if (! lbPreFetchEntryFound)
        {
            int lnIndexToReplaceValue = lanPreFetchPTR[1];
            int lnIndexToReplace = 0;

            for (int i = 0; i < lnCacheRowLength; i += 2)
            {
                if (lanPreFetchPTR[i+1] < lnIndexToReplaceValue)
                {
                    lnIndexToReplaceValue = lanPreFetchPTR[i+1];
                    lnIndexToReplace = i;
                }
            }

            lanPreFetchPTR[lnIndexToReplace] = lnPreFetchPTTag;
            lanPreFetchPTR[lnIndexToReplace + 1] = distance(gooInputs.begin(), loIterator);
        }
    }

    // Free dynamic 2D array
    for (int i = 0; i < lnCacheRowEntries; i++)
    {
        delete[] lanPageTable[i];
    }
    delete[] lanPageTable;

    return lnHit;
}

int Cache::setAssociativePreOnMiss(int anAssociativityEntries)
{
    // 2^9 entries w/ 2^5 line size = 16KB cache
    const int lnCacheRowEntries = 512 / anAssociativityEntries;

    // Row will be [TAG | timestamp | TAG | timestamp | ...]
    const int lnCacheRowLength = 2 * anAssociativityEntries;

    int lnHit = 0;

    // Dynamic allocation of 2D array
    int **lanPageTable = new int*[lnCacheRowEntries];
    for (int i = 0; i < lnCacheRowEntries; i++)
    {
        lanPageTable[i] = new int[lnCacheRowLength];
    }

    // Fill in new 2D Array
    auto loIterator = gooInputs.begin();
    for (int i = 0; i < lnCacheRowEntries; i++)
    {
        for (int j = 0; j < lnCacheRowLength; j += 2)
        {
            lanPageTable[i][j] = loIterator->second;
            lanPageTable[i][j+1] = -1;
            loIterator++;
        }
    }

    int lnBitsToOffset = (int)(log2(lnCacheRowEntries) + log2(BLOCK_SIZE));
    for (auto loIterator = gooInputs.begin(); loIterator != gooInputs.end(); loIterator++)
    {
        int lnBlockAddress = floor(loIterator->second / BLOCK_SIZE);
        int lnPTIndex = lnBlockAddress % lnCacheRowEntries;
        int lnPTTag = loIterator->second >> lnBitsToOffset;

        int lnPreFetchPTIndex = (lnBlockAddress + 1) % lnCacheRowEntries;
        int lnPreFetchPTTag = (loIterator->second + BLOCK_SIZE) >> lnBitsToOffset;

        // Iterate through each "way" in the cache row
        int *lanPageTableRow = lanPageTable[lnPTIndex];
        int *lanPreFetchPTR = lanPageTable[lnPreFetchPTIndex];

        bool lbEntryFound = false;
        bool lbPreFetchEntryFound = false;

        for (int i = 0; i < lnCacheRowLength; i += 2)
        {
            if (lanPageTableRow[i] == lnPTTag)
            {
                lnHit++;
                lanPageTableRow[i + 1] = distance(gooInputs.begin(), loIterator);
                lbEntryFound = true;
                break; // Exit the for() as we have a hit
            }
        }

        // If entry not found, will need to import it to the cache
        if (! lbEntryFound)
        {
            for (int i = 0; i < lnCacheRowLength; i += 2)
            {
                if (lanPreFetchPTR[i] == lnPreFetchPTTag)
                {
                    lanPreFetchPTR[i + 1] = distance(gooInputs.begin(), loIterator);
                    lbPreFetchEntryFound = true;
                    break;
                }
            }

            int lnIndexToReplaceValue = lanPageTableRow[1];
            int lnIndexToReplace = 0;

            for (int i = 0; i < lnCacheRowLength; i += 2)
            {
                if (lanPageTableRow[i+1] < lnIndexToReplaceValue)
                {
                    lnIndexToReplaceValue = lanPageTableRow[i+1];
                    lnIndexToReplace = i;
                }
            }

            lanPageTableRow[lnIndexToReplace] = lnPTTag;
            lanPageTableRow[lnIndexToReplace + 1] = distance(gooInputs.begin(), loIterator);
        }

        if (! lbPreFetchEntryFound && ! lbEntryFound)
        {
            int lnIndexToReplaceValue = lanPreFetchPTR[1];
            int lnIndexToReplace = 0;

            for (int i = 0; i < lnCacheRowLength; i += 2)
            {
                if (lanPreFetchPTR[i+1] < lnIndexToReplaceValue)
                {
                    lnIndexToReplaceValue = lanPreFetchPTR[i+1];
                    lnIndexToReplace = i;
                }
            }

            lanPreFetchPTR[lnIndexToReplace] = lnPreFetchPTTag;
            lanPreFetchPTR[lnIndexToReplace + 1] = distance(gooInputs.begin(), loIterator);
        }
    }

    // Free dynamic 2D array
    for (int i = 0; i < lnCacheRowEntries; i++)
    {
        delete[] lanPageTable[i];
    }
    delete[] lanPageTable;

    return lnHit;
}