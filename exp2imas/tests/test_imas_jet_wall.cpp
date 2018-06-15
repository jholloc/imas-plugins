#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_helpers.h"

#include <c++/UDA.hpp>

#define QUOTE_(X) #X
#define QUOTE(X) QUOTE_(X)
#define SHOT_NUM "84600"

/*
 * wall/description_2d/Shape_of
 */
TEST_CASE( "Test wall description count", "[IMAS][JET][TF]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='wall/description_2d/Shape_of', indices='', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(int).name() );

    auto val = dynamic_cast<uda::Scalar*>(data);

    REQUIRE( val != nullptr );
    REQUIRE( !val->isNull() );

    REQUIRE( val->type().name() == typeid(int).name() );
    REQUIRE( val->as<int>() == 1 );
}

/*
 * wall/description_2d/#/vessel/unit/#/annular/outline_inner/r
 */
TEST_CASE( "Test wall vessel unit annular outline", "[IMAS][JET][WALL]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='wall/description_2d/#/vessel/unit/#/annular/outline_inner/r', indices='1;1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    std::vector<float> expected = { 1.658, 1.6592, 1.6604, 1.6617, 1.6629 };

    REQUIRE( arr->type().name() == typeid(float).name() );

    auto vals = arr->as<float>();
    vals.resize(5);

    REQUIRE( vals == ApproxVector(expected) );
}
