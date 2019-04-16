#ifndef UDA_PLUGIN_TCV_XML_H
#define UDA_PLUGIN_TCV_XML_H

#include <libxml/xpath.h>

#include <clientserver/udaStructs.h>

#ifdef LIBXML2_PRINTF_CHAR_ARG
#  define XML_FMT_TYPE const char*
#else
#  define XML_FMT_TYPE const xmlChar*
#endif

int execute_xpath_expression(const char* filename, const xmlChar* xpathExpr, DATA_BLOCK* data_block, int* nodeIndices);

#endif // UDA_PLUGIN_TCV_XML_H
