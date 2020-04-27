#ifndef HL2A_FLUX_LOOP_DATA_H
#define HL2A_FLUX_LOOP_DATA_H

#include <clientserver/udaStructs.h>
#ifdef __cplusplus
extern "C" {
#endif
	void magnetics_flux_loop_xdata(int shotNumber, DATA_BLOCK* data_block, int* nodeIndices);
	void magnetics_flux_loop_ydata(int shotNumber, DATA_BLOCK* data_block, char * system, char * channel);

	void setReturnHL2AData(DATA_BLOCK* data_block, char * channel, int channelid, int dataCount, int dtType, void * xDatas, void * yDatas, int yDataLength);
#ifdef __cplusplus
}
#endif


#endif // IDAM_PLUGIN_HL2A_SOFT_X_RAYS_H