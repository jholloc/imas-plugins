#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <imasdd/imasdd_plugin.h>

#include "test_helpers.h"

namespace {

std::vector<std::string> received_requests;

}

extern "C" int mock_plugin_func(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    received_requests.emplace_back(idam_plugin_interface->request_block->signal);

    if (std::string(idam_plugin_interface->request_block->function) == "getDim") {
        setReturnDataIntScalar(idam_plugin_interface->data_block, 3, nullptr);
    }

    return 0;
}

void add_mock_plugin(IDAM_PLUGIN_INTERFACE* idam_plugin_interface)
{
    auto pluginlist = const_cast<PLUGINLIST*>(idam_plugin_interface->pluginList);

    pluginlist->plugin = (PLUGIN_DATA*)realloc(pluginlist->plugin, (pluginlist->mcount + 1) * sizeof(PLUGIN_DATA));
    ++pluginlist->mcount;
    ++pluginlist->count;

    uda::test::add_plugin(pluginlist, "IMAS_MAPPING::get()", mock_plugin_func);
}

void setup()
{
    received_requests.clear();
    setenv("UDA_IMASDD_IDSDEF_PATH", "/Users/jhollocombe/Projects/iter-plugins/source/imasdd/IDSDef.xml", true);
}

TEST_CASE( "get root magnetics", "[MAG]" )
{
    setup();

    IDAM_PLUGIN_INTERFACE interface = uda::test::generate_plugin_interface("imasdd::get(path=/magnetics)");

    add_mock_plugin(&interface);

    int rc = imasdd_plugin(&interface);

    REQUIRE( rc == 0 );

    std::vector<std::string> expected = {
        "getDim(group='magnetics', variable='flux_loop', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='flux_loop/1/position', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='flux_loop/2/position', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='flux_loop/3/position', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='bpol_probe', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='method', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='ids_properties/comment', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='ids_properties/homogeneous_time', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='ids_properties/source', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='ids_properties/provider', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='ids_properties/creation_date', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='flux_loop', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/name', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/name', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/name', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/identifier', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/identifier', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/identifier', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='flux_loop/1/position', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='flux_loop/2/position', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='flux_loop/3/position', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/flux/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/flux/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/flux/data_error_upper', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/flux/data_error_upper', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/data_error_upper', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/flux/data_error_lower', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/flux/data_error_lower', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/data_error_lower', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/flux/data_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/flux/data_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/data_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/flux/time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/flux/time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='bpol_probe', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/name', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/name', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/name', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/identifier', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/identifier', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/identifier', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/position/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/position/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/position/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/position/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/position/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/position/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/position/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/position/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/position/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/position/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/position/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/position/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/position/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/position/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/position/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/position/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/position/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/position/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/position/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/position/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/position/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/position/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/position/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/position/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/position/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/position/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/position/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/position/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/position/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/position/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/position/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/position/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/position/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/position/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/position/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/position/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/poloidal_angle', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/poloidal_angle', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/poloidal_angle', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/poloidal_angle_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/poloidal_angle_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/poloidal_angle_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/poloidal_angle_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/poloidal_angle_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/poloidal_angle_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/poloidal_angle_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/poloidal_angle_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/poloidal_angle_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/toroidal_angle', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/toroidal_angle', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/toroidal_angle', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/toroidal_angle_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/toroidal_angle_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/toroidal_angle_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/toroidal_angle_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/toroidal_angle_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/toroidal_angle_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/toroidal_angle_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/toroidal_angle_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/toroidal_angle_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/area', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/area', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/area', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/area_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/area_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/area_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/area_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/area_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/area_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/area_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/area_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/area_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/length', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/length', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/length', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/length_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/length_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/length_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/length_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/length_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/length_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/length_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/length_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/length_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/turns', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/turns', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/turns', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/field/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/field/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/field/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/field/data_error_upper', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/field/data_error_upper', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/field/data_error_upper', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/field/data_error_lower', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/field/data_error_lower', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/field/data_error_lower', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/field/data_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/field/data_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/field/data_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/1/field/time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/2/field/time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='bpol_probe/3/field/time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='method', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/1/name', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/2/name', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/3/name', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/1/ip/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/2/ip/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/3/ip/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/1/ip/data_error_upper', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/2/ip/data_error_upper', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/3/ip/data_error_upper', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/1/ip/data_error_lower', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/2/ip/data_error_lower', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/3/ip/data_error_lower', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/1/ip/data_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/2/ip/data_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/3/ip/data_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/1/ip/time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/2/ip/time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/3/ip/time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/1/diamagnetic_flux/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/2/diamagnetic_flux/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/3/diamagnetic_flux/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/1/diamagnetic_flux/data_error_upper', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/2/diamagnetic_flux/data_error_upper', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/3/diamagnetic_flux/data_error_upper', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/1/diamagnetic_flux/data_error_lower', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/2/diamagnetic_flux/data_error_lower', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/3/diamagnetic_flux/data_error_lower', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/1/diamagnetic_flux/data_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/2/diamagnetic_flux/data_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/3/diamagnetic_flux/data_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/1/diamagnetic_flux/time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/2/diamagnetic_flux/time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='method/3/diamagnetic_flux/time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='code/name', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='code/commit', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='code/version', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='code/repository', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='code/parameters', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='code/output_flag', type='integer', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
    };

    REQUIRE( received_requests == expected );
}

TEST_CASE( "get all magnetics flux_loops", "[MAG]" )
{
    setup();

    IDAM_PLUGIN_INTERFACE interface = uda::test::generate_plugin_interface("imasdd::get(path=/magnetics/flux_loop)");

    add_mock_plugin(&interface);

    int rc = imasdd_plugin(&interface);

    REQUIRE( rc == 0 );

    std::vector<std::string> expected = {
        "getDim(group='magnetics', variable='flux_loop', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='flux_loop/1/position', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='flux_loop/2/position', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='flux_loop/3/position', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='flux_loop', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/name', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/name', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/name', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/identifier', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/identifier', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/identifier', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='flux_loop/1/position', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='flux_loop/2/position', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='flux_loop/3/position', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/1/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/2/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/position/3/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/1/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/2/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/position/3/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/flux/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/flux/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/flux/data_error_upper', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/flux/data_error_upper', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/data_error_upper', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/flux/data_error_lower', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/flux/data_error_lower', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/data_error_lower', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/flux/data_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/flux/data_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/data_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/1/flux/time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/2/flux/time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
    };

    REQUIRE( received_requests == expected );
}

TEST_CASE( "get magnetics flux_loop", "[MAG]" )
{
    setup();

    IDAM_PLUGIN_INTERFACE interface = uda::test::generate_plugin_interface("imasdd::get(path=/magnetics/flux_loop/3)");

    add_mock_plugin(&interface);

    int rc = imasdd_plugin(&interface);

    REQUIRE( rc == 0 );

    std::vector<std::string> expected = {
        "getDim(group='magnetics', variable='flux_loop/3/position', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/name', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/identifier', type='string', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "getDim(group='magnetics', variable='flux_loop/3/position', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/r', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/r_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/r_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/r_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/z', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/z_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/z_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/z_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/phi', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/phi_error_upper', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/phi_error_lower', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/1/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/2/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/position/3/phi_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/data_error_upper', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/data_error_lower', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/data_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
    };

    REQUIRE( received_requests == expected );
}

TEST_CASE( "get magnetics flux_loop flux structure", "[MAG]" )
{
    setup();

    IDAM_PLUGIN_INTERFACE interface = uda::test::generate_plugin_interface("imasdd::get(path=/magnetics/flux_loop/3/flux)");

    add_mock_plugin(&interface);

    int rc = imasdd_plugin(&interface);

    REQUIRE( rc == 0 );

    std::vector<std::string> expected = {
        "get(group='magnetics', variable='flux_loop/3/flux/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/data_error_upper', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/data_error_lower', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/data_error_index', type='integer', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)",
        "get(group='magnetics', variable='flux_loop/3/flux/time', type='float', rank=0, idx=0, expName='JET', shot=84600, run=0, user=jholloc)"
    };

    REQUIRE( received_requests == expected );
}

TEST_CASE( "get magnetics flux_loop flux data", "[MAG]" )
{
    setup();

    IDAM_PLUGIN_INTERFACE interface = uda::test::generate_plugin_interface("imasdd::get(path=/magnetics/flux_loop/3/flux/data)");

    add_mock_plugin(&interface);

    int rc = imasdd_plugin(&interface);

    REQUIRE( rc == 0 );

    std::vector<std::string> expected = {
        "get(group='magnetics', variable='flux_loop/3/flux/data', type='float', rank=1, idx=0, expName='JET', shot=84600, run=0, user=jholloc)"
    };

    REQUIRE( received_requests == expected );
}
