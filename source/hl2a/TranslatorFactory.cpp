#include "TranslatorFactory.h"
#include "HL2ATranslator.h"
#include <stdlib.h>
#include <string.h>

namespace HL2A
{

	ITranslator * TranslatorFactory::GetTranslator()
	{
		HL2ATranslator * heapData = (HL2ATranslator*)malloc(sizeof(HL2ATranslator));
		HL2ATranslator trans = HL2ATranslator();
		memcpy(heapData, &trans, sizeof(HL2ATranslator));
		return heapData;
	}
}

