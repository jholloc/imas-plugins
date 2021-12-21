#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <string>
#include <vector>

#include <imas_partial_plugin.h>
#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>
#include <structures/struct.h>

IDAM_PLUGIN_INTERFACE generate_plugin_interface(const std::string& function, const std::vector<std::pair<std::string, std::string>>& arguments)
{
    IDAM_PLUGIN_INTERFACE interface = {0};

    interface.dbgout = stdout;
    interface.errout = stderr;

    interface.data_block = new DATA_BLOCK;
    initDataBlock(interface.data_block);

    interface.request_data = new REQUEST_DATA;
    initRequestData(interface.request_data);

    strcpy(interface.request_data->function, function.c_str());

    initNameValueList(&interface.request_data->nameValueList);
    interface.request_data->nameValueList.listSize = (int)arguments.size();
    interface.request_data->nameValueList.pairCount = (int)arguments.size();
    interface.request_data->nameValueList.nameValue = (NAMEVALUE*)calloc(arguments.size(), sizeof(NAMEVALUE));
    int i = 0;
    for (auto& pair : arguments) {
        NAMEVALUE* nv = &interface.request_data->nameValueList.nameValue[i];
        nv->pair = strdup((pair.first + "=" + pair.second).c_str());
        nv->name = strdup(pair.first.c_str());
        nv->value = strdup(pair.second.c_str());
        ++i;
    }

    interface.userdefinedtypelist = new USERDEFINEDTYPELIST;
    initUserDefinedTypeList(interface.userdefinedtypelist);

    interface.logmalloclist = new LOGMALLOCLIST;
    initLogMallocList(interface.logmalloclist);

    return interface;
}

TEST_CASE( "get flux_loop flux data", "[MAG]" )
{
    auto user = getenv("USER");

    IDAM_PLUGIN_INTERFACE interface = generate_plugin_interface("get", {
        {"shot", "1000"},
        {"run", "0"},
        {"user", user},
        {"tokamak", "test"},
        {"version", "3"},
        {"path", "/magnetics/flux_loop/3/flux/data"}
    });

    int rc = imasPartial(&interface);

    delete interface.request_data->nameValueList.nameValue;
    delete interface.request_data;
    delete interface.data_block;
    delete interface.userdefinedtypelist;
    delete interface.logmalloclist;

    REQUIRE( rc == 0 );
}

TEST_CASE( "get flux_loop flux", "[MAG]" )
{
    auto user = getenv("USER");
    
    IDAM_PLUGIN_INTERFACE interface = generate_plugin_interface("get", {
            {"shot", "1000"},
            {"run", "0"},
            {"user", user},
            {"tokamak", "test"},
            {"version", "3"},
            {"path", "/magnetics/flux_loop/3/flux"}
    });

    int rc = imasPartial(&interface);

    delete interface.request_data->nameValueList.nameValue;
    delete interface.request_data;
    delete interface.data_block;
    delete interface.userdefinedtypelist;
    delete interface.logmalloclist;

    REQUIRE( rc == 0 );
}

TEST_CASE( "get flux_loop", "[MAG]" )
{
    auto user = getenv("USER");

    IDAM_PLUGIN_INTERFACE interface = generate_plugin_interface("get", {
            {"shot", "1000"},
            {"run", "0"},
            {"user", user},
            {"tokamak", "test"},
            {"version", "3"},
            {"path", "/magnetics/flux_loop/3"}
    });

    int rc = imasPartial(&interface);

    delete interface.request_data->nameValueList.nameValue;
    delete interface.request_data;
    delete interface.data_block;
    delete interface.userdefinedtypelist;
    delete interface.logmalloclist;

    REQUIRE( rc == 0 );
}

TEST_CASE( "get flux_loops", "[MAG]" )
{
    auto user = getenv("USER");

    IDAM_PLUGIN_INTERFACE interface = generate_plugin_interface("get", {
            {"shot", "1000"},
            {"run", "0"},
            {"user", user},
            {"tokamak", "test"},
            {"version", "3"},
            {"path", "/magnetics/flux_loop"}
    });

    int rc = imasPartial(&interface);

    delete interface.request_data->nameValueList.nameValue;
    delete interface.request_data;
    delete interface.data_block;
    delete interface.userdefinedtypelist;
    delete interface.logmalloclist;

    REQUIRE( rc == 0 );

    REQUIRE( interface.data_block->data_n == 1 );

    auto data_list = (DataList*)interface.data_block->data;

    REQUIRE( data_list->size == 341 );
    REQUIRE( data_list->list != nullptr );

    Data* data = &data_list->list[0];

    REQUIRE( std::string{data->name} == "magnetics/flux_loop" );
    REQUIRE( data->rank == 0 );
    REQUIRE( data->datatype == 51 );
    REQUIRE( ((int*)data->data)[0] == 10 );

    data = &data_list->list[1];

    REQUIRE( std::string{data->name} == "magnetics/flux_loop/1/flux/data" );
    REQUIRE( data->rank == 1 );
    REQUIRE( data->dims[0] == 1000 );
    REQUIRE( data->datatype == 52 );
    REQUIRE( ((double*)data->data)[0] == Approx(500.0) );
}

TEST_CASE( "get all magnetics", "[MAG]" )
{
    auto user = getenv("USER");

    IDAM_PLUGIN_INTERFACE interface = generate_plugin_interface("get", {
            {"shot", "1000"},
            {"run", "0"},
            {"user", user},
            {"tokamak", "test"},
            {"version", "3"},
            {"path", "/magnetics"}
    });

    int rc = 0;

//    interface = generate_plugin_interface("get", {{"path", "/magnetics"}});
//
//    rc = imasPartial(&interface);
//
//    delete interface.request_data->nameValueList.nameValue;
//    delete interface.request_data;
//    delete interface.data_block;
//    delete interface.userdefinedtypelist;
//    delete interface.logmalloclist;

    REQUIRE( rc == 0 );
}

TEST_CASE( "get flux data for a range of flux_loops", "[MAG]" )
{
    auto user = getenv("USER");

    IDAM_PLUGIN_INTERFACE interface = generate_plugin_interface("get", {
            {"shot", "1000"},
            {"run", "0"},
            {"user", user},
            {"tokamak", "test"},
            {"version", "3"},
            {"path", "/magnetics/flux_loop/3:5/flux"}
    });

    int rc = imasPartial(&interface);

    delete interface.request_data->nameValueList.nameValue;
    delete interface.request_data;
    delete interface.data_block;
    delete interface.userdefinedtypelist;
    delete interface.logmalloclist;

    REQUIRE( rc == 0 );

    REQUIRE( interface.data_block->data_n == 1 );

    auto data_list = (DataList*)interface.data_block->data;

    REQUIRE( data_list->size == 22 );
    REQUIRE( data_list->list != nullptr );

    Data* data = &data_list->list[0];

    REQUIRE( std::string{data->name} == "magnetics/flux_loop" );
    REQUIRE( data->rank == 0 );
    REQUIRE( data->datatype == 51 );
    REQUIRE( ((int*)data->data)[0] == 10 );

    data = &data_list->list[1];

    REQUIRE( std::string{data->name} == "magnetics/flux_loop/3/flux/data" );
    REQUIRE( data->rank == 1 );
    REQUIRE( data->dims[0] == 1000 );
    REQUIRE( data->datatype == 52 );
    REQUIRE( ((double*)data->data)[0] == Approx(500.0) );
}
