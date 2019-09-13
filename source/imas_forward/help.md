# IMAS_REMOTE plugin

The IMAS_REMOTE plugin is responsible for remote access of IDSs from the IMAS UDA
backend. The low level IMAS calls are mapped to plugin functions which are then used
to read the data using a local version of the IMAS backend.

## Functions

The IMAS_REMOTE plugins responds to the following plugin requests:

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

### open\_pulse
*Arguments:*
: backend\_id (required, int) -- the ID of the backend to use to read the data
: shot (required, int) -- the shot number of the IDS to read
: run (required, int) -- the run number of the IDS to read
: user (required, string) -- the user from whos imasdb we will read the IDS
: tokamak (required, string) -- the tokamak name in the imasdb
: version (required, string) -- the IDS version in the imasdb
: mode (required, int) -- the open mode flags
: options (required, string) -- any options to pass to the selected backend

*Returns:* Signal[datatype=int, rank=0] -- the context ID of the opened pulse

### closePulse
*Arguments:*
: ctxId (required, int) -- the context ID of the open pulse to close
: mode (required, int) -- the close mode flags
: options (required, string) -- any options to pass to the selected backend

*Returns:* Signal[datatype=int, rank=0] -- the context ID of the closed pulse

### beginAction
*Arguments:*
: ctxId (required, int) -- the context ID of an open pulse
: dataObject (required, string) -- the name of the data object to work on
: access (required, int) -- the access type we are performing [read|write|replace]
: range (required, int) -- the range type we are using [global|slice]
: time (required, float) -- the time used in slice operations
: interp (required, int) -- iterpolation method used in slice operations [closest|previous|linear|undefined]

*Returns:* Signal[datatype=int, rank=0] -- the context ID of the open pulse

### endAction
*Arguments:*
: ctxId (required, int) -- the context ID of an open pulse
: type (required, int) -- type type of action we are ending [pulse|opration|arraystruct]

*Returns:* Signal[datatype=int, rank=0] -- the context ID of the open pulse

### writeData
*Arguments:*
: ctxId (required, int) -- the context ID of an open pulse
: field (required, string) -- the name of the field to write
: timebase (required, string) -- the path of the timebase associated with this field
: datatype (required, int) -- the data type being written

*Data:*
The `PUTDATA_BLOCK` must contain the data to write.

*Returns:* Signal[datatype=int, rank=0] -- the context ID of the open pulse

### readData
*Arguments:*
: ctxId (required, int) -- the context ID of an open pulse
: field (required, string) -- the name of the field to read
: timebase (required, string) -- the path of the timebase associated with this field
: datatype (required, int) -- the data type being read

*Returns:* Signal[datatype=various, rank=various] -- the data read from the pulse file

### deleteData
*Arguments:*
: ctxId (required, int) -- the context ID of an open pulse
: path (required, string) -- the path of the data to delete

*Returns:* Signal[datatype=int, rank=0] -- the context ID of the open pulse

### beginArraystructAction
*Arguments:*
: ctxId (required, int) -- the context ID of an open pulse
: path (required, string) -- the path of the array structure
: timebase (required, string) -- the timebase associated with the array structure

*Returns:* Signal[datatype=int, rank=0] -- the size of the structure array