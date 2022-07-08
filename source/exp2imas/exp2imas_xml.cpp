#include "exp2imas_xml.h"

#include <assert.h>
#include <regex.h>

#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>
#include <clientserver/errorLog.h>
#include <logging/logging.h>
#include <clientserver/stringUtils.h>
#include <plugins/udaPlugin.h>

namespace {

int convert_type_string_to_uda_type(const std::string& value)
{
    int i = UDA_TYPE_UNKNOWN;
    int err = 0;

    if (value == "matstring_type" || value == "vecstring_type" || value == "xs:string" || value == "STR_0D") {
        i = UDA_TYPE_STRING;
    } else if (value == "matflt_type" || value == "vecflt_type" || value == "array3dflt_type" || value == "xs:float"
            || value == "FLT_0D") {
        i = UDA_TYPE_FLOAT;
    } else if (value == "matint_type" || value == "vecint_type" || value == "array3dint_type" || value == "xs:integer"
            || value == "INT_0D") {
        i = UDA_TYPE_INT;
    } else {
        err = 999;
        addIdamError(UDA_CODE_ERROR_TYPE, __func__, err, "Unsupported data type");
    }
    return i;
}

double* get_content(xmlNode* node, size_t* n_vals)
{
    xmlChar* content = node->children->content;

    const char* chr = (const char*)content;
    *n_vals = 0;

    double* vals = nullptr;

    bool in_expand = false;
    bool in_expand_range = false;
    size_t num_expand_vals = 0;
    double* expand_vals = nullptr;
    int expand_start = 0;
    int expand_end = 0;

    const char* num_start = chr;

    bool cont = true;

    while (cont) {
        switch (*chr) {
            case ',':
            case '\0':
                if (in_expand) {
                    if (in_expand_range) {
                        expand_start = (int)strtol(num_start, nullptr, 10);
                        num_start = chr + 1;
                    } else {
                        expand_vals = (double*)realloc(expand_vals, (num_expand_vals + 1) * sizeof(double));
                        expand_vals[num_expand_vals] = strtod(num_start, nullptr);
                        ++num_expand_vals;
                        num_start = chr + 1;
                    }
                } else if (*num_start != '\0') {
                    vals = (double*)realloc(vals, (*n_vals + 1) * sizeof(double));
                    vals[*n_vals] = strtod(num_start, nullptr);
                    ++(*n_vals);
                    num_start = chr + 1;
                }
                if (*chr == '\0') {
                    cont = false;
                }
                break;
            case '=':
                in_expand_range = true;
                num_start = chr + 1;
                break;
            case '(':
                in_expand = true;
                num_start = chr + 1;
                break;
            case ')': {
                expand_end = (int)strtol(num_start, nullptr, 10);
                int i = 0;
                for (i = expand_start; i <= expand_end; ++i) {
                    vals = (double*)realloc(vals, (*n_vals + num_expand_vals) * sizeof(double));
                    int j;
                    for (j = 0; j < num_expand_vals; ++j) {
                        vals[*n_vals + j] = expand_vals[j];
                    }
                    *n_vals += num_expand_vals;
                }
                if (*(chr + 1) == ',') {
                    ++chr;
                }
                num_start = chr + 1;
                in_expand = false;
                in_expand_range = false;
                free(expand_vals);
                expand_vals = nullptr;
                num_expand_vals = 0;
                expand_start = 0;
                expand_end = 0;
                break;
            }
            default:
                break;
        }
        ++chr;
    }

    return vals;
}

const char* get_type(const xmlChar* xpathExpr, xmlXPathContextPtr xpathCtx)
{
    const char* type = "xs:integer";

    /* First, we get the type of the element which is requested */

    //Creating the Xpath request for the type which does not exist necessarly (it depends on the XML element which is requested)
    const char* typeStr = "/@type";
    size_t len = 1 + xmlStrlen(xpathExpr) + strlen(typeStr);
    xmlChar* typeXpathExpr = (xmlChar*)malloc(len * sizeof(xmlChar));
    xmlStrPrintf(typeXpathExpr, (int)len, (XML_FMT_TYPE)"%s%s", xpathExpr, typeStr);

    /* Evaluate xpath expression for the type */
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(typeXpathExpr, xpathCtx);
    if (xpathObj != nullptr) {
        xmlNodeSetPtr nodes = xpathObj->nodesetval;

        xmlNodePtr cur;

        if (nodes != nullptr && nodes->nodeNr > 0) {
            cur = nodes->nodeTab[0];
            cur = cur->children;
            type = (char*)cur->content;
        }
    }

    free(typeXpathExpr);

    return type;
}

int* get_dims(const xmlChar* xpathExpr, xmlXPathContextPtr xpathCtx, int* rank)
{
    int* dims = nullptr;
    *rank = 0;

    /* First, we get the type of the element which is requested */

    //Creating the Xpath request for the type which does not exist necessarly (it depends on the XML element which is requested)
    const char* typeStr = "/@dim";
    size_t len = 1 + xmlStrlen(xpathExpr) + strlen(typeStr);
    xmlChar* typeXpathExpr = (xmlChar*)malloc(len * sizeof(xmlChar));
    xmlStrPrintf(typeXpathExpr, (int)len, (XML_FMT_TYPE)"%s%s", xpathExpr, typeStr);

    /* Evaluate xpath expression for the type */
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(typeXpathExpr, xpathCtx);
    if (xpathObj != nullptr) {
        xmlNodeSetPtr nodes = xpathObj->nodesetval;

        xmlNodePtr cur;

        if (nodes != nullptr && nodes->nodeNr > 0) {
            cur = nodes->nodeTab[0];
            cur = cur->children;

            char** tokens = SplitString((char*)cur->content, ",");
            int i = 0;
            while (tokens[i] != nullptr) {
                dims = (int*)realloc(dims, (i + 1) * sizeof(int));
                dims[i] = (int)strtol(tokens[i], nullptr, 10);
                ++i;
            }
            FreeSplitStringTokens(&tokens);

            *rank = i;
        }
    }

    free(typeXpathExpr);

    return dims;
}

int* get_dimensions(const xmlChar* xpathExpr, xmlXPathContextPtr xpathCtx)
{
    int* dimensions = nullptr;

    /* First, we get the type of the element which is requested */

    //Creating the Xpath request for the type which does not exist necessarly (it depends on the XML element which is requested)
    const char* typeStr = "/../dimension";
    size_t len = 1 + xmlStrlen(xpathExpr) + strlen(typeStr);
    xmlChar* typeXpathExpr = (xmlChar*)malloc(len * sizeof(xmlChar));
    xmlStrPrintf(typeXpathExpr, (int)len, (XML_FMT_TYPE)"%s%s", xpathExpr, typeStr);

    /* Evaluate xpath expression for the type */
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(typeXpathExpr, xpathCtx);
    if (xpathObj != nullptr) {
        xmlNodeSetPtr nodes = xpathObj->nodesetval;

        xmlNodePtr cur;

        if (nodes != nullptr && nodes->nodeNr > 0) {
            dimensions = (int*)malloc(nodes->nodeNr * sizeof(int));
            int i;
            for (i = 0; i < nodes->nodeNr; ++i) {
                cur = nodes->nodeTab[0];
                cur = cur->children;
                dimensions[i] = (int)strtol((char*)cur->content, nullptr, 10);
            }

        }
    }

    free(typeXpathExpr);

    return dimensions;
}

int get_time_dim(const xmlChar* xpathExpr, xmlXPathContextPtr xpathCtx)
{
    int time_dim = 0;

    /* First, we get the type of the element which is requested */

    //Creating the Xpath request for the type which does not exist necessarly (it depends on the XML element which is requested)
    const char* typeStr = "/../time_dim";
    size_t len = 1 + xmlStrlen(xpathExpr) + strlen(typeStr);
    xmlChar* typeXpathExpr = (xmlChar*)malloc(len * sizeof(xmlChar));
    xmlStrPrintf(typeXpathExpr, (int)len, (XML_FMT_TYPE)"%s%s", xpathExpr, typeStr);

    /* Evaluate xpath expression for the type */
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(typeXpathExpr, xpathCtx);
    if (xpathObj != nullptr) {
        xmlNodeSetPtr nodes = xpathObj->nodesetval;

        xmlNodePtr cur;

        if (nodes != nullptr && nodes->nodeNr > 0) {
            cur = nodes->nodeTab[0];
            cur = cur->children;
            time_dim = (int)strtol((char*)cur->content, nullptr, 10);
        }
    }

    free(typeXpathExpr);

    return time_dim;
}

char* get_download(const xmlChar* xpathExpr, xmlXPathContextPtr xpathCtx, double** values, size_t* n_values)
{
    *values = nullptr;
    *n_values = 0;

    char* download_type = 0;

    /* First, we get the type of the element which is requested */

    const char* typeStr = "/../download/download";
    size_t len = 1 + xmlStrlen(xpathExpr) + strlen(typeStr);
    xmlChar* typeXpathExpr = (xmlChar*)malloc(len * sizeof(xmlChar));
    xmlStrPrintf(typeXpathExpr, (int)len, (XML_FMT_TYPE)"%s%s", xpathExpr, typeStr);

    /* Evaluate xpath expression for the type */
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(typeXpathExpr, xpathCtx);
    if (xpathObj != nullptr) {
        xmlNodeSetPtr nodes = xpathObj->nodesetval;

        xmlNodePtr cur;

        if (nodes != nullptr && nodes->nodeNr > 0) {
            cur = nodes->nodeTab[0];
            cur = cur->children;
            download_type = (char*)cur->content;
        }
    }

    free(typeXpathExpr);

    if (StringEquals(download_type, "fixed")) {
        char** value_strings = nullptr;
        char** repeat_strings = nullptr;

        const char* valueStr = "/../download/fixed_value";
        len = 1 + xmlStrlen(xpathExpr) + strlen(valueStr);
        typeXpathExpr = (xmlChar*)malloc(len * sizeof(xmlChar));
        xmlStrPrintf(typeXpathExpr, (int)len, (XML_FMT_TYPE)"%s%s", xpathExpr, valueStr);

        xpathObj = xmlXPathEvalExpression(typeXpathExpr, xpathCtx);
        if (xpathObj != nullptr) {
            xmlNodeSetPtr nodes = xpathObj->nodesetval;

            xmlNodePtr cur;

            if (nodes != nullptr && nodes->nodeNr > 0) {
                cur = nodes->nodeTab[0];
                cur = cur->children;
                value_strings = SplitString((const char*)cur->content, " ");
            }
        }

        xmlXPathFreeObject(xpathObj);
        free(typeXpathExpr);

        const char* repeatStr = "/../download/fixed_repeat";
        len = 1 + xmlStrlen(xpathExpr) + strlen(repeatStr);
        typeXpathExpr = (xmlChar*)malloc(len * sizeof(xmlChar));
        xmlStrPrintf(typeXpathExpr, (int)len, (XML_FMT_TYPE)"%s%s", xpathExpr, repeatStr);

        xpathObj = xmlXPathEvalExpression(typeXpathExpr, xpathCtx);
        if (xpathObj != nullptr) {
            xmlNodeSetPtr nodes = xpathObj->nodesetval;

            xmlNodePtr cur;

            if (nodes != nullptr && nodes->nodeNr > 0) {
                cur = nodes->nodeTab[0];
                cur = cur->children;
                repeat_strings = SplitString((const char*)cur->content, " ");
            }
        }

        xmlXPathFreeObject(xpathObj);
        free(typeXpathExpr);

        int idx = 0;
        while (value_strings[idx] != nullptr) {
            if (repeat_strings[idx] == nullptr) {
                addIdamError(UDA_CODE_ERROR_TYPE, __func__, 999, "mis-matching number of values in fixed download");
                return nullptr;
            }

            double value = strtod(value_strings[idx], nullptr);
            long repeat = strtol(repeat_strings[idx], nullptr, 10);

            *values = (double*)realloc(*values, (*n_values + repeat) * sizeof(double));

            int i;
            for (i = 0; i < repeat; ++i) {
                (*values)[*n_values + i] = value;
            }

            *n_values += repeat;

            ++idx;
        }
    }

    return download_type;
}

int* get_sizes(const xmlChar* xpathExpr, xmlXPathContextPtr xpathCtx)
{
    int* sizes = nullptr;

    /* First, we get the type of the element which is requested */

    //Creating the Xpath request for the type which does not exist necessarly (it depends on the XML element which is requested)
    const char* typeStr = "/../size";
    size_t len = 1 + xmlStrlen(xpathExpr) + strlen(typeStr);
    xmlChar* typeXpathExpr = (xmlChar*)malloc(len * sizeof(xmlChar));
    xmlStrPrintf(typeXpathExpr, (int)len, (XML_FMT_TYPE)"%s%s", xpathExpr, typeStr);

    /* Evaluate xpath expression for the type */
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(typeXpathExpr, xpathCtx);
    if (xpathObj != nullptr) {
        xmlNodeSetPtr nodes = xpathObj->nodesetval;

        xmlNodePtr cur;

        if (nodes != nullptr && nodes->nodeNr > 0) {
            sizes = (int*)malloc(nodes->nodeNr * sizeof(int));

            int i;
            for (i = 0; i < nodes->nodeNr; ++i) {
                cur = nodes->nodeTab[i];
                cur = cur->children;
                sizes[i] = (int)strtol((char*)cur->content, nullptr, 10);
            }
        }
    }

    free(typeXpathExpr);

    return sizes;
}

void get_coefs(float** coefas, float** coefbs, const xmlChar* xpathExpr, xmlXPathContextPtr xpathCtx)
{
    *coefas = nullptr;
    *coefbs = nullptr;

    const char* typeStr = "/../coefa";
    size_t len = 1 + xmlStrlen(xpathExpr) + strlen(typeStr);
    xmlChar* typeXpathExpr = (xmlChar*)malloc(len * sizeof(xmlChar));
    xmlStrPrintf(typeXpathExpr, (int)len, (XML_FMT_TYPE)"%s%s", xpathExpr, typeStr);

    /* Evaluate xpath expression for the type */
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(typeXpathExpr, xpathCtx);
    if (xpathObj != nullptr) {
        xmlNodeSetPtr nodes = xpathObj->nodesetval;

        xmlNodePtr cur;

        if (nodes != nullptr && nodes->nodeNr > 0
            && !STR_EQUALS((char*)nodes->nodeTab[0]->children->content, "Put value here")) {

            *coefas = (float*)malloc(nodes->nodeNr * sizeof(int));

            int i;
            for (i = 0; i < nodes->nodeNr; ++i) {
                cur = nodes->nodeTab[i];
                cur = cur->children;
                char* endptr = nullptr;
                (*coefas)[i] = strtof((char*)cur->content, &endptr);
                if (endptr == (char*)cur->content) {
                    (*coefas)[i] = 1.0f;
                }
            }
        }
    }

    free(typeXpathExpr);

    typeStr = "/../coefb";
    len = 1 + xmlStrlen(xpathExpr) + strlen(typeStr);
    typeXpathExpr = (xmlChar*)malloc(len * sizeof(xmlChar));
    xmlStrPrintf(typeXpathExpr, (int)len, (XML_FMT_TYPE)"%s%s", xpathExpr, typeStr);

    /* Evaluate xpath expression for the type */
    xpathObj = xmlXPathEvalExpression(typeXpathExpr, xpathCtx);
    if (xpathObj != nullptr) {
        xmlNodeSetPtr nodes = xpathObj->nodesetval;

        xmlNodePtr cur;

        if (nodes != nullptr && nodes->nodeNr > 0
            && !STR_EQUALS((char*)nodes->nodeTab[0]->children->content, "Put value here")) {

            *coefbs = (float*)malloc(nodes->nodeNr * sizeof(int));

            int i;
            for (i = 0; i < nodes->nodeNr; ++i) {
                cur = nodes->nodeTab[i];
                cur = cur->children;
                char* endptr = nullptr;
                (*coefbs)[i] = strtof((char*)cur->content, &endptr);
                if (endptr == (char*)cur->content) {
                    (*coefbs)[i] = 0.0f;
                }
            }
        }
    }

    free(typeXpathExpr);
}

} // anon namespace

int uda::exp2imas::execute_xpath_expression(const std::string& filename, const xmlChar* xpathExpr, int index, XML_DATA* xml_data)
{
    assert(xpathExpr);

    static std::string cached_filename = {};
    static xmlDocPtr doc = nullptr;
    static xmlXPathContextPtr xpathCtx = nullptr;

    if (cached_filename.empty()) {
        // store the file name of the currently loaded XML file
        cached_filename = filename;
    }

    if (filename != cached_filename) {
        // clear the file so we reload
        xmlXPathFreeContext(xpathCtx);
        xmlFreeDoc(doc);

        doc = nullptr;
        xpathCtx = nullptr;

        cached_filename = filename;
    }

    if (doc == nullptr) {
        /* Load XML document */
        doc = xmlParseFile(filename.c_str());
        if (doc == nullptr) {
            UDA_LOG(UDA_LOG_ERROR, "Error: unable to parse file \"%s\"\n", filename.c_str());
            return -1;
        }
    }

    if (xpathCtx == nullptr) {
        /* Create xpath evaluation context */
        xpathCtx = xmlXPathNewContext(doc);
        if (xpathCtx == nullptr) {
            UDA_LOG(UDA_LOG_ERROR, "Error: unable to create new XPath context\n");
            return -1;
        }
    }

    memset(xml_data, '\0', sizeof(XML_DATA));

    xml_data->dims = get_dims(xpathExpr, xpathCtx, &xml_data->rank);
    const char* type = get_type(xpathExpr, xpathCtx);
    xml_data->time_dim = get_time_dim(xpathExpr, xpathCtx);
    xml_data->dimensions = get_dimensions(xpathExpr, xpathCtx);
    xml_data->sizes = get_sizes(xpathExpr, xpathCtx);
    get_coefs(&xml_data->coefas, &xml_data->coefbs, xpathExpr, xpathCtx);
    xml_data->download = get_download(xpathExpr, xpathCtx, &xml_data->values, &xml_data->n_values);

    /* Evaluate xpath expression for requesting the data  */
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);

    if (xpathObj == nullptr) {
        UDA_LOG(UDA_LOG_ERROR, "Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr);
        return -1;
    }

    xmlNodeSetPtr nodes = xpathObj->nodesetval;

    if (nodes == nullptr || nodes->nodeNr == 0) {
        UDA_LOG(UDA_LOG_ERROR, "error in XPath request  \n");
        return -1;
    }

    xml_data->data_type = convert_type_string_to_uda_type(type);
    int i;

    if (xml_data->dims == nullptr && index == -1) {
        index = 1;
    }

    size_t data_n = 1;
    if (xml_data->dims != nullptr) {
        for (i = 0; i < xml_data->rank; ++i) {
            data_n *= (xml_data->dims)[i];
        }
    } else {
        data_n = 1;
    }

    double* content = nullptr;
    if (xml_data->data_type != UDA_TYPE_STRING) {
        xmlNodePtr cur = nodes->nodeTab[0];

        if (cur->name == nullptr) {
            UDA_LOG(UDA_LOG_ERROR, "Error: null pointer (nodes->nodeTab[nodeindex]->name) \n");
            return -1;
        }

        size_t n_vals = 0;
        if (xml_data->dims == nullptr && index > 0) {
            content = get_content(cur, &n_vals);
        } else {
            content = get_content(cur, &n_vals);
            if (n_vals != data_n) {
                UDA_LOG(UDA_LOG_ERROR, "Error: incorrect number of points read from XML file\n");
                return -1;
            }
        }
    }

    if (xml_data->data_type == UDA_TYPE_DOUBLE) {
        xml_data->data = (char*)malloc(data_n * sizeof(double));
        if (xml_data->dims == nullptr) {
            ((double*)xml_data->data)[0] = content[index-1];
        } else {
            for (i = 0; i < (xml_data->dims)[0]; i++) {
                ((double*)xml_data->data)[i] = content[i];
            }
        }
    } else if (xml_data->data_type == UDA_TYPE_FLOAT) {
        xml_data->data = (char*)malloc(data_n * sizeof(float));
        if (xml_data->dims == nullptr) {
            ((float*)xml_data->data)[0] = (float)content[index-1];
        } else {
            for (i = 0; i < data_n; i++) {
                ((float*)xml_data->data)[i] = (float)content[i];
            }
        }
    } else if (xml_data->data_type == UDA_TYPE_LONG) {
        xml_data->data = (char*)malloc(data_n * sizeof(long));
        if (xml_data->dims == nullptr) {
            ((long*)xml_data->data)[0] = (long)content[index-1];
        } else {
            for (i = 0; i < data_n; i++) {
                ((long*)xml_data->data)[i] = (long)content[i];
            }
        }
    } else if (xml_data->data_type == UDA_TYPE_INT) {
        xml_data->data = (char*)malloc(data_n * sizeof(int));
        if (xml_data->dims == nullptr) {
            ((int*)xml_data->data)[0] = (int)content[index-1];
        } else {
            for (i = 0; i < data_n; i++) {
                ((int*)xml_data->data)[i] = (int)content[i];
            }
        }
    } else if (xml_data->data_type == UDA_TYPE_SHORT) {
        xml_data->data = (char*)malloc(data_n * sizeof(short));
        if (xml_data->dims == nullptr) {
            ((short*)xml_data->data)[0] = (short)content[index-1];
        } else {
            for (i = 0; i < data_n; i++) {
                ((short*)xml_data->data)[i] = (short)content[i];
            }
        }
    } else if (xml_data->data_type == UDA_TYPE_STRING) {
        char** strings = nullptr;
        size_t n_strings = 0;

        if (nodes->nodeNr == 1 && StringIEquals((char*)nodes->nodeTab[0]->children->content, "Put value here")) {
            xmlXPathFreeObject(xpathObj);
            return -1;
        }

        for (i = 0; i < nodes->nodeNr; ++i) {
            xmlNodePtr cur = nodes->nodeTab[i];

            char* string = (char*)cur->children->content;
            if (STR_STARTSWITH(type, "vec")) {
                char** tokens = SplitString(string, ",");
                int j = 0;
                while (tokens[j] != nullptr) {
                    strings = (char**)realloc(strings, (n_strings + 1) * sizeof(char*));
                    strings[n_strings] = tokens[j];
                    ++n_strings;
                    ++j;
                }
                free(tokens);
            } else {
                strings = (char**)realloc(strings, (n_strings + 1) * sizeof(char*));
                strings[n_strings] = strdup(string);
                ++n_strings;
            }
        }

        strings = (char**)realloc(strings, (n_strings + 1) * sizeof(char*));
        strings[n_strings] = nullptr;

        xml_data->data = (char*)strings;
    } else {
        xmlXPathFreeObject(xpathObj);
        RAISE_PLUGIN_ERROR("Unsupported data type");
    }

    /* Cleanup */
    xmlXPathFreeObject(xpathObj);

    return 0;
}
