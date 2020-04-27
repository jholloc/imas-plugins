#include "HL2AConverter.h"

namespace HL2A
{
	float Converter::Byte2Float(unsigned char * bArray)
	{
		float fv;

		void  * pf = &fv;
		unsigned char * px = bArray;

		for (unsigned char i = 0; i < 4; i++)
		{
			*((unsigned char*)pf + i) = *(px + i);
		}

		return fv;
	}
	int Converter::Byte2Int(unsigned char * bArray)
	{
		int iv;

		void  * pf = &iv;
		unsigned char * px = bArray;

		for (unsigned char i = 0; i < 4; i++)
		{
			*((unsigned char*)pf + i) = *(px + i);
		}

		return iv;
	}
	short  Converter::Byte2Short(unsigned char * bArray)
	{
		short sv;

		void  * pf = &sv;
		unsigned char * px = bArray;

		for (unsigned char i = 0; i < 2; i++)
		{
			*((unsigned char*)pf + i) = *(px + i);
		}

		return sv;
	}

	void Converter::Float2ByteArray(unsigned char * pArray, float fv)
	{
		unsigned char * pdata = (unsigned char*)&fv;  //把float类型的指针强制转换为unsigned char型  
		for (unsigned char i = 0; i < 4; i++)
		{
			pArray[i] = *pdata++;//把相应地址中的数据保存到unsigned char数组中       
		}
	}
	void Converter::Int2ByteArray(unsigned char * pArray, int iv)
	{
		unsigned char * pdata = (unsigned char*)&iv;  //把float类型的指针强制转换为unsigned char型  
		for (unsigned char i = 0; i < 4; i++)
		{
			pArray[i] = *pdata++;//把相应地址中的数据保存到unsigned char数组中       
		}
	}
	void Converter::Short2ByteArray(unsigned char * pArray, short sv)
	{
		unsigned char * pdata = (unsigned char*)&sv;  //把float类型的指针强制转换为unsigned char型  
		for (unsigned char i = 0; i < 4; i++)
		{
			pArray[i] = *pdata++;//把相应地址中的数据保存到unsigned char数组中       
		}
	}
}
