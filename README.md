# ITER UDA plugins

This is the repository for ITER related UDA plugins. For the core UDA repository
see https://github.com/ukaea/uda.

## Building the plugins

This plugin repository uses CMake to configure which plugins to build and locate the dependencies need to build the
plugins. This makes use of the "plugin" macro imported from the cmake/plugins.cmake file.

### Configuring with CMake

By default, it will attempt to build every plugin it can &ndash; i.e. every plugin for which the dependencies can be
located. If a plugin is not being built then a warning will be printed to the terminal when CMake is run specifying
which dependency cannot be found.

In order to run CMake to configure the build you need to run:

```bash
cmake3 -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$UDA_HOME
```

This will create a directory called "build" that contains all the GNU makefiles required to build the plugins.

If you wish to only build specific plugins you can use the "-DBUILD_PLUGINS" CMake option to specify the plugins you'd
like to build as a semicolon separated list, for example the following will only attempt to configure the exp2imas and 
imas_mapping plugins:

```bash
cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$UDA_HOME -DBUILD_PLUGINS="exp2imas;imas_mapping"
```

### Building and installing

Once the plugin build has be configured using CMake you can build the code using:

```bash
cd build
make
```

And then install the plugins into the directory specified by the "CMAKE_INSTALL_PREFIX" CMake argument:

```bash
make install
```

## Activating the plugins

Once the plugins have been built and installed they need to be activated in the UDA server. As part of the CMake
configuration step a script is generated to activate all built plugins. This script can be found in the `build/scripts`
directory as is called `activate-plugins.sh`.

This script loops over all the built plugins calling the UDA helper program `install_plugin` for each plugin. This call
looks like:

```bash
$UDA_HOME/bin/install_plugin -u $PLUGINS_HOME install $PLUGIN
```

Where `$PLUGINS_HOME` is the location the plugins where installed to and `$PLUGIN` is the name of the plugin.

## Plugins

A description of the plugins included in this repository is given below:

| Name                        | Description                                                                                                          | Dependencies       |
|-----------------------------|----------------------------------------------------------------------------------------------------------------------|--------------------|
| exp2imas                    | Uses the EXP2ITM mappings to return data from experimental data sources.                                             | MDSplus, LibSSH    |
| hl2a                        | Uses the WEST HL2A data access library to return data to IMAS.                                                       | MDSplus, LibHL2A   |
| [imas](source/imas/help.md) | Handles requests from UDA backend for IMAS AL5 for either remote data or mapped experimental data.                   | IMAS               |
| imas_forward                | Forwards IMAS related requests to another UDA server.                                                                | IMAS               |
| imas_mapping                | Forwards IMAS low-level data requests to plugin determined by machine name.                                          | IMAS               |
| imas_old                    | Deprecated plugin for handling IMAS data requests.                                                                   | MDSplus, HDF5      |
| imas_partial                | Uses data dictionary to expand partial IMAS data request and return data for expanded tree.                          | MDSplus, IMAS      |
| imas_remote                 | Handles IMAS low-level data requests using IMAS low-level library to allow for IMAS remote data access.              | IMAS               |
| imas_uda                    | Converts IMAS data access request to call to signal/source requests from another UDA server.                         |                    |
| imasdd                      | Uses data dictionary to expand IMAS data request and forward each sub-request to IMAS_MAPPING plugin.                |                    |
| iter_md                     | Uses the ITER machine description database to handle IMAS data requests.                                             | ITERMD, PostgreSQL |
| jet_equilibrium             | Has a special case handling for JET equilibrium profiles_1d mapping, otherwise forwards requests to EXP2IMAS plugin. |                    |
| jet_magnetics               | Proof of concept plugin for returning JET magnetics machine description from JET sensors file.                       |                    |
| jet_summary                 | Plugin for populating the summary IDS for JET.                                                                       | MDSplus            |
| livedisplay                 | Deprecated plugin for returning predefined IDS structures to CODAC live-display system.                              |                    |
| mast_imas                   | Deprecated plugin for returning MAST data for IMAS magnetics IDS data access requests.                               | PostgreSQL         |
| tcv                         | Plugin for forwarding signal requests to TCV MDS+ server.                                                            | MDSplus            |
| tcvm                        | Plugin which uses Matlab scripts to map TCV data for response to IMAS data access requests.                          | Matlab             |
| tore_supra                  | Maps IDS data requests to MDS+ signal requests for Tore Supra experiment.                                            | MDSplus            |
| west                        | Maps IDS data requests to MDS+ signal requests for WEST experiment.                                                  | MDSplus, LibTS     |
| west_tunnel                 | Plugin for opening an SSH tunnel through to the WEST UDA server.                                                     | LibSSH             |

