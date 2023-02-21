# IMAS plugin

The IMAS plugin is responsible for responding to requests from the IMAS UDA backend.
The low level IMAS calls are mapped to plugin functions which are then used
to read the data using either a local version of the IMAS backend or another UDA plugin
to map non-IMAS data into the IMAS structure.

## Functions

The IMAS plugin responds to the following plugin requests:

### help
*Arguments:* None

*Returns:* Signal[datatype=STRING, rank=0]

The `help` function returns the help documentation for this plugin.

### version
*Arguments:* None

*Returns:* Signal[datatype=int, rank=0]

The `version` function returns the version of the plugin being called.

### buildDate
*Arguments:* None

*Returns:* Signal[datatype=STRING, rank=0]

The `buildDate` function returns the date this plugin was built.

### defaultMethod
*Arguments:* None

*Returns:* Signal[datatype=STRING, rank=0]

The `defaultMethod` function returns the method that will be called if no function name is provided.

### maxInterfaceVersion
*Arguments:* None

*Returns:* Signal[datatype=int, rank=0]

The `maxInterfaceVersion` function returns the highest UDA plugin interface version that this plugin will respond to.

### get
*Arguments:*

| Name           | Required   | Type   | Description                                                  |
|----------------|------------|--------|--------------------------------------------------------------|
| uri            | True       | string | uri for data                                                 |
| access         | True       | string | read access mode `[read&#124;write&#124;replace]`            |
| range          | True       | string | range mode `[global&#124;slice]`                             |
| time           | True       | float  | slice time (ignored for global range mode)                   |
| interp         | True       | string | interpolation mode (ignored for global range mode)           |
| path           | True       | string | IDS path, i.e. `flux_loop[3]/flux/data`                      |
| datatype       | True       | string | IDS data type `[char&#124;integer&#124;double&#124;complex]` |
| rank           | True       | int    | rank of data to return                                       |
| is_homogeneous | True       | int    | flag specifying whether data has been stored homogeneously   |
| dynamic_flags  | True       | in     | - flags specifying dynamic status for each level of the path |

*Returns:* CapNp serialised tree of depth 1, where each leaf node contains the name and data of a returned IMAS data node

Returns the IMAS data for the given IDS path. If the database entry is not currently open then it will be opened.

### open
*Arguments:*

| Name   | Required   | Type   | Description                                                      |
|--------|------------|--------|------------------------------------------------------------------|
| uri    | True       | string | uri for data                                                     |
| mode   | True       | string | open mode `[open&#124;create&#124;force_open&#124;force_create]` |

*Returns:* Integer scalar containing the pulse context handle.

Open an IDS database entry, caching the pulse context handle.
Example:
IMAS::open(shot=1000, run=1, user='test', tokamak='iter', version='3')

### close
*Arguments:*

| Name   | Required   | Type   | Description   |
|--------|------------|--------|---------------|
| uri    | True       | string | uri for data  |
| mode   | True       | int    | close mode    |

*Returns:* Integer scalar -1

Closes the IMAS database entry corresponding to the given arguments. The entry must have been opened by calling the
open(...) or get(...) functions.
Example:
IMAS::close(shot=1000, run=1, user='test', tokamak='iter', version='3')

