#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_helpers.h"
#include "setup.inc"

#include "../imas_mapping_plugin.h"

TEST_CASE( "Test open", "[IMAS_MAPPING][OPEN]" )
{
    IDAM_PLUGIN_INTERFACE plugin_interface = uda::test::generate_plugin_interface("imas_mapping::open(shot=999, run=1)");

    int res = imas_mapping_plugin(&plugin_interface);

    REQUIRE( res == 0 );
}
