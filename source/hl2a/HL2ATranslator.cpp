#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include "HL2ATranslator.h"
#include "HL2AResult.h"


using namespace std;
namespace HL2A
{
	
	char * HL2ATranslator::ToXml(list<HL2A_DATA>* listData, int * dataLength)
	{
		//Create doc 
		xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
		
		//Create root node
		xmlNodePtr root = xmlNewNode(NULL, BAD_CAST"flux_loop");

		//Attach root node to doc
		xmlDocSetRootElement(doc, root);

		//Decimal Data
		char deciData[10];
		for (list<HL2A_DATA>::const_iterator iter = listData->begin(); iter != listData->end(); iter++)
		{
			if (iter->chnl_id != 178)
				continue;
			//Add Channel Node 
			xmlNodePtr channel = xmlNewNode(NULL, BAD_CAST "channel");
			xmlAddChild(root, channel);

			//convert decimal to string
			sprintf(deciData, "%d", iter->chnl_id);
			//Set Channel Node property
			xmlNewProp(channel, BAD_CAST"id", BAD_CAST deciData);
			xmlNewProp(channel, BAD_CAST"name", BAD_CAST iter->chnl);
			
			//Add X Axis Nodes
			xmlNodePtr xDatas = xmlNewNode(NULL, BAD_CAST"xDatas");
			xmlAddChild(channel, xDatas);

			//Add Y Axis Nodes
			xmlNodePtr yDatas = xmlNewNode(NULL, BAD_CAST"yDatas");
			xmlAddChild(channel, yDatas);

			for (int i = 0; i < iter->dtCnt; i++)
			{
				//Process X Axis Data
				//reset data to 0
				bzero(deciData, sizeof(deciData));
				xmlNodePtr data = xmlNewNode(NULL, BAD_CAST"data");

				//convert decimal to string,  because the data is float,
				
				sprintf(deciData, "%f", _converter.Byte2Float((unsigned char *)(iter->xData + i * 4)));

				xmlNewTextChild(xDatas, NULL, BAD_CAST"data", BAD_CAST deciData);

				//Process Y Axis Data
				//reset data to 0
				bzero(deciData, sizeof(deciData));
				data = xmlNewNode(NULL, BAD_CAST"data");

				//convert decimal to string
				switch (iter->dtType)
				{
				case Int16:
					sprintf(deciData, "%d", _converter.Byte2Short((unsigned char *)(iter->yData + i * iter->yDataLength)));
					break;
				case Int32:
					sprintf(deciData, "%d", _converter.Byte2Int((unsigned char *)(iter->yData + i * iter->yDataLength)));
					break;
				case Float:
				case Other:
					sprintf(deciData, "%f", _converter.Byte2Float((unsigned char *)(iter->yData + i * iter->yDataLength)));
					break;
				}
				if (i == 20000)
				{
					int m = i;
				}

				xmlNewTextChild(yDatas, NULL, BAD_CAST"data", BAD_CAST deciData);

			}

		}

		xmlChar* pXmlStr = NULL;
		
		xmlDocDumpFormatMemoryEnc(doc, &pXmlStr, dataLength, "UTF-8", 0);

		
		return (char*)pXmlStr;
	}
	//Because struct include char array. As we all know the memory assigned for char array should greater 1 byte. So we can't use memcopy to assign data once.
	void HL2ATranslator::ToInf(DATA_INF * inf, char * buffer)
	{
		memset(inf, 0, sizeof(DATA_INF));
		memcpy(&inf->filetype, buffer, 10);
		memcpy(&inf->chnl_id, buffer + 10, 2);
		memcpy(&inf->chnl, buffer + 12, 12);
		memcpy(&inf->addr, buffer + 24, 4);
		memcpy(&inf->freq, buffer + 28, 4);
		memcpy(&inf->len, buffer + 32, 4);
		memcpy(&inf->post, buffer + 36, 4);
		memcpy(&inf->maxDat, buffer + 40, 2);
		memcpy(&inf->lowRang, buffer + 42, 4);
		memcpy(&inf->highRang, buffer + 46, 4);
		memcpy(&inf->factor, buffer + 50, 4);
		memcpy(&inf->offset, buffer + 54, 4);
		memcpy(&inf->unit, buffer + 58, 8);
		memcpy(&inf->dly, buffer + 66, 4);
		memcpy(&inf->attribDt, buffer + 70, 2);
		memcpy(&inf->datWth, buffer + 72, 2);
		memcpy(&inf->sparI1, buffer + 74, 2);
		memcpy(&inf->sparI2, buffer + 76, 2);
		memcpy(&inf->sparI3, buffer + 78, 2);
		memcpy(&inf->sparF1, buffer + 80, 4);
		memcpy(&inf->sparF2, buffer + 84, 4);
		memcpy(&inf->sparC1, buffer + 88, 8);
		memcpy(&inf->sparC2, buffer + 96, 16);
		memcpy(&inf->sparC3, buffer + 112, 10);
		
	}
	bool HL2ATranslator::Translate(char * keyParam, list<HL2A_DATA> * listData, IDataManager dataManager)
	{
		FILE * fInf = NULL;
		FILE * fDat = NULL;

		bool isSuccess = false;
		int INFLENGTH = 122;

		BYTE readBuffer[INFLENGTH];
		int readCount;

		//Storage Data

		struct DATA_INF inf;
		try
		{
			//Open File
			printf("\r=============Ready to open file.=====================\n");
			OpenHL2AFile(keyParam, &fInf, &fDat);

			if (NULL == &fInf || NULL == &fDat)
			{
				printf("\r=============file not opened.=====================\n");
				throw "Inf file or Dat file not opened.";
			}


			while (true)
			{
				memset(&readBuffer, 0, INFLENGTH);

				//read 122 bytes in memory for every time.
				readCount = fread(&readBuffer, 1, sizeof(readBuffer), fInf);

				//loop stop condition.
				if (readCount < INFLENGTH)
				{
					if (readCount > 0)
						throw "Inf file may be cracked.";
					break;
				}
				//change memory to data structure.
				//memcpy(&inf, &readBuffer, INFLENGTH);
				ToInf(&inf, (char*)readBuffer);

				
				//invoke function to read data file.
				HL2A_DATA * pData = NULL;

				pData = ProcessChannelData(&inf, fDat);
				if (NULL == pData)
				{
					//throw "Haven't got any data from data file.";
				}
				listData->push_back(*pData);
			}


			//关闭DAT文件
			fclose(fDat);
			//关闭INF文件
			fclose(fInf);

			isSuccess = true;
		}
		catch (exception ex)
		{
			//关闭DAT文件
			fclose(fDat);
			//关闭INF文件
			fclose(fInf);

		}
		return isSuccess;
	}
	HL2AResult HL2ATranslator::DoTranslateWithOutputParam(char * keyParam, unsigned char * inputListData, IDataManager dataManager)
	{
		HL2AResult result;

		list< HL2A_DATA> * listData = (list< HL2A_DATA> *)inputListData;
		if (Translate(keyParam, listData, dataManager))
		{

			//Set Return Data Collection.
			
			result.SetStatus(0);
			result.SetDescription("");
			result.SetData(NULL);

		}
		else
		{
			result.SetStatus(-1);
			result.SetDescription("Fail to translate data format.");
		}

		return result;
	}
	HL2AResult HL2ATranslator::DoTranslate(char * keyParam, char * reserved, IDataManager dataManager)
	{
		HL2AResult result;

		list< HL2A_DATA> listData;
		if (Translate(keyParam, &listData, dataManager))
		{
			
			//设置返回结果集
			int length;
			char * xml = ToXml(&listData,&length);

			result.SetStatus(0);
			result.SetDescription("");
			result.SetData(xml);
			result.SetDataLength(length);

			//清理内存信息	
			listData.clear();
		}
		else
		{
			result.SetStatus(-1);
			result.SetDescription("Fail to translate data format.");
		}

		return result;
		
	}
	
	void HL2ATranslator::OpenHL2AFile(char * keyParam, FILE ** fInf, FILE ** fDat)
	{
		//读取环境变量
		char * fileDir = getenv(DATADIR);
		if (NULL == fileDir || strcmp("", fileDir) > 0)
		{
			fileDir = (char*)malloc(sizeof("/home/XL016373/hl2a_test_files/") + 1);
			strcpy(fileDir, "/home/XL016373/hl2a_test_files/");
		}
		printf("\r=The file Directory is : %s=\n", fileDir);
		//转换参数成文件名
		char fileNameInf[400];
		char fileNameDat[400];
		memset(fileNameInf, 0, sizeof(fileNameInf));
		memset(fileNameDat, 0, sizeof(fileNameDat));

		strcpy(fileNameInf, fileDir);
		strcpy(fileNameInf + strlen(fileDir), keyParam);
		strcpy(fileNameInf + strlen(fileDir) + strlen(keyParam), ".inf");

		printf("\r=The inf file name is : %s=\n", fileNameInf);

		strcpy(fileNameDat, fileDir);
		strcpy(fileNameDat + strlen(fileDir), keyParam);
		strcpy(fileNameDat + strlen(fileDir) + strlen(keyParam), ".dat");

		printf("\r=The dat file name is : %s=\n", fileNameDat);

		if (access(fileNameInf, R_OK) != 0)
		{
			printf("\r=============The inf file is not exists or have no permission to read=====================\n");
			throw "The inf file is not exists or have no permission to read.";
		}
		if (access(fileNameDat, R_OK) != 0)
		{
			printf("\r=============The dat file is not exists or have no permission to read=====================\n");
			throw "The dat file is not exists or have no permission to read.";
		}
		//Open INF file
		*fInf = fopen((char *)fileNameInf, "rb");
		
		fseek(*fInf, 0, SEEK_SET);
		//Open Data file
		*fDat = fopen((char *)fileNameDat, "rb");
		
		
		fseek(*fDat, 0, SEEK_SET);
	}
	HL2AResult HL2ATranslator::DoRead(char * keyParam, char * reserved, IDataManager dataManager)
	{
		HL2AResult result;
		return result;
	}

	HL2A_DATA * HL2ATranslator::ProcessChannelData(DATA_INF * inf, FILE * fDat)
	{
		//Check the validation of the info file. temporarily ignore the verification.
		HL2A_DATA * data = (HL2A_DATA*)malloc(sizeof(HL2A_DATA));
		strcpy(data->chnl, inf->chnl);
		data->chnl_id = inf->chnl_id;
		data->dtCnt = inf->len;
		//data type
		int dataLength;
		switch (inf->attribDt)
		{
		case 3:
		case 4:
			data->dtType = Float;
			dataLength = 4;
			break;
		case 2:
		case 5:
			dataLength = 2;
			data->dtType = Int16;
			break;
		default:
			dataLength = 2;
			data->dtType = Other;
			break;
		}
		//Announce read buffer.
		unsigned char pBuffer[1024];
		//Create heap data to assign data buffer.
		unsigned char * pData = (unsigned char*)malloc(inf->len * dataLength);
		unsigned char * tData = (data->dtType == Other) ? (unsigned char*)malloc(inf->len * 4) : pData;
		unsigned char * xData = (unsigned char*)malloc(inf->len * sizeof(float));

		data->yDataLength = data->dtType == Other ? 4 : dataLength;
		
		memset(pData, 0, sizeof(pData));
		
		//set file pointer to the special address.
		fseek(fDat, inf->addr, SEEK_SET);

		//read data from data file. Assume the data format is accuracy. we will not verify the data address and data length.
		int readTotalCnt=0, readCnt=0, maxLength = dataLength * inf->len;
		while (true)
		{
			memset(pBuffer, 0, sizeof(pBuffer));
			readCnt = fread(&pBuffer, 1, sizeof(pBuffer) > (maxLength - readTotalCnt) ? maxLength - readTotalCnt : sizeof(pBuffer), fDat);
			
			//if file EOF
			if (readCnt < 1)
				break;

			//copy buffer to heap data
			memcpy(pData + readTotalCnt, pBuffer, readCnt);



			readTotalCnt += readCnt;
			//if already read all data marked in inf structure.
			if (readTotalCnt >= inf->len * dataLength)
				break;

		}
		
		
		if (readTotalCnt < inf->len)
			throw "data file length doesn't match the structure description of inf file.";

		//process Y data

		
		for (int i = 0; i < inf->len; i++)
		{
			switch (data->dtType)
			{
			case Other:
				
				//转换byte to short
				short shortData = _converter.Byte2Short(pData + i * dataLength);
				_converter.Float2ByteArray(tData + i * 4, (float)((((inf->highRang - inf->lowRang) / inf->maxDat) * shortData + inf->lowRang) * inf->factor + inf->offset));
				
				break;
			}
			
			//process X data

			_converter.Float2ByteArray(xData + i * 4, (float)((i - (inf->len - inf->post)) / inf->freq * 1000 + inf->dly));

		}
		data->xData = xData;
		if (data->dtType != Other)
		{
			data->yData = pData;
		}
		else
		{
			//Release allocated memeory in heap.
			free(pData);
			data->yData = tData;
		}

		return data;
	}

}





