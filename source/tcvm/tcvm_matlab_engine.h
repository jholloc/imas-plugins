#include "engine.h"

Engine *mystartMATLAB(char* path);
int testMATLAB(Engine* ep);
int mycloseMATLAB(Engine *ep);
int runquery_s(Engine *ep, const char *query, char **result);
int runquery(Engine *ep, const char *query, int shot, int *indices, int nindices, int dtype, void **data, size_t *rank, size_t **dims);
