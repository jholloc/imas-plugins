# IMAS plugin

The IMAS plugin is responsible for responding to requests from the IMAS UDA backend.
The low level IMAS calls are mapped to plugin functions which are then used
to read the data using either a local version of the IMAS backend or another UDA plugin
to map non-IMAS data into the IMAS structure.

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
