#ifndef UDA_HL2A_SAVERFACTORY_H
#define UDA_HL2A_SAVERFACTORY_H
#include "IDataManager.h"
namespace HL2A
{

	class DataManagerFactory
	{
	public:
		IDataManager GetDataManager();

	};

}


#endif




