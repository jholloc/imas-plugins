#ifndef UDA_HL2A_MDSSAVER_H
#define UDA_HL2A_MDSSAVER_H

#include "IDataManager.h"

namespace HL2A
{
	class MDSSaver : public IDataManager
	{
	public:
		virtual bool SaveData(char * keyParam, char * reserved);

		virtual char * Read(char * keyParam, char * reserved);

		/*explicit MDSSaver()
		{
			char * host = getenv(HOSTENV);
			if (NULL == host && strcmp("", host) > 0)
			{
				host = "127.0.0.1:8000";
			}
		}*/
	private:
		/*char _host[50];
		char * HOSTENV = "MDSHOST";
		char * _host[100];*/

	};
}


#endif




