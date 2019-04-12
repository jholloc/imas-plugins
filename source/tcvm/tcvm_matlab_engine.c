#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "engine.h"

#define BUFSIZE 256

void printtime(void)
{
    struct timeval tmnow;
    struct tm* tm;
    char buf[30], usec_buf[7];
    gettimeofday(&tmnow, NULL);
    tm = localtime(&tmnow.tv_sec);
    strftime(buf, 30, "%Y-%m-%d %H:%M:%S", tm);
    strncat(buf, ".", 1);
    snprintf(usec_buf, 7, "%06d", (int)tmnow.tv_usec);
    strncat(buf, usec_buf, 6);
    printf("[%s]", buf);
}

Engine* mystartMATLAB(char* path)
{
    char expr[BUFSIZE + 1];
    snprintf(expr, BUFSIZE + 1, "matlab -nodisplay -r \'addpath %s\'", path);
    Engine* ep = engOpen(expr);
    if (!ep) {
        fprintf(stderr, "\nCan't start MATLAB engine\n");
    }

    return ep;
}

int testMATLAB(Engine* ep)
{
    if (!ep) {
        fprintf(stderr, "\nInvalid MATLAB engine pointer\n");
    } else {
        char buffer[BUFSIZE + 1];
        buffer[BUFSIZE] = '\0';
        engOutputBuffer(ep, buffer, BUFSIZE);
        engEvalString(ep, "which myFunction2");
        printf("%s", buffer);
    }

    return 0;
}

int mycloseMATLAB(Engine* ep)
{

    int status = engClose(ep);
    if (status) {
        fprintf(stderr, "\nCan't close MATLAB engine\n");
    }

    return status;
}

int runquery_s(Engine* ep, const char* query, char** result)
{
    int err = 0;

    if (!ep) {
        fprintf(stderr, "\nInvalid MATLAB engine pointer\n");
        return -1;
    }

    // Create a variable for query
    mxArray* mquery = mxCreateString(query);
    // Place the variable query into the MATLAB workspace
    err = engPutVariable(ep, "query", mquery);

    err = engEvalString(ep, "answ=myFunction(query);");
    if (err) {
        fprintf(stderr, "\nError in evaluating MATLAB function myFunction\n");
        return -1;
    }

    printtime();

    mxArray* mansw = NULL;
    if ((mansw = engGetVariable(ep, "answ")) == NULL) {
        fprintf(stderr, "Impossible to retrieve result of myFunction");
    } else {
        *result = mxArrayToString(mansw);
        printf("[myFunction] result for query %s is: %s\n", query, *result);
    }

    // Free arguments and return arrays
    mxDestroyArray(mquery);
    mxDestroyArray(mansw);

    return err;
}

size_t datasize(int dtype)
{
    size_t size = 0;

    if (dtype == 17) {
        // String case
        size = sizeof(char);
    } else {
        // Numeric data
        switch (dtype) {
            case 1 :
                size = sizeof(char);
                break;
            case 2 :
                size = sizeof(short);
                break;
            case 3 :
                size = sizeof(int);
                break;
            case 4 :
                size = sizeof(unsigned int);
                break;
            case 5 :
                size = sizeof(long);
                break;
            case 6 :
                size = sizeof(float);
                break;
            case 7 :
                size = sizeof(double);
                break;
            case 8 :
                size = sizeof(unsigned char);
                break;
            case 9 :
                size = sizeof(unsigned short);
                break;
            case 10 :
                size = sizeof(unsigned long);
                break;
            case 11 :
                size = sizeof(long long);
                break;
            case 12 :
                size = sizeof(unsigned long long);
                break;
            case 17 : // Case 17 should not land here
                fprintf(stderr, "This switch statement should not handle dtype=17");
                break;
            default :
                fprintf(stderr, "Unsupported dtype value: %d", dtype);
                break;
        }
    }

    return size;
}

int dataCharacteristics(mxArray* mdata, int dtype, void** data, size_t* rank, size_t** dims)
{

    size_t dsize;
    size_t numel;
    const size_t* mdims;

    *dims = NULL;
    *data = NULL;

    if (dtype == 17) {
        // String case
        *data = mxArrayToString(mdata);
        *rank = 0;
        mdims = NULL;
    } else {
        // Numeric data
        numel = mxGetNumberOfElements(mdata);
        if (mxIsEmpty(mdata)) {
            // Empty case
            *rank = 0;
            *data = NULL;
            mdims = NULL;
            return 1;
        } else if (mxIsScalar(mdata)) {
            // Scalar case
            *rank = 0;
            mdims = NULL;
        } else {
            // N-D array case
            *rank = mxGetNumberOfDimensions(mdata);
            mdims = mxGetDimensions(mdata);
            if (*rank == 2 && ((mdims)[0] == 1 || (mdims)[1] == 1)) {
                // 1-D vector case
                *rank = 1;
                mdims = &numel;
            }
        }

        // Copy dimensions pointer
        if (*rank > 0) {
            *dims = malloc(*rank * sizeof(size_t));
            memcpy(*dims, mdims, *rank * sizeof(size_t));
        }

        // Copy data pointer
        dsize = datasize(dtype);
        *data = malloc(numel * dsize);
        memcpy(*data, mxGetData(mdata), numel * dsize);

    }

    return 0;
}

char* dataToString(void* data, int dtype)
{

    char* out_print = NULL;

    if (dtype == 17) {
        // String case
        out_print = (char*)data;
    } else {
        // Numeric data
        /* Prepare some output string */
        out_print = malloc(20 * sizeof(char));
        switch (dtype) {
            case 1 :
                sprintf(out_print, "%hhi", *(char*)data);
                break;
            case 2 :
                sprintf(out_print, "%hi", *(short*)data);
                break;
            case 3 :
                sprintf(out_print, "%i", *(int*)data);
                break;
            case 4 :
                sprintf(out_print, "%u", *(unsigned int*)data);
                break;
            case 5 :
                sprintf(out_print, "%li", *(long*)data);
                break;
            case 6 :
                sprintf(out_print, "%g", *(float*)data);
                break;
            case 7 :
                sprintf(out_print, "%g", *(double*)data);
                break;
            case 8 :
                sprintf(out_print, "%hhu", *(unsigned char*)data);
                break;
            case 9 :
                sprintf(out_print, "%hu", *(unsigned short*)data);
                break;
            case 10 :
                sprintf(out_print, "%lu", *(unsigned long*)data);
                break;
            case 11 :
                sprintf(out_print, "%lli", *(long long*)data);
                break;
            case 12 :
                sprintf(out_print, "%llu", *(unsigned long long*)data);
                break;
            case 17 : // Case 17 should not land here
                fprintf(stderr, "This switch statement should not handle dtype=17");
                out_print = "ERROR";
                break;
            default :
                fprintf(stderr, "Unsupported dtype value: %d", dtype);
                out_print = "ERROR";
                break;
        }
    }

    return out_print;
}

int runquery(Engine* ep, const char* query, int shot, int* indices, int nindices, int dtype, void** data, size_t* rank,
             size_t** dims)
{
    int err = 0;

    if (!ep) {
        fprintf(stderr, "\nInvalid MATLAB engine pointer\n");
        return -1;
    }

    // Create a variable for query
    mxArray* mquery = mxCreateString(query);
    // Place the variable query into the MATLAB workspace
    err = engPutVariable(ep, "query", mquery);
    if (err) {
        fprintf(stderr, "\nError in putting variable query\n");
        return -1;
    }

    // Create a variable for shot
    mxArray* mshot = mxCreateDoubleScalar((double)shot);
    // Place the variable shot into the MATLAB workspace
    err = engPutVariable(ep, "shot", mshot);
    if (err) {
        fprintf(stderr, "\nError in putting variable shot\n");
        return -1;
    }

    // Create a variable for indices
    mxArray* mindices = mxCreateNumericMatrix(1, nindices, mxINT32_CLASS, mxREAL);
    int* array = mxGetData(mindices);
    for (mwSize index = 0; index < nindices; index++) {
        array[index] = *(indices + index);
    }
    // Place the variable shot into the MATLAB workspace
    err = engPutVariable(ep, "indices", mindices);
    if (err) {
        fprintf(stderr, "\nError in putting variable indices\n");
        return -1;
    }

    // Create a variable for dtype
    mxArray* mdtype = mxCreateDoubleScalar((double)dtype);
    // Place the variable shot into the MATLAB workspace
    err = engPutVariable(ep, "dtype", mdtype);
    if (err) {
        fprintf(stderr, "\nError in putting variable dtype\n");
        return -1;
    }

    err = engEvalString(ep, "answ=myFunction2(query,shot,indices,dtype);");
    if (err) {
        fprintf(stderr, "\nError in evaluating MATLAB function myFunction2\n");
        return -1;
    }

    // Show current time
    printtime();

    // Get output of myFunction2 call
    static mxArray* mansw = NULL;
    if ((mansw = engGetVariable(ep, "answ")) == NULL) {
        fprintf(stderr, "Impossible to retrieve result of myFunction2");
        err = 1;
    } else {
        int status = (int)mxGetScalar(mxGetCell(mansw, 1));
        char* message = mxArrayToString(mxGetCell(mansw, 2));
        if (status) {
            fprintf(stderr, "myFunction2 throwed an exception:\n");
            fprintf(stderr, message);
            err = 1;
        } else {
            mxArray* mout = mxGetCell(mansw, 0);
            // Get data pointer, rank and dims
            err = dataCharacteristics(mout, dtype, data, rank, dims);
            if (err == 0) {
                /* Prepare some output string */
                char* out_print = dataToString(*data, dtype);
                printf("[myFunction2] result for query %s is: \n", query);
                printf("  data: %s\n", out_print);
                printf("  rank: %d, dims:", *rank);
                for (int i = 0; i < *rank; i++) {
                    printf(" %d", (*dims)[i]);
                }
                printf("\n  status: %d\n", status);
                printf("  message: %s\n", message);
            }
        }
    }

    // Free arguments
    mxDestroyArray(mquery);
    mxDestroyArray(mshot);
    mxDestroyArray(mindices);
    mxDestroyArray(mdtype);
    mxDestroyArray(mansw); //(seems to free more than it advertises)

    return err;
}
