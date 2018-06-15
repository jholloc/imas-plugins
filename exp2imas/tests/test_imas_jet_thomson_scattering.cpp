#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_helpers.h"

#include <c++/UDA.hpp>

#define QUOTE_(X) #X
#define QUOTE(X) QUOTE_(X)
#define SHOT_NUM "84600"

TEST_CASE( "Test thomson scattering channel count", "[IMAS][JET][TSCAT]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='thomson_scattering/channel/Shape_of', indices='', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");

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
    REQUIRE( val->as<int>() == 63 );
}

TEST_CASE( "Test thomson scattering channel position r", "[IMAS][JET][TSCAT]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    {
        const uda::Result& result = client.get("EXP2IMAS::read(element='thomson_scattering/channel/#/position/r', indices='1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

        REQUIRE( result.errorCode() == 0 );
        REQUIRE( result.errorMessage().empty() );

        uda::Data* data = result.data();

        REQUIRE( data != nullptr );
        REQUIRE( !data->isNull() );
        REQUIRE( data->type().name() == typeid(float).name() );

        auto arr = dynamic_cast<uda::Array*>(data);

        REQUIRE( arr != nullptr );
        REQUIRE( !arr->isNull() );

        REQUIRE( arr->type().name() == typeid(float).name() );
        REQUIRE( arr->size() == 701 );
        REQUIRE( arr->as<float>()[0] == Approx(2.97450018) );
    }

    {
        const uda::Result& result = client.get("EXP2IMAS::read(element='thomson_scattering/channel/#/position/r', indices='2', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

        REQUIRE( result.errorCode() == 0 );
        REQUIRE( result.errorMessage().empty() );

        uda::Data* data = result.data();

        REQUIRE( data != nullptr );
        REQUIRE( !data->isNull() );
        REQUIRE( data->type().name() == typeid(float).name() );

        auto arr = dynamic_cast<uda::Array*>(data);

        REQUIRE( arr != nullptr );
        REQUIRE( !arr->isNull() );

        REQUIRE( arr->type().name() == typeid(float).name() );
        REQUIRE( arr->size() == 701 );
        REQUIRE( arr->as<float>()[0] == Approx(2.99075007) );
    }
}

TEST_CASE( "Test thomson scattering channel position z", "[IMAS][JET][TSCAT]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    {
        // required to set some state in the MDSplus server!
        client.get("EXP2IMAS::read(element='thomson_scattering/channel/#/position/r', indices='1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

        const uda::Result& result = client.get("EXP2IMAS::read(element='thomson_scattering/channel/#/position/z', indices='1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

        REQUIRE( result.errorCode() == 0 );
        REQUIRE( result.errorMessage().empty() );

        uda::Data* data = result.data();

        REQUIRE( data != nullptr );
        REQUIRE( !data->isNull() );
        REQUIRE( data->type().name() == typeid(float).name() );

        auto arr = dynamic_cast<uda::Array*>(data);

        REQUIRE( arr != nullptr );
        REQUIRE( !arr->isNull() );

        REQUIRE( arr->type().name() == typeid(float).name() );
        REQUIRE( arr->size() == 701 );
        REQUIRE( arr->as<float>()[0] == Approx(0.061622526) );
    }

    {
        // required to set some state in the MDSplus server!
        client.get("EXP2IMAS::read(element='thomson_scattering/channel/#/position/r', indices='2', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

        const uda::Result& result = client.get("EXP2IMAS::read(element='thomson_scattering/channel/#/position/z', indices='2', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

        REQUIRE( result.errorCode() == 0 );
        REQUIRE( result.errorMessage().empty() );

        uda::Data* data = result.data();

        REQUIRE( data != nullptr );
        REQUIRE( !data->isNull() );
        REQUIRE( data->type().name() == typeid(float).name() );

        auto arr = dynamic_cast<uda::Array*>(data);

        REQUIRE( arr != nullptr );
        REQUIRE( !arr->isNull() );

        REQUIRE( arr->type().name() == typeid(float).name() );
        REQUIRE( arr->size() == 701 );
        REQUIRE( arr->as<float>()[0] == Approx(0.062425219) );
    }
}

TEST_CASE( "Test thomson scattering channel position phi", "[IMAS][JET][TSCAT]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    {
        const uda::Result& result = client.get("EXP2IMAS::read(element='thomson_scattering/channel/#/position/phi', indices='1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

        REQUIRE( result.errorCode() == 0 );
        REQUIRE( result.errorMessage().empty() );

        uda::Data* data = result.data();

        REQUIRE( data != nullptr );
        REQUIRE( !data->isNull() );
        REQUIRE( data->type().name() == typeid(float).name() );

        auto arr = dynamic_cast<uda::Array*>(data);

        REQUIRE( arr != nullptr );
        REQUIRE( !arr->isNull() );

        REQUIRE( arr->type().name() == typeid(float).name() );
        REQUIRE( arr->size() == 701 );
        REQUIRE( arr->as<float>()[0] == Approx(0.0) );
    }

    {
        const uda::Result& result = client.get("EXP2IMAS::read(element='thomson_scattering/channel/#/position/phi', indices='2', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

        REQUIRE( result.errorCode() == 0 );
        REQUIRE( result.errorMessage().empty() );

        uda::Data* data = result.data();

        REQUIRE( data != nullptr );
        REQUIRE( !data->isNull() );
        REQUIRE( data->type().name() == typeid(float).name() );

        auto arr = dynamic_cast<uda::Array*>(data);

        REQUIRE( arr != nullptr );
        REQUIRE( !arr->isNull() );

        REQUIRE( arr->type().name() == typeid(float).name() );
        REQUIRE( arr->size() == 701 );
        REQUIRE( arr->as<float>()[0] == Approx(0.0) );
    }
}

TEST_CASE( "Test thomson scattering channel t_e data", "[IMAS][JET][TSCAT]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='thomson_scattering/channel/#/t_e/data', indices='1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    std::vector<float> expected = { 0.0f, 0.0f, 0.0f, 0.0f, 12.56754398f, 491.86361694f, 300.91174316f };

    REQUIRE( arr->type().name() == typeid(float).name() );

    auto vals = arr->as<float>();
    vals.resize(7);

    REQUIRE( vals == ApproxVector(expected) );
}

TEST_CASE( "Test thomson scattering channel t_e time", "[IMAS][JET][TSCAT]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='thomson_scattering/channel/#/t_e/time', indices='1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    std::vector<float> expected = { 40.016254425f, 40.066280365f, 40.1163063049f, 40.1663322449f, 40.2163581848f };

    REQUIRE( arr->type().name() == typeid(float).name() );

    auto vals = arr->as<float>();
    vals.resize(5);

    REQUIRE( vals == ApproxVector(expected) );
}

TEST_CASE( "Test thomson scattering channel n_e data", "[IMAS][JET][TSCAT]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='thomson_scattering/channel/#/n_e/data', indices='1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    std::vector<float> expected = { 0.0f, 0.0f, 0.0f, 0.0f, 15769506927354052608.0f, 3531266860311904256.0f, 4806206118055378944.0f };

    REQUIRE( arr->type().name() == typeid(float).name() );

    auto vals = arr->as<float>();
    vals.resize(7);

    REQUIRE( vals == ApproxVector(expected) );
}

TEST_CASE( "Test thomson scattering channel n_e time", "[IMAS][JET][TSCAT]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='thomson_scattering/channel/#/n_e/time', indices='1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    std::vector<float> expected = { 40.016254425f, 40.066280365f, 40.1163063049f, 40.1663322449f, 40.2163581848f };

    REQUIRE( arr->type().name() == typeid(float).name() );

    auto vals = arr->as<float>();
    vals.resize(5);

    REQUIRE( vals == ApproxVector(expected) );
}
