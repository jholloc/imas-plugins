#ifndef UDA_HL2A_IDATAMANAGER_H
#define UDA_HL2A_IDATAMANAGER_H
#include "BaseTypes.h"


namespace HL2A
{
	interface IDataManager
	{
		bool SaveData(char * keyParam, char * reserved) {}

		char * Read(char * keyParam, char * reserved) {}
	};

}















#endif