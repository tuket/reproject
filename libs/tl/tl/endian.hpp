#pragma once

//#include <SDL_config.h>

typedef unsigned char uchar;

/*
 --- BIG ENDIAN ----------------

					INTEGER
	   MEM   0x  01  23 45 56
	 | ... |     |   |  |  |
	 | ... |     |   |  |  |
	 | a+0 | <----   |  |  |
	 | a+1 | <--------  |  |
	 | a+2 | <-----------  |
	 | a+3 | <-------------|
	 | ... |
	 | ... |

 --- LITTLE ENDIAN -------------

 INTEGER
 0x  01 23 45 56      MEM
      |  |  |  |    | ... |
      |  |  |  |    | ... |
      |  |  |  ---> | a+0 |
      |  |  ------> | a+1 |
      |  ---------> | a+2 |
      | ----------> | a+3 |
                    | ... |
                    | ... |

 */

namespace tl
{
	template <typename T>
    static void writeBigEndian(char* out, T x)
	{
        uchar* const uout = reinterpret_cast<uchar*>(out);
		int n = sizeof(T) - 1;
		while(n >= 0)
		{
            uout[n] = (uchar)x;
            x >>= 8;
			n--;
		}
	}
	
	template <typename T>
    static void writeLittleEndian(char* out, T x)
	{
        uchar* const uout = reinterpret_cast<uchar*>(out);
		const int n = sizeof(T);
		for(int i=0; i<n; i++)
		{
            uout[i] = (uchar)x;
            x >>= 8;
		}
    }
	
	template <typename T>
    static T readBigEndian(const char* in)
	{
        const uchar* const uin = reinterpret_cast<const uchar*>(in);
		const int n = sizeof(T);
		T x = 0;
		for(int i=0; i<n; i++)
		{
            x <<= 8;
            x |= uin[i];
		}
		return x;
	}
	
    template <typename T>
    static T readLittleEndian(const char* in)
    {
        const uchar* const uin = reinterpret_cast<const uchar*>(in);
        const int n = sizeof(T);
        T x = 0;
        for(int i = n-1; i >= 0; i--)
        {
            x <<= 8;
            x |= in[i];
        }
        return x;
    }
}
