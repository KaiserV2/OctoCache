#ifndef _BOBHASH32_H
#define _BOBHASH32_H
#include <cstdio>
#include <random>
#include <vector>
#include <unordered_set>
#include <iostream>
#include "Param.h"

using namespace std;






class BOBHash32
{
public:
	BOBHash32()
	{
		this->prime32Num = 0;
	}

	BOBHash32(uint32_t prime32Num)
	{
		this->prime32Num = prime32Num;
	}

	void initialize(uint32_t prime32Num)
	{
		this->prime32Num = prime32Num;
	}

	uint32_t run(const char * str, uint32_t len)
	{
		//register ub4 a,b,c,len;
		uint32_t a,b,c;
	//	uint32_t initval = 0;
		/* Set up the internal state */
		//len = length;
		a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
		c = prime32[this->prime32Num];         /* the previous hash value */
		/*---------------------------------------- handle most of the key */
		while (len >= 12)
		{
			a += (str[0] +((uint32_t)str[1]<<8) +((uint32_t)str[2]<<16) +((uint32_t)str[3]<<24));
			b += (str[4] +((uint32_t)str[5]<<8) +((uint32_t)str[6]<<16) +((uint32_t)str[7]<<24));
			c += (str[8] +((uint32_t)str[9]<<8) +((uint32_t)str[10]<<16)+((uint32_t)str[11]<<24));
			mix(a,b,c);
			str += 12; len -= 12;
		}
		/*------------------------------------- handle the last 11 bytes */
		c += len;
		switch(len)              /* all the case statements fall through */
		{
			case 11: c+=((uint32_t)str[10]<<24);
			// fall through
			case 10: c+=((uint32_t)str[9]<<16);
			// fall through
			case 9 : c+=((uint32_t)str[8]<<8);
			/* the first byte of c is reserved for the length */
			// fall through
			case 8 : b+=((uint32_t)str[7]<<24);
			// fall through
			case 7 : b+=((uint32_t)str[6]<<16);
			// fall through
			case 6 : b+=((uint32_t)str[5]<<8);
			// fall through
			case 5 : b+=str[4];
			// fall through
			case 4 : a+=((uint32_t)str[3]<<24);
			// fall through
			case 3 : a+=((uint32_t)str[2]<<16);
			// fall through
			case 2 : a+=((uint32_t)str[1]<<8);
			// fall through
			case 1 : a+=str[0];
			/* case 0: nothing left to add */
		}
		mix(a,b,c);
		/*-------------------------------------------- report the result */
		return c;
	}

	~BOBHash32(){}

	static uint32_t get_random_prime_index()
	{
		random_device rd;
		return rd() % MAX_PRIME32;
	}

    static vector<uint32_t> get_random_prime_index_list(int n)
    {
        random_device rd;
        unordered_set<int> st;
        while ((int)st.size() < n) {
            st.insert(rd() % MAX_PRIME32);
        }
        return vector<uint32_t>(st.begin(), st.end());
    }
private:
	uint32_t prime32Num;
};


#endif //_BOBHASH32_H
