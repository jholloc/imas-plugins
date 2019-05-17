#include "DataManagerFactory.h"
#include "MDSSaver.h"
namespace HL2A
{
	IDataManager DataManagerFactory::GetDataManager()
	{
		return MDSSaver();
	}

}
