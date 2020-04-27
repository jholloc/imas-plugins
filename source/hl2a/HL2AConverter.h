#ifndef UDA_HL2A_CONVERTOR_H
#define UDA_HL2A_CONVERTOR_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

namespace HL2A
{
	class Converter
	{
	public:
		float Byte2Float(unsigned char * bArray);
		int Byte2Int(unsigned char * bArray);
		short Byte2Short(unsigned char * bArray);
		
		void Float2ByteArray(unsigned char * pArray, float f);
		void Int2ByteArray(unsigned char * pArray, int f);
		void Short2ByteArray(unsigned char * pArray, short f);
	};
}


#endif