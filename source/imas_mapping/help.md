# IMAS_MAPPING plugin

The IMAS_MAPPING plugin is responsible for handling data access requests from
the IMAS UDA backend and responding to those requests with data mapped from
different experimental machines. The actual machine specific mapping is done
via a machine specific plugin (i.e. the EXP2IMAS plugin) with the plugin to use
being defined in the machines.txt file.

## Functions

The IMAS_PARTIAL plugins responds to the following plugin requests:

### help
*Arguments:* None
*Returns:* Signal[datatype=STRING, rank=0]

The `help` method returns the help documentation for this plugin.

### version
*Arguments:* None
*Returns:* Signal[datatype=int, rank=0]

The `version` method returns the version of the plugin being called.

### buildDate
*Arguments:* None
*Returns:* Signal[datatype=STRING, rank=0]

The `build_date` method returns the date this plugin was built.

### defaultMethod
*Arguments:* None
*Returns:* Signal[datatype=STRING, rank=0]

The `default_method` returns the method that will be called if no function name is provided.

### maxInterfaceVersion
*Arguments:* None
*Returns:* Signal[datatype=int, rank=0]

The `max_interface_version` method returns the highest UDA plugin interface version that this plugin will respond to.

### maxInterfaceVersion
*Arguments:* None
*Returns:* Signal[datatype=int, rank=0]

### openPulse
*Arguments:*
: shot (required, int) -- the shot number of the IDS to read
: run (required, int) -- the run number of the IDS to read
: user (required, string) -- the user from whos imasdb we will read the IDS
: tokamak (required, string) -- the tokamak name in the imasdb
: version (required, string) -- the IDS version in the imasdb

*Returns:* Signal[datatype=int, rank=0]

### closePulse
*Arguments:*
: ctxId (required, int) -- the context ID of the open pulse to close

*Returns:* Signal[datatype=int, rank=0]

### beginAction
*Arguments:*
: ctxId (required, int) -- the context ID of an open pulse
: dataObject (required, string) -- the name of the data object to work on
: access (required, int) -- the access type we are performing [read|write|replace]
: range (required, int) -- the range type we are using [global|slice]
: time (required, float) -- the time used in slice operations
: interp (required, int) -- iterpolation method used in slice operations [closest|previous|linear|undefined]

*Returns:* Signal[datatype=int, rank=0]

### endAction
*Arguments:*
: ctxId (required, int) -- the context ID of an open pulse
: type (required, int) -- type type of action we are ending [pulse|opration|arraystruct]

*Returns:* Signal[datatype=int, rank=0]

### writeData
*Arguments:* None
*Returns:* None

The `IMAS_MAPPING` plugin cannot be used to write data.

### readData
*Arguments:*
: ctxId (required, int) -- the context ID of an open pulse
: field (required, string) -- the name of the field to read
: timebase (required, string) -- the path of the timebase associated with this field
: datatype (required, int) -- the data type being read
: index (optional, int) -- the arraystruct index, required when reading an arraystruct field

*Returns:* Signal[datatype=int, rank=0]

Generates the request `"<plugin_name>::read(experiment='<tokamak>', element='<ids_name>/<path>', shot=<shot>, indices='<indices>', dtype=<datatype>, IDS_version='<version>', run=<run>, user='<user>')"` and passes it to the machine specific plugin, where:
: `<plugin_name>` -- the plugin to call as mapped using the `machines.txt` using the tokamak and IDS names
: `<tokamak>` -- the tokamak name provided in the `openPulse` call
: `<ids_name>` -- the IDS name provided in the `beginAction` call
: `<path>` -- the path built from the current arraystruct context and the specified `field`
: `<shot>` -- the shot as provided in the `openPulse` call
: `<indices>` -- the numeric values corresponding to `#` in the path
: `<data_type>` -- the UDA data type as mapped from the IMAS data type provided in `datatype`
: `<version>` -- the version as specified in the `openPulse` call
: `<run>` -- the run as specified in the `openPulse` call
: `<user>` -- the user as specified in the `openPulse` call

### deleteData
*Arguments:* None
*Returns:* None

The `IMAS_MAPPING` plugin cannot be used to delete data.

### beginArraystructAction
*Arguments:*
: ctxId (required, int) -- the context ID of an open pulse
: path (required, string) -- the path of the array structure

*Returns:* The result of calling the machine specific plugin

Generates the request `"<plugin_name>::read(experiment='<tokamak>', element='<ids_name>/<path>/Shape_of', shot=<shot>, indices='<indices>', dtype=<data_type>, IDS_version='')"` and passes this to the machine specific plugin, where:
: `<plugin_name>` -- the plugin to call as mapped using the `machines.txt` using the tokamak and IDS names
: `<tokamak>` -- the tokamak name provided in the `openPulse` call
: `<ids_name>` -- the IDS name provided in the `beginAction` call
: `<path>` -- the path specified in this call, but with numeric values replaced with `#`
: `<shot>` -- the shot as provided in the `openPulse` call
: `<indices>` -- the numeric values extracted from the path
: `<data_type>` -- equal to `UDA_TYPE_INT`

### get
*Arguments:*
: expName (required, string) -- the machine name
: group (required, string) -- the IDS name
: type (required, string) -- the data type [double|integer|string]
: variable (required, string) -- the data path
: shot (required, int) -- the pulse shot
: run (required, int) -- the pulse run
: user (required, string) -- the user name

### getDim
*Arguments:*
: expName (required, string) -- the machine name
: group (required, string) -- the IDS name
: variable (required, string) -- the data path
: shot (required, int) -- the pulse shot
: run (required, int) -- the pulse run
: user (required, string) -- the user name