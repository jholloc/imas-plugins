#include "BaseTypes.h"
#include "HL2AResult.h"

namespace HL2A
{
	int HL2AResult::GetStatus()
	{
		return 1;
	}

	char * HL2AResult::GetDescription()
	{
		return 0;
	}

	char * HL2AResult::GetTranslatedData()
	{
		return _data;
	}
	long HL2AResult::GetTranslatedDataLength()
	{
		return _dataLength;
	}
	void HL2AResult::SetStatus(int status)
	{
		_status = status;
	}
	
	void HL2AResult::SetDescription(const char * desc)
	{
		char * p = (char*)malloc(sizeof(desc));
		_description = p;
	}
	void HL2AResult::SetData(char * data)
	{
		_data = data;
	}
	void HL2AResult::SetDataLength(long length)
	{
		_dataLength = length;
	}
}