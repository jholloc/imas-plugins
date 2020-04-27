#ifndef UDA_HL2A_TRANSLATORFACTORY_H
#define UDA_HL2A_TRANSLATORFACTORY_H

#include "ITranslator.h"
//#include "HL2ATranslator.h"

namespace HL2A
{
	class TranslatorFactory
	{
	public:
		ITranslator * GetTranslator();
		
	};
}


#endif




