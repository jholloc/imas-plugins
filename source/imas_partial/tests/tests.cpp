#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <string>
#include <vector>

#include <imas_partial_plugin.h>
#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>

IDAM_PLUGIN_INTERFACE generate_plugin_interface(const std::string& function, const std::vector<std::pair<std::string, std::string>>& arguments)
{
    IDAM_PLUGIN_INTERFACE interface = {0};

    interface.dbgout = stdout;
    interface.errout = stderr;

    interface.data_block = (DATA_BLOCK*)malloc(sizeof(DATA_BLOCK));
    initDataBlock(interface.data_block);

    interface.request_block = (REQUEST_BLOCK*)malloc(sizeof(REQUEST_BLOCK));
    initRequestBlock(interface.request_block);

    strcpy(interface.request_block->function, function.c_str());

    initNameValueList(&interface.request_block->nameValueList);
    interface.request_block->nameValueList.listSize = (int)arguments.size();
    interface.request_block->nameValueList.pairCount = (int)arguments.size();
    interface.request_block->nameValueList.nameValue = (NAMEVALUE*)calloc(arguments.size(), sizeof(NAMEVALUE));
    int i = 0;
    for (auto& pair : arguments) {
        NAMEVALUE* nv = &interface.request_block->nameValueList.nameValue[i];
        nv->pair = strdup((pair.first + "=" + pair.second).c_str());
        nv->name = strdup(pair.first.c_str());
        nv->value = strdup(pair.second.c_str());
        ++i;
    }

    return interface;
}

TEST_CASE( "get flux_loop flux data", "[MAG]" )
{
    IDAM_PLUGIN_INTERFACE interface = generate_plugin_interface("open", {});

    int rc = imasPartial(&interface);

    free(interface.request_block);
    free(interface.data_block);

    interface = generate_plugin_interface("get", {{"path", "/magnetics/flux_loop/3/flux/data"}});

    rc = imasPartial(&interface);

    REQUIRE( rc == 0 );
}

TEST_CASE( "get flux_loop flux", "[MAG]" )
{
    IDAM_PLUGIN_INTERFACE interface = generate_plugin_interface("open", {});

    int rc = imasPartial(&interface);

    free(interface.request_block);
    free(interface.data_block);

    interface = generate_plugin_interface("get", {{"path", "/magnetics/flux_loop/3/flux"}});

    rc = imasPartial(&interface);

    REQUIRE( rc == 0 );
}

TEST_CASE( "get flux_loop", "[MAG]" )
{
    IDAM_PLUGIN_INTERFACE interface = generate_plugin_interface("open", {});

    int rc = imasPartial(&interface);

    free(interface.request_block->nameValueList.nameValue);
    free(interface.request_block);
    free(interface.data_block);

    interface = generate_plugin_interface("get", {{"path", "/magnetics/flux_loop/3"}});

    rc = imasPartial(&interface);

    free(interface.request_block->nameValueList.nameValue);
    free(interface.request_block);
    free(interface.data_block);

    REQUIRE( rc == 0 );
}

TEST_CASE( "get flux_loops", "[MAG]" )
{
    IDAM_PLUGIN_INTERFACE interface = generate_plugin_interface("open", {});

    int rc = imasPartial(&interface);

    free(interface.request_block->nameValueList.nameValue);
    free(interface.request_block);
    free(interface.data_block);

    interface = generate_plugin_interface("get", {{"path", "/magnetics/flux_loop"}});

    rc = imasPartial(&interface);

    free(interface.request_block->nameValueList.nameValue);
    free(interface.request_block);
    free(interface.data_block);

    REQUIRE( rc == 0 );

    REQUIRE( interface.data_block->data_n == 210 );

    auto data_blocks = (DATA_BLOCK*)interface.data_block->data;

    REQUIRE( std::string{data_blocks[14].data_label} == "magnetics/flux_loop/1/flux/data" );
    REQUIRE( data_blocks[14].data_n == 1000 );
    REQUIRE( data_blocks[14].data_type == UDA_TYPE_DOUBLE );
    REQUIRE( ((double*)data_blocks[14].data)[0] == Approx(-10) );
}

TEST_CASE( "get all magnetics", "[MAG]" )
{
    IDAM_PLUGIN_INTERFACE interface = generate_plugin_interface("open", {});

    int rc = imasPartial(&interface);

    free(interface.request_block->nameValueList.nameValue);
    free(interface.request_block);
    free(interface.data_block);

//    interface = generate_plugin_interface("get", {{"path", "/magnetics"}});
//
//    rc = imasPartial(&interface);
//
//    free(interface.request_block->nameValueList.nameValue);
//    free(interface.request_block);
//    free(interface.data_block);

    REQUIRE( rc == 0 );
}
