/*
* Copyright (c) 2017, Private Octopus, Inc.
* All rights reserved.
*
* Permission to use, copy, modify, and distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Private Octopus, Inc. BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>
#include <string.h>
#include "WriteOnceHash.h"

WriteOnceHash::WriteOnceHash()
    :
    tableSize(0),
    tableCount(0),
    hashTable(NULL),
    valueTable(NULL)
{
}


WriteOnceHash::~WriteOnceHash()
{
    Clear();
}

void WriteOnceHash::Clear()
{
    if (hashTable != NULL)
    {
        delete[] hashTable;
        hashTable = NULL;
    }

    if (valueTable != NULL)
    {
        delete[] valueTable;
        valueTable = NULL;
    }
    tableSize = 0;
}

bool WriteOnceHash::Resize(unsigned newSize)
{
    bool ret = false;
    unsigned long long * oldTable = hashTable;
    unsigned long long * oldValue = valueTable;
    unsigned int oldSize = tableSize;

    if (oldSize >= newSize)
    {
        ret = true;
    }
    else
    {
        unsigned long long * newTable = new unsigned long long[newSize];
        unsigned long long * newValue = new unsigned long long[newSize];

        if (newTable != NULL && newValue != NULL)
        {
            hashTable = newTable;
            valueTable = newValue;
            tableSize = newSize;
            memset(hashTable, 0, sizeof(unsigned long long)*tableSize);
            memset(valueTable, 0, sizeof(unsigned long long)*tableSize);
            ret = true;
            tableCount = 0;

            if (oldTable != NULL)
            {
                for (unsigned int i = 0; ret && i < oldSize; i++)
                {
                    if (oldTable[i] != 0)
                    {
                        ret = DoInsert(oldTable[i], oldValue[i]);
                    }
                }

                if (!ret)
                {
                    hashTable = oldTable;
                    valueTable = oldValue;
                    tableSize = oldSize;
                    delete[] newTable;
                    delete[] newValue;
                }
                else
                {
                    delete[] oldTable;
                    delete[] oldValue;
                }
            }
        }
    }

    return ret;
}


bool WriteOnceHash::Insert(unsigned long long key, unsigned long long value)
{
    bool ret = true;
    unsigned int newCount = tableCount+1;

    if (key == 0)
    {
        ret = false;
    }
    else if (tableSize < 2 * newCount)
    {
        unsigned int newSize = tableSize;

        if (tableSize == 0)
        {
            newSize = 128;
        }

        while (newSize < 4* newCount)
        {
            newSize *= 2;
        }

        ret = Resize(newSize);
    }

    if (ret)
    {
        ret = DoInsert(key, value);
    }

    return ret;
}

bool WriteOnceHash::DoInsert(unsigned long long key, unsigned long long value)
{
    bool ret = false;
    unsigned int hash_index = key%tableSize;

    for (unsigned int i = 0; i < tableSize; i++)
    {
        if (hashTable[hash_index] == 0)
        {
            hashTable[hash_index] = key;
            valueTable[hash_index] = value;
            tableCount++;
            ret = true;
            break;
        }
        else if (hashTable[hash_index] == key)
        {
            /* found it. Do not insert it twice! */
            ret = false;
        }
        else
        {
            hash_index++;

            if (hash_index >= tableSize)
            {
                hash_index = 0;
            }
        }
    }

    return ret;
}

bool WriteOnceHash::Retrieve(unsigned long long key, unsigned long long * value)
{
    bool ret = false;
    unsigned int hash_index;

    *value = 0;

    if (key != 0 && tableSize > 0)
    {
        hash_index = key%tableSize;

        for (unsigned int i = 0; i < tableSize; i++)
        {
            if (hashTable[hash_index] == 0)
            {
                /* found a hole, which means the key is not there */
                break;
            }
            else if (hashTable[hash_index] == key)
            {
                /* found it! */
                *value = valueTable[hash_index];
                ret = true;
            }
            else
            {
                /* did not find the key here */
                hash_index++;

                if (hash_index >= tableSize)
                {
                    hash_index = 0;
                }
            }
        }
    }

    return ret;
}
