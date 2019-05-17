#ifndef UDA_HL2A_HL2ARESULT_H
#define UDA_HL2A_HL2ARESULT_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

namespace HL2A
{
	//interface IResult
	//{
	//	virtual int GetStatus() {}
	//	virtual char * GetDescription() {}
	//	virtual void * GetTranslatedData() {}
	//	virtual long GetTranslatedDataLength() {}
	//	//virtual ~IResult() {};
	//};

	class HL2AResult //: public IResult
	{
	public:
		virtual int GetStatus();
		virtual char * GetDescription();
		virtual char * GetTranslatedData();
		virtual long GetTranslatedDataLength();
		void SetStatus(int status);
		void SetDescription(const char * desc);
		void SetData(char * data);
		void SetDataLength(long length);

		~HL2AResult()
		{
			free(_description);
		}
	private:
		int _status;
		char * _description;
		char * _data;
		long _dataLength;
	};
}


#endif