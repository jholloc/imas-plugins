#include "tcv_mds.h"
#include <stdlib.h>
#include <mdslib.h>

#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>
#include <clientserver/errorLog.h>
#include <logging/logging.h>
#include <clientserver/stringUtils.h>

#define status_ok(status) (((status) & 1) == 1)

int get_signal_length(const char *signal)
{
    /* local vars */
    int dtype_long = DTYPE_LONG;
    char buf[1024];
    int size;
    int null = 0;
    int idesc = descr(&dtype_long, &size, &null);
    int status;

    /* init buffer */
    memset(buf, 0, sizeof(buf));

    /* put SIZE() TDI function around signal name */
    snprintf(buf, sizeof(buf) - 1, "SIZE(%s)", signal);

    /* use MdsValue to get the signal length */
    status = MdsValue(buf, &idesc, &null, NULL);
    if (!((status & 1) == 1)) {
	fprintf(stderr, "Unable to get length of %s.\n", signal);
	return -1;
    }

    /* return signal length */
    return size;

}

int tcv_mds_get(const char *signalName, int shot, float **time, float **data, int *len)
{

   UDA_LOG(UDA_LOG_DEBUG, "TCV: Entering in tcv_mds_get()\n");

    int dtype_float = DTYPE_FLOAT;
    int null = 0;
    int status;
    /* Connect to MDSplus */
    int socket = MdsConnect("localhost:8001");
    UDA_LOG(UDA_LOG_DEBUG, "TCV: MDS+ socket connection successfull ?\n");
    if (socket == -1) {
            UDA_LOG(UDA_LOG_DEBUG, "TCV: Error connecting to localhost.\n");
	    fprintf(stderr, "Error connecting to localhost.\n");
    	return -1;
    } else {
      UDA_LOG(UDA_LOG_DEBUG, "TCV: Connection to localhost:8001 successfull\n");
    }

    /* Open tcv_shot tree for requested shot */
    status = MdsOpen("tcv_shot",&shot);
    UDA_LOG(UDA_LOG_DEBUG, "TCV: MDS+ tree open successful ?\n");
    if (!((status & 1) == 1)) {
            UDA_LOG(UDA_LOG_DEBUG, "TCV: Error opening the tcv_shot tree.\n");
            fprintf(stderr, "Error opening the tcv_shot tree.\n");
        return -1;
    } else {
      UDA_LOG(UDA_LOG_DEBUG, "TCV: MDS+ tree open successfull\n");
    }

    char buf[1024];
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf) - 1, "%s", signalName);

    *len = get_signal_length(buf);

    if (len < 0) {
            UDA_LOG(UDA_LOG_DEBUG, "TCV: Unable to get signal length of %s\n", signalName);
            UDA_LOG(UDA_LOG_ERROR, "TCV: Unable to get signal length of %s\n", signalName);
	    fprintf(stderr, "Unable to get signal length.\n");
	    return -1;
    }

    *time = malloc(*len * sizeof(float));
    *data = malloc(*len * sizeof(float));

    int fdesc = descr(&dtype_float, *time, len, &null);
    int rlen = 0;

    //Get time data
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf) - 1, "dim_of(%s, 0)", signalName);

    status = MdsValue(buf, &fdesc, &null, &rlen, NULL);
    if (!((status & 1) == 1)) {
	    fprintf(stderr, "Unable to get signal.\n");
    	return -1;
    }

    fdesc = descr(&dtype_float, *data, len, &null);

    //Get data
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf) - 1, "%s", signalName);


    status = MdsValue(buf, &fdesc, &null, &rlen, NULL);
    if (!((status & 1) == 1)) {
	    fprintf(stderr, "Unable to get signal.\n");
	    return -1;
    }


    return 0;
}
