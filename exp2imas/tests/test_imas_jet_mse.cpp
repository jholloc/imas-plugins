#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_helpers.h"

#include <c++/UDA.hpp>

#define QUOTE_(X) #X
#define QUOTE(X) QUOTE_(X)
#define SHOT_NUM "89140"

//TEST_CASE( "Test tf coil count", "[IMAS][JET][TF]" )
//{
//#ifdef FATCLIENT
//#  include "setup.inc"
//#endif
//
//    uda::Client client;
//
//    const uda::Result& result = client.get("EXP2IMAS::read(group='tf', variable='coil/Shape_of', indices='', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");
//
//    REQUIRE( result.errorCode() == 0 );
//    REQUIRE( result.errorMessage().empty() );
//
//    uda::Data* data = result.data();
//
//    REQUIRE( data != nullptr );
//    REQUIRE( !data->isNull() );
//    REQUIRE( data->type().name() == typeid(int).name() );
//
//    auto val = dynamic_cast<uda::Scalar*>(data);
//
//    REQUIRE( val != nullptr );
//    REQUIRE( !val->isNull() );
//
//    REQUIRE( val->type().name() == typeid(int).name() );
//    REQUIRE( val->as<int>() == 32 );
//}
//
//TEST_CASE( "Test tf coil conductor count", "[IMAS][JET][TF]" )
//{
//#ifdef FATCLIENT
//#  include "setup.inc"
//#endif
//
//    uda::Client client;
//
//    const uda::Result& result = client.get("EXP2IMAS::read(group='tf', variable='coil/1/conductor/Shape_of', indices='', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");
//
//    REQUIRE( result.errorCode() == 0 );
//    REQUIRE( result.errorMessage().empty() );
//
//    uda::Data* data = result.data();
//
//    REQUIRE( data != nullptr );
//    REQUIRE( !data->isNull() );
//    REQUIRE( data->type().name() == typeid(int).name() );
//
//    auto val = dynamic_cast<uda::Scalar*>(data);
//
//    REQUIRE( val != nullptr );
//    REQUIRE( !val->isNull() );
//
//    REQUIRE( val->type().name() == typeid(int).name() );
//    REQUIRE( val->as<int>() == 0 );
//}
//
//TEST_CASE( "Test tf coil turns", "[IMAS][JET][TF]" )
//{
//#ifdef FATCLIENT
//#  include "setup.inc"
//#endif
//
//    uda::Client client;
//
//    const uda::Result& result = client.get("EXP2IMAS::read(group='tf', variable='coil/1/turns', indices='', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");
//
//    REQUIRE( result.errorCode() == 0 );
//    REQUIRE( result.errorMessage().empty() );
//
//    uda::Data* data = result.data();
//
//    REQUIRE( data != nullptr );
//    REQUIRE( !data->isNull() );
//    REQUIRE( data->type().name() == typeid(int).name() );
//
//    auto val = dynamic_cast<uda::Scalar*>(data);
//
//    REQUIRE( val != nullptr );
//    REQUIRE( !val->isNull() );
//
//    REQUIRE( val->type().name() == typeid(int).name() );
//    REQUIRE( val->as<int>() == 24 );
//}
//
//TEST_CASE( "Test tf vacuum field", "[IMAS][JET][TF]" )
//{
//#ifdef FATCLIENT
//#  include "setup.inc"
//#endif
//
//    uda::Client client;
//
//    const uda::Result& result = client.get("EXP2IMAS::read(group='tf', variable='b_field_tor_vacuum_r/data', indices='', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");
//
//    REQUIRE( result.errorCode() == 0 );
//    REQUIRE( result.errorMessage().empty() );
//
//    uda::Data* data = result.data();
//
//    REQUIRE( data != nullptr );
//    REQUIRE( !data->isNull() );
//    REQUIRE( data->type().name() == typeid(double).name() );
//
//    auto arr = dynamic_cast<uda::Array*>(data);
//
//    REQUIRE( arr != nullptr );
//    REQUIRE( !arr->isNull() );
//
//    std::vector<double> expected = { -0.0006830642, -0.0003118947, -0.0000411735, -0.0001789987, -0.000205223 };
//
//    REQUIRE( arr->type().name() == typeid(double).name() );
//
//    auto vals = arr->as<double>();
//    vals.resize(5);
//
//    REQUIRE( vals == ApproxVector(expected) );
//}
//
//TEST_CASE( "Test tf vacuum field error upper", "[IMAS][JET][TF]" )
//{
//#ifdef FATCLIENT
//#  include "setup.inc"
//#endif
//
//    uda::Client client;
//
//    const uda::Result& result = client.get("EXP2IMAS::read(group='tf', variable='b_field_tor_vacuum_r/data_error_upper', indices='', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");
//
//    REQUIRE( result.errorCode() == 0 );
//    REQUIRE( result.errorMessage().empty() );
//
//    uda::Data* data = result.data();
//
//    REQUIRE( data != nullptr );
//    REQUIRE( !data->isNull() );
//    REQUIRE( data->type().name() == typeid(double).name() );
//
//    auto arr = dynamic_cast<uda::Array*>(data);
//
//    REQUIRE( arr != nullptr );
//    REQUIRE( !arr->isNull() );
//
//    std::vector<double> expected = { 0.0093169361, 0.0096881054, 0.009958826, 0.0098210014, 0.0097947773 };
//
//    REQUIRE( arr->type().name() == typeid(double).name() );
//
//    auto vals = arr->as<double>();
//    vals.resize(5);
//
//    REQUIRE( vals == ApproxVector(expected) );
//}
//
//TEST_CASE( "Test tf vacuum field error lower", "[IMAS][JET][TF]" )
//{
//#ifdef FATCLIENT
//#  include "setup.inc"
//#endif
//
//    uda::Client client;
//
//    const uda::Result& result = client.get("EXP2IMAS::read(group='tf', variable='b_field_tor_vacuum_r/data_error_lower', indices='', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");
//
//    REQUIRE( result.errorCode() == 0 );
//    REQUIRE( result.errorMessage().empty() );
//
//    uda::Data* data = result.data();
//
//    REQUIRE( data != nullptr );
//    REQUIRE( !data->isNull() );
//    REQUIRE( data->type().name() == typeid(double).name() );
//
//    auto arr = dynamic_cast<uda::Array*>(data);
//
//    REQUIRE( arr != nullptr );
//    REQUIRE( !arr->isNull() );
//
//    std::vector<double> expected = { -0.0106830643, -0.0103118951, -0.0100411735, -0.010178999, -0.0102052232 };
//
//    REQUIRE( arr->type().name() == typeid(double).name() );
//
//    auto vals = arr->as<double>();
//    vals.resize(5);
//
//    REQUIRE( vals == ApproxVector(expected) );
//}
//
//TEST_CASE( "Test tf vacuum field time", "[IMAS][JET][TF]" )
//{
//#ifdef FATCLIENT
//#  include "setup.inc"
//#endif
//
//    uda::Client client;
//
//    const uda::Result& result = client.get("EXP2IMAS::read(group='tf', variable='b_field_tor_vacuum_r/time', indices='', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");
//
//    REQUIRE( result.errorCode() == 0 );
//    REQUIRE( result.errorMessage().empty() );
//
//    uda::Data* data = result.data();
//
//    REQUIRE( data != nullptr );
//    REQUIRE( !data->isNull() );
//    REQUIRE( data->type().name() == typeid(double).name() );
//
//    auto arr = dynamic_cast<uda::Array*>(data);
//
//    REQUIRE( arr != nullptr );
//    REQUIRE( !arr->isNull() );
//
//    std::vector<double> expected = { 25.0161991119, 25.0314006805, 25.0466003418, 25.0618000031, 25.0769996643 };
//
//    REQUIRE( arr->type().name() == typeid(double).name() );
//
//    auto vals = arr->as<double>();
//    vals.resize(5);
//
//    REQUIRE( vals == ApproxVector(expected) );
//}

TEST_CASE( "Test mse active_spatial_resolution count", "[IMAS][JET][MSE]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='mse/channel/#/active_spatial_resolution/Shape_of', indices='1', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");

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

TEST_CASE( "Test mse active_spatial_resolution centre r", "[IMAS][JET][MSE]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    {
        const uda::Result& result = client.get(
                "EXP2IMAS::read(element='mse/channel/#/active_spatial_resolution/#/centre/r', indices='1;1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')",
                "");

        REQUIRE(result.errorCode() == 0);
        REQUIRE(result.errorMessage().empty());

        uda::Data* data = result.data();

        REQUIRE(data != nullptr);
        REQUIRE(!data->isNull());
        REQUIRE(data->type().name() == typeid(float).name());

        auto val = dynamic_cast<uda::Scalar*>(data);

        REQUIRE(val != nullptr);
        REQUIRE(!val->isNull());

        REQUIRE(val->type().name() == typeid(float).name());
        REQUIRE(val->as<float>() == Approx(3.88219f));
    }

    {
        const uda::Result& result = client.get(
                "EXP2IMAS::read(element='mse/channel/#/active_spatial_resolution/#/centre/r', indices='7;1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')",
                "");

        REQUIRE(result.errorCode() == 0);
        REQUIRE(result.errorMessage().empty());

        uda::Data* data = result.data();

        REQUIRE(data != nullptr);
        REQUIRE(!data->isNull());
        REQUIRE(data->type().name() == typeid(float).name());

        auto val = dynamic_cast<uda::Scalar*>(data);

        REQUIRE(val != nullptr);
        REQUIRE(!val->isNull());

        REQUIRE(val->type().name() == typeid(float).name());
        REQUIRE(val->as<float>() == Approx(3.59839f));
    }
}

TEST_CASE( "Test mse active_spatial_resolution centre z", "[IMAS][JET][MSE]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    {
        const uda::Result& result = client.get(
                "EXP2IMAS::read(element='mse/channel/#/active_spatial_resolution/#/centre/z', indices='1;1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')",
                "");

        REQUIRE(result.errorCode() == 0);
        REQUIRE(result.errorMessage().empty());

        uda::Data* data = result.data();

        REQUIRE(data != nullptr);
        REQUIRE(!data->isNull());
        REQUIRE(data->type().name() == typeid(float).name());

        auto val = dynamic_cast<uda::Scalar*>(data);

        REQUIRE(val != nullptr);
        REQUIRE(!val->isNull());

        REQUIRE(val->type().name() == typeid(float).name());
        REQUIRE(val->as<float>() == Approx(0.24008f));
    }

    {
        const uda::Result& result = client.get(
                "EXP2IMAS::read(element='mse/channel/#/active_spatial_resolution/#/centre/z', indices='7;1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')",
                "");

        REQUIRE(result.errorCode() == 0);
        REQUIRE(result.errorMessage().empty());

        uda::Data* data = result.data();

        REQUIRE(data != nullptr);
        REQUIRE(!data->isNull());
        REQUIRE(data->type().name() == typeid(float).name());

        auto val = dynamic_cast<uda::Scalar*>(data);

        REQUIRE(val != nullptr);
        REQUIRE(!val->isNull());

        REQUIRE(val->type().name() == typeid(float).name());
        REQUIRE(val->as<float>() == Approx(0.23235f));
    }
}

TEST_CASE( "Test mse active_spatial_resolution centre phi", "[IMAS][JET][MSE]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='mse/channel/#/active_spatial_resolution/#/centre/phi', indices='1;1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( data->isNull() );
}

TEST_CASE( "Test mse active_spatial_resolution geometric_coefficients", "[IMAS][JET][MSE]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    {
        const uda::Result& result = client.get(
                "EXP2IMAS::read(element='mse/channel/#/active_spatial_resolution/#/geometric_coefficients', indices='1;1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')",
                "");

        REQUIRE(result.errorCode() == 0);
        REQUIRE(result.errorMessage().empty());

        uda::Data* data = result.data();

        REQUIRE(data != nullptr);
        REQUIRE(!data->isNull());
        REQUIRE(data->type().name() == typeid(float).name());

        auto arr = dynamic_cast<uda::Array*>(data);

        REQUIRE(arr != nullptr);
        REQUIRE(!arr->isNull());

        std::vector<float> expected{ -0.26617f, -0.0051f, 0.02266f, 0.0f, -0.01411f, 0.47638f, -0.87878f, 0.0f, 0.0f };

        REQUIRE(arr->size() == 9);
        REQUIRE(arr->type().name() == typeid(float).name());

        auto vals = arr->as<float>();

        REQUIRE(vals == ApproxVector(expected));
    }

    {
        const uda::Result& result = client.get(
                "EXP2IMAS::read(element='mse/channel/#/active_spatial_resolution/#/geometric_coefficients', indices='25;1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')",
                "");

        REQUIRE(result.errorCode() == 0);
        REQUIRE(result.errorMessage().empty());

        uda::Data* data = result.data();

        REQUIRE(data != nullptr);
        REQUIRE(!data->isNull());
        REQUIRE(data->type().name() == typeid(float).name());

        auto arr = dynamic_cast<uda::Array*>(data);

        REQUIRE(arr != nullptr);
        REQUIRE(!arr->isNull());

        std::vector<float> expected{ -0.62394f, -0.00142f, 0.02303f, 0.0f, -0.01669f, 0.68639f, -0.72665f, 0.0f, 0.0f };

        REQUIRE(arr->size() == 9);
        REQUIRE(arr->type().name() == typeid(float).name());

        auto vals = arr->as<float>();

        REQUIRE(vals == ApproxVector(expected));
    }
}