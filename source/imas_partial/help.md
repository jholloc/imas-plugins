# IMAS_PARTIAL plugin

The IMAS_PARTIAL plugin is responsible for reading chunks of an IDS by dynamically parsing the IDSDef.xml data dictionary. This is used to generate the IMAS path requests which can be used to read the data using the IMAS MDS+ backend or passed to another plugin.

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

### get
*Arguments:*
: shot (required, int) -- the shot number of the IDS to read from
: run (required, int) -- the run number of the IDS to read from
: user (required, string) -- the user from whom's imasdb we will read the IDS from
: tokamak (required, string) -- the tokamak name
: version (required, string) -- the IDS data dictionary version
: path (required, string) -- the IMAS path to get the data for

*Returns:* StructuredData
```c++
struct Data {
    const char* name = nullptr;
    const unsigned char* data = nullptr;
    int rank = 0;
    int dims[64] = {0};
    int datatype = 0;
};

struct DataList {
    Data* list;
    int size;
};
```

*Example:* The following example returns all flux data for flux loops 3 and 4.
```c++
"IMAS_PARTIAL::get(shot=12345, run=0, user='test', tokamak='iter', version='3.21.1', path='magnetics/flux/3:4/flux/data')"
```

The `get` method uses the provided `path` to unpack all the IMAS path requests required to populate that element of the
IDS and below. The data for each IMAS path is then fetched and all the data is returned as structured data. If `path` is
 equal to a top level IDS (i.e. `path='magnetics'`) then the entire IDS is returned.