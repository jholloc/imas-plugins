#ifndef UDA_HL2A_HL2ATRANSLATOR_H
#define UDA_HL2A_HL2ATRANSLATOR_H
#include "BaseTypes.h"
#include "ITranslator.h"
#include "IDataManager.h"
#include <stdio.h>
#include <list>
#include "HL2AConverter.h"

using namespace std;
#define TESTDATA "/home/fanze/share/";
namespace HL2A
{
	
	struct DATA_INF
	{
		char filetype[10];         //0   文件类型：固定为“swip_das"
		short int chnl_id;         //10  通道 ID
		char chnl[12];             //12  通道信号名称
		int addr;                  //24  数据指针
		float freq;                //28  采样率
		int len;                   //32  数据采集长度
		int post;                  //36  触发后采集长度
		unsigned short int maxDat; //40  满量程时的 A/D 转换值
		float lowRang;             //42  量程下限
		float highRang;            //46  量程上限
		float factor;              //50  系数因子
		float offset;              //54  信号偏移量
		char unit[8];              //58  物理量单位
		float dly;                 //66  采数延时(ms)
		short int attribDt;        //70  数据属性：A/D数据1、 整形数据2、实型数据3
		short int datWth;          //72  数据字宽度
		short int sparI1;          //74  备用2字节整数 1
		short int sparI2;          //76  备用2字节整数 2
		short int sparI3;          //78  备用2字节整数 3
		float sparF1;              //80  备用4字节浮点 1
		float sparF2;              //84  备用4字节浮点 2
		char sparC1[8];            //88  备用字符串 1
		char sparC2[16];           //96  备用字符串 2
		char sparC3[10];           //112 备用字符串 3
	};
	enum DataType
	{
		Int16 = 1,
		Float = 2,
		Int32 = 3,
		Other = 4
	};

	struct HL2A_DATA
	{
		short int chnl_id;         //10  通道 ID
		char chnl[12];             //12  通道信号名称
		DataType dtType;
		int dtCnt;
		int yDataLength;
		void * xData;
		void * yData;
	};
	
	class HL2ATranslator : public ITranslator
	{
	public:
		virtual HL2AResult DoTranslate(char * keyParam, char * reserved, IDataManager dataManager);
		
		virtual HL2AResult DoTranslateWithOutputParam(char * keyParam, unsigned char * reserved, IDataManager dataManager);

		virtual HL2AResult DoRead(char * keyParam, char * reserved, IDataManager dataManager);

		explicit HL2ATranslator()
		{

		}
		
	private:
		
		const char * _pDefualtDir = "/home/fanze/share/";
		const char * DATADIR = "HL2A_DATADIR";

		char * ToXml(list<HL2A_DATA> * listData, int * dataLength);
		char _directory[300];
		HL2A_DATA * ProcessChannelData(DATA_INF* inf, FILE * fDat);
		void OpenHL2AFile(char * keyParam, FILE ** fInf, FILE ** fDat);

		bool Translate(char * keyParam, list<HL2A_DATA> * listData, IDataManager dataManager);

		void ToInf(DATA_INF * inf, char * buffer);
		Converter _converter;


		
	};
}


#endif




