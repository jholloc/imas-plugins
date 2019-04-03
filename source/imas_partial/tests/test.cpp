#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <string>
#include <vector>

#include <clientserver/initStructs.h>
#include <clientserver/udaTypes.h>
#include <structures/struct.h>
#include <client/udaGetAPI.h>
#include <c++/UDA.hpp>

#include "test_helpers.h"

TEST_CASE( "get flux_loop flux data", "[MAG]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    auto user = getenv("USER");
    std::string request = std::string("IMAS_PARTIAL::get(shot=1000, run=0, user=") + user
            + ", tokamak=test, version=3, path=/magnetics/flux_loop/3/flux/data)";

    const uda::Result& result = client.get(request, "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );
    REQUIRE( result.isTree() );

    uda::TreeNode tree = result.tree();

    REQUIRE( tree.numChildren() == 1 );

    uda::TreeNode child = tree.child(0);

    REQUIRE( child.name() == "data" );
    REQUIRE( child.numChildren() == 2 );
    REQUIRE( child.atomicCount() == 1 );
    REQUIRE( child.atomicNames()[0] == "size" );
    REQUIRE( child.atomicPointers()[0] == false );
    REQUIRE( child.atomicTypes()[0] == "int" );
    REQUIRE( child.atomicRank()[0] == 0 );

    uda::Scalar size = child.atomicScalar("size");
    REQUIRE( !size.isNull() );

    REQUIRE( size.type().name() == typeid(int).name() );
    REQUIRE( size.as<int>() == 2 );

    {
        int i = 0;
        uda::TreeNode list = child.child(i);

        REQUIRE( list.name() == "list" );
        REQUIRE( list.numChildren() == 0 );
        REQUIRE( list.atomicCount() == 5 );

        std::vector<std::string> exp_names = { "name", "data", "rank", "dims", "datatype" };
        REQUIRE( list.atomicNames() == exp_names );

        std::vector<bool> exp_pointers = { true, true, false, false, false };
        REQUIRE( list.atomicPointers() == exp_pointers );

        std::vector<std::string> exp_types = { "STRING", "unsigned char *", "int", "int", "int" };
        REQUIRE( list.atomicTypes() == exp_types );

        std::vector<size_t> exp_ranks = { 0, 0, 0, 1, 0 };
        REQUIRE( list.atomicRank() == exp_ranks );

        uda::Scalar name = list.atomicScalar("name");
        REQUIRE( !name.isNull() );
        REQUIRE( std::string{ name.as<char*>() } == "magnetics/flux_loop" );

        uda::Vector data = list.atomicVector("data");
        REQUIRE( !data.isNull() );
        REQUIRE( data.size() == sizeof(int) );
        REQUIRE( data.type().name() == typeid(unsigned char).name() );
        std::vector<unsigned char> bytes = data.as<unsigned char>();
        auto id = reinterpret_cast<int*>(bytes.data());
        REQUIRE( *id == 10 );

        uda::Scalar rank = list.atomicScalar("rank");
        REQUIRE( !rank.isNull() );
        REQUIRE( rank.as<int>() == 0 );

        uda::Vector dims = list.atomicVector("dims");
        REQUIRE( !dims.isNull() );
        std::vector<int> exp_dims = { 0 };
        exp_dims.resize(64, 0);
        REQUIRE( dims.as<int>() == exp_dims );

        uda::Scalar datatype = list.atomicScalar("datatype");
        REQUIRE( !datatype.isNull() );
        REQUIRE( datatype.as<int>() == 51 );
    }

    {
        int i = 1;
        uda::TreeNode list = child.child(i);

        REQUIRE( list.name() == "list" );
        REQUIRE( list.numChildren() == 0 );
        REQUIRE( list.atomicCount() == 5 );

        std::vector<std::string> exp_names = { "name", "data", "rank", "dims", "datatype" };
        REQUIRE( list.atomicNames() == exp_names );

        std::vector<bool> exp_pointers = { true, true, false, false, false };
        REQUIRE( list.atomicPointers() == exp_pointers );

        std::vector<std::string> exp_types = { "STRING", "unsigned char *", "int", "int", "int" };
        REQUIRE( list.atomicTypes() == exp_types );

        std::vector<size_t> exp_ranks = { 0, 0, 0, 1, 0 };
        REQUIRE( list.atomicRank() == exp_ranks );

        uda::Scalar name = list.atomicScalar("name");
        REQUIRE( !name.isNull() );
        REQUIRE( std::string{ name.as<char*>() } == "magnetics/flux_loop/3/flux/data" );

        uda::Vector data = list.atomicVector("data");
        REQUIRE( !data.isNull() );
        REQUIRE( data.size() == 1000 * sizeof(double) );
        REQUIRE( data.type().name() == typeid(unsigned char).name() );
        std::vector<unsigned char> bytes = data.as<unsigned char>();
        auto dd = reinterpret_cast<double*>(bytes.data());
        std::vector<double> exp_data = { 500, 500.5, 501, 501.5, 502 };
        REQUIRE( std::vector<double>{ &dd[0], &dd[5] } == ApproxVector(exp_data) );

        uda::Scalar rank = list.atomicScalar("rank");
        REQUIRE( !rank.isNull() );
        REQUIRE( rank.as<int>() == 1 );

        uda::Vector dims = list.atomicVector("dims");
        REQUIRE( !dims.isNull() );
        std::vector<int> exp_dims = { 1000 };
        exp_dims.resize(64, 0);
        REQUIRE( dims.as<int>() == exp_dims );

        uda::Scalar datatype = list.atomicScalar("datatype");
        REQUIRE( !datatype.isNull() );
        REQUIRE( datatype.as<int>() == 52 );
    }
}

//TEST_CASE( "get flux_loop flux", "[MAG]" )
//{
//    IDAM_PLUGIN_INTERFACE interface = generate_plugin_interface("get", {
//            {"shot", "1000"},
//            {"run", "0"},
//            {"user", "jhollocombe"},
//            {"tokamak", "test"},
//            {"version", "3"},
//            {"path", "/magnetics/flux_loop/3/flux"}
//    });
//
//    int rc = imasPartial(&interface);
//
//    delete interface.request_block->nameValueList.nameValue;
//    delete interface.request_block;
//    delete interface.data_block;
//    delete interface.userdefinedtypelist;
//    delete interface.logmalloclist;
//
//    REQUIRE( rc == 0 );
//}
//
//TEST_CASE( "get flux_loop", "[MAG]" )
//{
//    IDAM_PLUGIN_INTERFACE interface = generate_plugin_interface("get", {
//            {"shot", "1000"},
//            {"run", "0"},
//            {"user", "jhollocombe"},
//            {"tokamak", "test"},
//            {"version", "3"},
//            {"path", "/magnetics/flux_loop/3"}
//    });
//
//    int rc = imasPartial(&interface);
//
//    delete interface.request_block->nameValueList.nameValue;
//    delete interface.request_block;
//    delete interface.data_block;
//    delete interface.userdefinedtypelist;
//    delete interface.logmalloclist;
//
//    REQUIRE( rc == 0 );
//}
//
//TEST_CASE( "get flux_loops", "[MAG]" )
//{
//    IDAM_PLUGIN_INTERFACE interface = generate_plugin_interface("get", {
//            {"shot", "1000"},
//            {"run", "0"},
//            {"user", "jhollocombe"},
//            {"tokamak", "test"},
//            {"version", "3"},
//            {"path", "/magnetics/flux_loop"}
//    });
//
//    int rc = imasPartial(&interface);
//
//    delete interface.request_block->nameValueList.nameValue;
//    delete interface.request_block;
//    delete interface.data_block;
//    delete interface.userdefinedtypelist;
//    delete interface.logmalloclist;
//
//    REQUIRE( rc == 0 );
//
//    REQUIRE( interface.data_block->data_n == 1 );
//
//    auto data_list = (DataList*)interface.data_block->data;
//
//    REQUIRE( data_list->size == 210 );
//    REQUIRE( data_list->list != nullptr );
//
//    Data* data = &data_list->list[14];
//
//    REQUIRE( std::string{data->name} == "magnetics/flux_loop/1/flux/data" );
//    REQUIRE( data->rank == 1 );
//    REQUIRE( data->dims[0] == 1000 );
//    REQUIRE( data->datatype == 52 );
//    REQUIRE( ((double*)data->data)[0] == Approx(-10) );
//}
//
//TEST_CASE( "get all magnetics", "[MAG]" )
//{
//    IDAM_PLUGIN_INTERFACE interface = generate_plugin_interface("get", {
//            {"shot", "1000"},
//            {"run", "0"},
//            {"user", "jhollocombe"},
//            {"tokamak", "test"},
//            {"version", "3"},
//            {"path", "/magnetics"}
//    });
//
//    int rc = 0;
//
////    interface = generate_plugin_interface("get", {{"path", "/magnetics"}});
////
////    rc = imasPartial(&interface);
////
////    delete interface.request_block->nameValueList.nameValue;
////    delete interface.request_block;
////    delete interface.data_block;
////    delete interface.userdefinedtypelist;
////    delete interface.logmalloclist;
//
//    REQUIRE( rc == 0 );
//}
