#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <c++/UDA.hpp>

#include "test_helpers.h"

#define QUOTE_(X) #X
#define QUOTE(X) QUOTE_(X)
#define SHOT_NUM "51262"


TEST_CASE( "Test myFunction S1", "[TCVM][myFunction][S1]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("TCVM::read_s(element='S1', indices='', experiment='TCV', dtype=17, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(char*).name() );

    auto str = dynamic_cast<uda::String*>(data);

    REQUIRE( str != nullptr );
    REQUIRE( !str->isNull() );

    REQUIRE( str->str() == "TCV (regen)" );
}


TEST_CASE( "Test myFunction S2", "[TCVM][myFunction][S2]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("TCVM::read_s(element='S2', indices='', experiment='TCV', dtype=17, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(char*).name() );

    auto str = dynamic_cast<uda::String*>(data);

    REQUIRE( str != nullptr );
    REQUIRE( !str->isNull() );

    REQUIRE( str->str() == "AUG (cache)" );
}


TEST_CASE( "Test myFunction S3", "[TCVM][myFunction][S3]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("TCVM::read_s(element='S3', indices='', experiment='TCV', dtype=17, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(char*).name() );

    auto str = dynamic_cast<uda::String*>(data);

    REQUIRE( str != nullptr );
    REQUIRE( !str->isNull() );

    REQUIRE( str->str() == "JET (cache)" );
}


TEST_CASE( "Test myFunction2 tf/ids_properties/provider", "[TCVM][myFunction2][tf]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("TCVM::read(element='tf/ids_properties/provider', indices='', experiment='TCV', dtype=17, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(char*).name() );

    auto str = dynamic_cast<uda::String*>(data);

    REQUIRE( str != nullptr );
    REQUIRE( !str->isNull() );

    REQUIRE( str->str() == "tcv_get_ids_tf" );
}


TEST_CASE( "Test myFunction2 tf/coil/shape_of", "[TCVM][myFunction2][tf]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("TCVM::read(element='tf/coil/shape_of', indices='', experiment='TCV', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");

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
    REQUIRE( val->as<int>() == 0 );
}


TEST_CASE( "Test myFunction2 tf/r0", "[TCVM][myFunction2][tf]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("TCVM::read(element='tf/r0', indices='', experiment='TCV', dtype=6, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto val = dynamic_cast<uda::Scalar*>(data);

    REQUIRE( val != nullptr );
    REQUIRE( !val->isNull() );

    REQUIRE( val->type().name() == typeid(float).name() );
    REQUIRE( val->as<float>() == Approx(0.88) );
}


TEST_CASE( "Test myFunction2 tf/b_field_tor_vacuum_r/data", "[TCVM][myFunction2][tf]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("TCVM::read(element='tf/b_field_tor_vacuum_r/data', indices='', experiment='TCV', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(double).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    std::vector<double> expected = { -6.386791972212357e-04, -5.109433743563565e-04, -6.386791972212357e-04, -5.109433743563565e-04, -5.109433743563565e-04 };

    REQUIRE( arr->type().name() == typeid(double).name() );

    auto vals = arr->as<double>();
    vals.resize(5);

    REQUIRE( vals == ApproxVector(expected) );
}


TEST_CASE( "Test myFunction2 tf/b_field_tor_vacuum_r/time", "[TCVM][myFunction2][tf]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("TCVM::read(element='tf/b_field_tor_vacuum_r/time', indices='', experiment='TCV', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(double).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    std::vector<double> expected = { -1.357999920845032e+00, -1.357899904251099e+00, -1.357799887657166e+00, -1.357699871063232e+00, -1.357599973678589e+00 };

    REQUIRE( arr->type().name() == typeid(double).name() );

    auto vals = arr->as<double>();
    vals.resize(5);

    REQUIRE( vals == ApproxVector(expected) );

    client.get("TCVM::reset()", "");
}
