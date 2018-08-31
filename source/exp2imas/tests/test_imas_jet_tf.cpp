#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_helpers.h"

#include <c++/UDA.hpp>

#define QUOTE_(X) #X
#define QUOTE(X) QUOTE_(X)
#define SHOT_NUM "84600"

TEST_CASE( "Test tf coil count", "[IMAS][JET][TF]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='tf/coil/Shape_of', indices='', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");

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
    REQUIRE( val->as<int>() == 32 );
}

TEST_CASE( "Test tf coil conductor count", "[IMAS][JET][TF]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='tf/coil/#/conductor/Shape_of', indices='1', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");

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

TEST_CASE( "Test tf coil turns", "[IMAS][JET][TF]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='tf/coil/#/turns', indices='1', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");

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
    REQUIRE( val->as<int>() == 24 );
}

TEST_CASE( "Test tf vacuum field", "[IMAS][JET][TF]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='tf/b_field_tor_vacuum_r/data', indices='', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    std::vector<float> expected = { -0.0006830642f, -0.0003118947f, -0.0000411735f, -0.0001789987f, -0.000205223f };

    REQUIRE( arr->type().name() == typeid(float).name() );

    auto vals = arr->as<float>();
    vals.resize(5);

    REQUIRE( vals == ApproxVector(expected) );
}

TEST_CASE( "Test tf vacuum field error upper", "[IMAS][JET][TF]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='tf/b_field_tor_vacuum_r/data_error_upper', indices='', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    std::vector<float> expected = { 0.0093169361f, 0.0096881054f, 0.009958826f, 0.0098210014f, 0.0097947773f };

    REQUIRE( arr->type().name() == typeid(float).name() );

    auto vals = arr->as<float>();
    vals.resize(5);

    REQUIRE( vals == ApproxVector(expected) );
}

TEST_CASE( "Test tf vacuum field error lower", "[IMAS][JET][TF]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='tf/b_field_tor_vacuum_r/data_error_lower', indices='', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    std::vector<float> expected = { -0.0106830643f, -0.0103118951f, -0.0100411735f, -0.010178999f, -0.0102052232f };

    REQUIRE( arr->type().name() == typeid(float).name() );

    auto vals = arr->as<float>();
    vals.resize(5);

    REQUIRE( vals == ApproxVector(expected) );
}

TEST_CASE( "Test tf vacuum field time", "[IMAS][JET][TF]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='tf/b_field_tor_vacuum_r/time', indices='', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    std::vector<float> expected = { 25.0161991119f, 25.0314006805f, 25.0466003418f, 25.0618000031f, 25.0769996643f };

    REQUIRE( arr->type().name() == typeid(float).name() );

    auto vals = arr->as<float>();
    vals.resize(5);

    REQUIRE( vals == ApproxVector(expected) );
}
