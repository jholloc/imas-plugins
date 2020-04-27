#ifndef UDA_HL2A_ITRANSLATOR_H
#define UDA_HL2A_ITRANSLATOR_H

#include "IDataManager.h"
#include "HL2AResult.h"

namespace HL2A
{
	
	interface ITranslator
	{
		virtual HL2AResult DoTranslateWithOutputParam(char * keyParam, unsigned char * reserved, IDataManager dataManager) { HL2AResult result; return result; }
		virtual HL2AResult DoTranslate(char * keyParam, char * reserved, IDataManager dataManager) { HL2AResult result; return result; }
		virtual HL2AResult DoRead(char * keyParam, char * reserved, IDataManager dataManager) { HL2AResult result; return result; }

		//virtual ~ITranslator() {}

	};

	
}


#endif


