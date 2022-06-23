#include "qmcdecoder.h"
#include <fstream>

uint8_t Seed::NextMask()
    {
        uint8_t ret;
        index++;
        if(x<0)
        {
            dx = 1;
            y = (8-y)%8;
            //ret = (8 -y)%8;
            ret = 0xc3;
        }
        else if(x > 6)
        {
            dx = -1;
            y= 7-y;
            ret = 0xd8;
        }
        else
        {
            ret = seedMap[y][x];
        }

        x += dx;
        if(index==0x8000||(index>0x8000&&(index+1)%0x8000==0))
            return NextMask();
        return ret;
    }

char* Seed::process(char * buffer,long len)
{
    Seed seed;
    for(int i=0; i<len; ++i)
    {
        buffer[i] ^= seed.NextMask();
    }
    return buffer;
}

