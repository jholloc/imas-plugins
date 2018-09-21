#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <c++/UDA.hpp>

#define QUOTE_(X) #X
#define QUOTE(X) QUOTE_(X)
#define SHOT_NUM "84600"

/*
  ✓ bpol_probe/Shape_of
  ✓ bpol_probe/#/name
  ✓ bpol_probe/#/identifier
  ✓ bpol_probe/#/position/r
  ✓ bpol_probe/#/position/z
  ✓ bpol_probe/#/position/phi
  ✓ bpol_probe/#/poloidal_angle
  ✓ bpol_probe/#/toroidal_angle
  ✓ bpol_probe/#/area
  ✓ bpol_probe/#/length
  ✓ bpol_probe/#/turns
  ✓ bpol_probe/#/field
  ✓ bpol_probe/#/field/time
 */

/*
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/Shape_of', dtype=3, shot=84600, )
 */
TEST_CASE( "Test bpol_probe count", "[IMAS][JET][BPOL]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='magnetics/bpol_probe/Shape_of', indices='', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");

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
    REQUIRE( val->as<int>() == 71 );
}

/*
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/name', dtype=17, shot=84600, )
 */
TEST_CASE( "Test bpol_probe name", "[IMAS][JET][BPOL]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='magnetics/bpol_probe/#/name', indices='1', experiment='JET', dtype=17, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(char*).name() );

    auto val = dynamic_cast<uda::String*>(data);

    REQUIRE( val != nullptr );
    REQUIRE( !val->isNull() );

    REQUIRE( val->str() == "C2-CX01" );
}

/*
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/identifier', dtype=17, shot=84600, )
 */
TEST_CASE( "Test bpol_probe identifier", "[IMAS][JET][BPOL]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='magnetics/bpol_probe/#/identifier', indices='1', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(int).name() );

    auto val = dynamic_cast<uda::Scalar*>(data);

    REQUIRE( val != nullptr );
    REQUIRE( !val->isNull() );

    REQUIRE( val->as<int>() == 1 );
}

/*
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/toroidal_angle_error_index', dtype=3, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/toroidal_angle_error_lower', dtype=7, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/toroidal_angle_error_upper', dtype=7, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/toroidal_angle', dtype=7, shot=84600, )
 */
TEST_CASE( "Test bpol_probe toroidal_angle", "[IMAS][JET][BPOL]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

	uda::Client client;

	const uda::Result& result = client.get("EXP2IMAS::read(element='magnetics/bpol_probe/#/toroidal_angle', indices='1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

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
	REQUIRE( val->as<float>() == Approx(0.0) );
}

/*
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/poloidal_angle_error_index', dtype=3, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/poloidal_angle_error_lower', dtype=7, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/poloidal_angle_error_upper', dtype=7, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/poloidal_angle', dtype=7, shot=84600, )
 */
TEST_CASE( "Test bpol_probe poloidal_angle", "[IMAS][JET][BPOL]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    double expected_vals[] = { -1.2933, -1.0559, -0.8203, -0.5847, -0.2304 };

    int index = 1;
    for (auto expected_val : expected_vals) {

        std::string signal = "EXP2IMAS::read(element='magnetics/bpol_probe/#/poloidal_angle', indices='"
                             + std::to_string(index) + "', experiment='JET', dtype=7, shot=" + SHOT_NUM + ", IDS_version='')";

        const uda::Result& result = client.get(signal, "");
        ++index;

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
        REQUIRE(val->as<float>() == Approx(expected_val));
    }
}

/*
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/area_error_index', dtype=3, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/area_error_lower', dtype=7, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/area_error_upper', dtype=7, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/area', dtype=7, shot=84600, )
 */
TEST_CASE( "Test bpol_probe area", "[IMAS][JET][BPOL]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='magnetics/bpol_probe/#/area', indices='1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

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
    REQUIRE( val->as<float>() == Approx(0.001) );
}

/*
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/position/r_error_index', dtype=3, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/position/r_error_lower', dtype=7, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/position/r_error_upper', dtype=7, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/position/r', dtype=7, shot=84600, )
 */
TEST_CASE( "Test bpol_probe position r", "[IMAS][JET][BPOL]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='magnetics/bpol_probe/#/position/r', indices='1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

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
    REQUIRE( val->as<float>() == Approx(4.2920) );
}

/*
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/position/z_error_index', dtype=3, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/position/z_error_lower', dtype=7, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/position/z_error_upper', dtype=7, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/position/z', dtype=7, shot=84600, )
 */
TEST_CASE( "Test bpol_probe position z", "[IMAS][JET][BPOL]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='magnetics/bpol_probe/#/position/z', indices='1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

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
    REQUIRE( val->as<float>() == Approx(0.6040) );
}

/*
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/position/phi_error_index', dtype=3, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/position/phi_error_lower', dtype=7, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/position/phi_error_upper', dtype=7, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/position/phi', dtype=7, shot=84600, )
 */
//TEST_CASE( "Test bpol_probe position phi", "[IMAS][JET][BPOL]" )
//{
//#ifdef FATCLIENT
//#  include "setup.inc"
//#endif
//
//    uda::Client client;
//
//    const uda::Result& result = client.get("EXP2IMAS::read(element='magnetics/bpol_probe/#/position/phi', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");
//
//    REQUIRE( result.errorCode() == 0 );
//    REQUIRE( result.errorMessage().empty() );
//
//    uda::Data* data = result.data();
//
//    REQUIRE( data != nullptr );
//    REQUIRE( !data->isNull() );
//    REQUIRE( data->type().name() == typeid(float).name() );
//
//    auto val = dynamic_cast<uda::Scalar*>(data);
//
//    REQUIRE( val != nullptr );
//    REQUIRE( !val->isNull() );
//
//    REQUIRE( val->type().name() == typeid(float).name() );
//    REQUIRE( val->as<float>() == Approx(0.0) );
//}

/*
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/length_error_index', dtype=3, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/length_error_lower', dtype=7, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/length_error_upper', dtype=7, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/length', dtype=7, shot=84600, )
 */
TEST_CASE( "Test bpol_probe length", "[IMAS][JET][BPOL]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='magnetics/bpol_probe/#/length', indices='1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

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
    REQUIRE( val->as<float>() == Approx(0.15) );
}

/*
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/turns', dtype=3, shot=84600, )
 */
TEST_CASE( "Test bpol_probe turns", "[IMAS][JET][BPOL]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='magnetics/bpol_probe/#/turns', indices='1', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");

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
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/field/data_error_index', dtype=3, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/field/data_error_lower', dtype=7, rank=1, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/field/data_error_upper', dtype=7, rank=1, shot=84600, )
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/field/data', dtype=7, rank=1, shot=84600, )
 */
TEST_CASE( "Test bpol_probe field", "[IMAS][JET][BPOL]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='magnetics/bpol_probe/#/field/data', indices='1', experiment='JET', dtype=7, rank=1, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    REQUIRE( arr->size() == 1024 );
    REQUIRE( arr->type().name() == typeid(float).name() );
    REQUIRE( arr->as<float>()[0] == Approx(-0.0000273885871) );
}

TEST_CASE( "Test bpol_probe field error upper", "[IMAS][JET][BPOL]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='magnetics/bpol_probe/#/field/data_error_upper', indices='1', experiment='JET', dtype=7, rank=1, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    REQUIRE( arr->size() == 1024 );
    REQUIRE( arr->type().name() == typeid(float).name() );
    REQUIRE( arr->as<float>()[0] == Approx(0.00497f) );
}

TEST_CASE( "Test bpol_probe field error lower", "[IMAS][JET][BPOL]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='magnetics/bpol_probe/#/field/data_error_lower', indices='1', experiment='JET', dtype=7, rank=1, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    REQUIRE( arr->size() == 1024 );
    REQUIRE( arr->type().name() == typeid(float).name() );
    REQUIRE( arr->as<float>()[0] == Approx(-0.00503f) );
}

/*
 * imas::get(experiment='JET', idx=0, group='magnetics/bpol_probe/#/field/time', dtype=7, rank=1, shot=84600, )
 */
TEST_CASE( "Test bpol_probe time", "[IMAS][JET][BPOL]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='magnetics/bpol_probe/#/field/time', indices='1', experiment='JET', dtype=7, rank=1, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    REQUIRE( arr->size() == 1024 );
    REQUIRE( arr->type().name() == typeid(float).name() );
    REQUIRE( arr->as<float>()[0] == Approx(25.0161991) );
}

/*
  ✓ flux_loop/Shape_of
  ✓ flux_loop/#/name
  ✓ flux_loop/#/identifier
  ✓ flux_loop/#/position/Shape_of
  ✓ flux_loop/#/position/#/r
  ✓ flux_loop/#/position/#/z
  ✓ flux_loop/#/position/#/phi
  ✓ flux_loop/#/flux
  ✓ flux_loop/#/flux/time
 */

TEST_CASE( "Test flux_loop count", "[IMAS][JET][FLUX]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='magnetics/flux_loop/Shape_of', indices='', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");

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
    REQUIRE( val->as<int>() == 36 );
}

TEST_CASE( "Test flux_loop name", "[IMAS][JET][FLUX]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/name', indices='1', experiment='JET', dtype=17, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( data->isNull() );
}

TEST_CASE( "Test flux_loop identifier", "[IMAS][JET][FLUX]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/identifier', indices='1', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");

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

TEST_CASE( "Test flux_loop position count", "[IMAS][JET][FLUX]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    {
        const uda::Result& result = client.get(
                "EXP2IMAS::read(element='magnetics/flux_loop/#/position/Shape_of', experiment='JET', indices='1', dtype=3, shot=" SHOT_NUM ", IDS_version='')",
                "");

        REQUIRE(result.errorCode() == 0);
        REQUIRE(result.errorMessage().empty());

        uda::Data* data = result.data();

        REQUIRE(data != nullptr);
        REQUIRE(!data->isNull());
        REQUIRE(data->type().name() == typeid(int).name());

        auto val = dynamic_cast<uda::Scalar*>(data);

        REQUIRE(val != nullptr);
        REQUIRE(!val->isNull());

        REQUIRE(val->type().name() == typeid(int).name());
        REQUIRE(val->as<int>() == 1);
    }

    {
        const uda::Result& result = client.get(
                "EXP2IMAS::read(element='magnetics/flux_loop/#/position/Shape_of', indices='7', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')",
                "");

        REQUIRE(result.errorCode() == 0);
        REQUIRE(result.errorMessage().empty());

        uda::Data* data = result.data();

        REQUIRE(data != nullptr);
        REQUIRE(!data->isNull());
        REQUIRE(data->type().name() == typeid(int).name());

        auto val = dynamic_cast<uda::Scalar*>(data);

        REQUIRE(val != nullptr);
        REQUIRE(!val->isNull());

        REQUIRE(val->type().name() == typeid(int).name());
        REQUIRE(val->as<int>() == 2);
    }
}

TEST_CASE( "Test flux_loop position r", "[IMAS][JET][FLUX]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result_x = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/position/#/r', indices='1;1', experiment='JET', dtype=6, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result_x.errorCode() == 0 );
    REQUIRE( result_x.errorMessage().empty() );

    uda::Data* data = result_x.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto val = dynamic_cast<uda::Scalar*>(data);

    REQUIRE( val != nullptr );
    REQUIRE( !val->isNull() );

    REQUIRE( val->type().name() == typeid(float).name() );
    REQUIRE( val->as<float>() == Approx(2.6080f) );

    const uda::Result& result_y = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/position/#/r', indices='1;2', experiment='JET', dtype=6, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result_y.errorCode() == 0 );
    REQUIRE( result_y.errorMessage().empty() );

    data = result_y.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    val = dynamic_cast<uda::Scalar*>(data);

    REQUIRE( val != nullptr );
    REQUIRE( !val->isNull() );

    REQUIRE( val->type().name() == typeid(float).name() );
    REQUIRE( val->as<float>() == Approx(0.0f) );
}

TEST_CASE( "Test flux_loop position z", "[IMAS][JET][FLUX]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result_x = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/position/#/z', indices='1;1', experiment='JET', dtype=6, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result_x.errorCode() == 0 );
    REQUIRE( result_x.errorMessage().empty() );

    uda::Data* data = result_x.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto val = dynamic_cast<uda::Scalar*>(data);

    REQUIRE( val != nullptr );
    REQUIRE( !val->isNull() );

    REQUIRE( val->type().name() == typeid(float).name() );
    REQUIRE( val->as<float>() == Approx(2.3220f) );

    const uda::Result& result_y = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/position/#/z', indices='1;2', experiment='JET', dtype=6, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result_y.errorCode() == 0 );
    REQUIRE( result_y.errorMessage().empty() );

    data = result_y.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    val = dynamic_cast<uda::Scalar*>(data);

    REQUIRE( val != nullptr );
    REQUIRE( !val->isNull() );

    REQUIRE( val->type().name() == typeid(float).name() );
    REQUIRE( val->as<float>() == Approx(0.0f) );
}

TEST_CASE( "Test flux_loop position phi", "[IMAS][JET][FLUX]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result_x = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/position/#/phi', indices='1;1', experiment='JET', dtype=6, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result_x.errorCode() == 0 );
    REQUIRE( result_x.errorMessage().empty() );

    uda::Data* data = result_x.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto val = dynamic_cast<uda::Scalar*>(data);

    REQUIRE( val != nullptr );
    REQUIRE( !val->isNull() );

    REQUIRE( val->type().name() == typeid(float).name() );
    REQUIRE( val->as<float>() == Approx(0.0f) );

    const uda::Result& result_y = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/position/#/phi', indices='1;2', experiment='JET', dtype=6, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result_y.errorCode() == 0 );
    REQUIRE( result_y.errorMessage().empty() );

    data = result_y.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    val = dynamic_cast<uda::Scalar*>(data);

    REQUIRE( val != nullptr );
    REQUIRE( !val->isNull() );

    REQUIRE( val->type().name() == typeid(float).name() );
    REQUIRE( val->as<float>() == Approx(0.0f) );
}

TEST_CASE( "Test flux_loop flux", "[IMAS][JET][FLUX]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result_1 = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/flux/data', indices='1', experiment='JET', dtype=6, rank=1, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result_1.errorCode() == 0 );
    REQUIRE( result_1.errorMessage().empty() );

    uda::Data* data = result_1.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    REQUIRE( arr->size() == 1024 );
    REQUIRE( arr->type().name() == typeid(float).name() );
    REQUIRE( arr->as<float>()[0] == Approx(0.00029f) );

    const uda::Result& result_36 = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/flux/data', indices='36', experiment='JET', dtype=6, rank=1, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result_36.errorCode() == 0 );
    REQUIRE( result_36.errorMessage().empty() );

    data = result_36.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    REQUIRE( arr->size() == 1024 );
    REQUIRE( arr->type().name() == typeid(float).name() );
    REQUIRE( arr->as<float>()[0] == Approx(0.00089f) );
}

TEST_CASE( "Test flux_loop flux error upper", "[IMAS][JET][FLUX]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result_1 = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/flux/data_error_upper', indices='1', experiment='JET', dtype=6, rank=1, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result_1.errorCode() == 0 );
    REQUIRE( result_1.errorMessage().empty() );

    uda::Data* data = result_1.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    REQUIRE( arr->size() == 1024 );
    REQUIRE( arr->type().name() == typeid(float).name() );
    REQUIRE( arr->as<float>()[0] == Approx(0.00129f) );

    const uda::Result& result_36 = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/flux/data_error_upper', indices='36', experiment='JET', dtype=6, rank=1, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result_36.errorCode() == 0 );
    REQUIRE( result_36.errorMessage().empty() );

    data = result_36.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    REQUIRE( arr->size() == 1024 );
    REQUIRE( arr->type().name() == typeid(float).name() );
    REQUIRE( arr->as<float>()[0] == Approx(0.00929f) );
}

TEST_CASE( "Test flux_loop flux error lower", "[IMAS][JET][FLUX]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result_1 = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/flux/data_error_lower', indices='1', experiment='JET', dtype=6, rank=1, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result_1.errorCode() == 0 );
    REQUIRE( result_1.errorMessage().empty() );

    uda::Data* data = result_1.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    REQUIRE( arr->size() == 1024 );
    REQUIRE( arr->type().name() == typeid(float).name() );
    REQUIRE( arr->as<float>()[0] == Approx(-0.00071f) );

    const uda::Result& result_36 = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/flux/data_error_lower', indices='36', experiment='JET', dtype=6, rank=1, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result_36.errorCode() == 0 );
    REQUIRE( result_36.errorMessage().empty() );

    data = result_36.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    REQUIRE( arr->size() == 1024 );
    REQUIRE( arr->type().name() == typeid(float).name() );
    REQUIRE( arr->as<float>()[0] == Approx(-0.00751f) );
}

//TEST_CASE( "Test flux_loop flux errors", "[IMAS][JET][BPOL]" )
//{
//#ifdef FATCLIENT
//#  include "setup.inc"
//#endif
//
//    setenv("UDA_EXP2IMAS_MAPPING_FILE_DIRECTORY", MAPPINGS_DIR, 1);
//
//    uda::Client client;
//
////    const uda::Result& error_index = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/flux/data_error_index', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");
////
////    REQUIRE( error_index.errorCode() == 0 );
////    REQUIRE( error_index.errorMessage().empty() );
////
////    uda::Data* data = error_index.data();
////
////    REQUIRE( data != nullptr );
////    REQUIRE( !data->isNull() );
////    REQUIRE( data->type().name() == typeid(int).name() );
////
////    auto val = dynamic_cast<uda::Scalar*>(data);
////
////    REQUIRE( val != nullptr );
////    REQUIRE( !val->isNull() );
////
////    REQUIRE( val->type().name() == typeid(int).name() );
////    REQUIRE( val->as<int>() == 0 );
//
//    const uda::Result& error_upper = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/flux/data_error_upper', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");
//
//    REQUIRE( error_upper.errorCode() == 0 );
//    REQUIRE( error_upper.errorMessage().empty() );
//
//    uda::Data* data = error_upper.data();
//
//    REQUIRE( data != nullptr );
//    REQUIRE( !data->isNull() );
//    REQUIRE( data->type().name() == typeid(float).name() );
//
//    auto arr = dynamic_cast<uda::Array*>(data);
//
//    REQUIRE( arr != nullptr );
//    REQUIRE( !arr->isNull() );
//
//    REQUIRE( arr->size() == 1024 );
//    REQUIRE( arr->type().name() == typeid(float).name() );
//    REQUIRE( arr->as<float>()[0] == Approx(0.2953f) );
//}

TEST_CASE( "Test flux_loop time", "[IMAS][JET][FLUX]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result_1 = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/flux/time', indices='1', experiment='JET', dtype=6, rank=1, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result_1.errorCode() == 0 );
    REQUIRE( result_1.errorMessage().empty() );

    uda::Data* data = result_1.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    REQUIRE( arr->size() == 1024 );
    REQUIRE( arr->type().name() == typeid(float).name() );
    REQUIRE( arr->as<float>()[0] == Approx(25.0162f) );
    REQUIRE( arr->as<float>()[1023] == Approx(55.1882f) );

    const uda::Result& result_36 = client.get("EXP2IMAS::read(element='magnetics/flux_loop/#/flux/time', indices='36', experiment='JET', dtype=6, rank=1, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result_36.errorCode() == 0 );
    REQUIRE( result_36.errorMessage().empty() );

    data = result_36.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    REQUIRE( arr->size() == 1024 );
    REQUIRE( arr->type().name() == typeid(float).name() );
    REQUIRE( arr->as<float>()[0] == Approx(25.0162f) );
    REQUIRE( arr->as<float>()[1023] == Approx(55.1882f) );
}

/*
  pf_active/coil/Shape_of
  pf_active/coil/#/element/Shape_of
  pf_active/vertical_force/Shape_of
  pf_active/circuit/Shape_of
  pf_active/supply/Shape_of
 */

/*
  tf/coil/Shape_of
  tf/coil/#/conductor/Shape_of
  tf/field_map/Shape_of
  tf/field_map/grid/space/Shape_of
  tf/field_map/#/grid/space/objects_per_dimension(:)/Shape_of
  tf/field_map/#/grid/space/objects_per_dimension(:)/object(:)/Shape_of
  tf/field_map/#/grid/space/objects_per_dimension(:)/object(:)/boundary(:)/Shape_of
  tf/field_map/grid/grid_subset/Shape_of
  tf/field_map/#/grid/grid_subset/element(:)/Shape_of
  tf/field_map/#/grid/grid_subset/element(:)/object(:)/Shape_of
  tf/field_map/#/grid/grid_subset/base(:)/Shape_of
  tf/field_map/#/b_r/Shape_of
  tf/field_map/#/b_z/Shape_of
  tf/field_map/#/b_tor/Shape_of
  tf/field_map/#/a_r/Shape_of
  tf/field_map/#/a_z/Shape_of
  tf/field_map/#/a_tor/Shape_of
 */

/*
  wall/global_quantities/neutral/Shape_of
  wall/global_quantities/neutral/#/element/Shape_of
  wall/description_2d/Shape_of
  wall/description_2d/limiter/unit/Shape_of
  wall/description_2d/vessel/unit/Shape_of
  wall/description_2d/#/vessel/unit/element(:)/Shape_of
 */
