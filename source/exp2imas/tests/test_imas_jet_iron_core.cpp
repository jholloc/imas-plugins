#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_helpers.h"

#include <c++/UDA.hpp>

#define QUOTE_(X) #X
#define QUOTE(X) QUOTE_(X)
#define SHOT_NUM "84600"

/*
  <mapping key="iron_core/ids_properties/comment" value="JET data mapped using UDA and exp2itm mappings 4.10b" type="constant"/>
  <mapping key="iron_core/ids_properties/homogeneous_time" value="0" type="constant"/>
  <mapping key="iron_core/segment/Shape_of" value="10" type="constant"/>
  <mapping key="iron_core/segment/#/name" value="" type="constant"/>
  <mapping key="iron_core/segment/#/identifier" value="" type="constant"/>
  <mapping key="iron_core/segment/#/b_field" value="//ironmodel/desc_iron/permeability/b" type="static"/>
  <mapping key="iron_core/segment/#/permeability_relative" value="//ironmodel/desc_iron/permeability/mur" type="constant"/>
  <mapping key="iron_core/segment/#/geometry/geometry_type" value="1" type="constant"/>
  <mapping key="iron_core/segment/#/geometry/outline/r" value="//ironmodel/desc_iron/geom_iron/rzcoordinate/r" type="constant"/>
  <mapping key="iron_core/segment/#/geometry/outline/z" value="//ironmodel/desc_iron/geom_iron/rzcoordinate/z" type="constant"/>
  <mapping key="iron_core/segment/#/magnetisation_r/data" value="0" type="constant"/>
  <mapping key="iron_core/segment/#/magnetisation_z/data" value="0" type="constant"/>
 */

TEST_CASE( "Test iron_core comment", "[IMAS][JET][IRON_CORE]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='iron_core/ids_properties/comment', indices='', experiment='JET', dtype=17, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(char*).name() );

    auto str = dynamic_cast<uda::String*>(data);

    REQUIRE( str != nullptr );
    REQUIRE( !str->isNull() );

    REQUIRE( str->str() == "JET data mapped using UDA and exp2itm mappings 4.10b" );
}

TEST_CASE( "Test iron_core homogeneous_time", "[IMAS][JET][IRON_CORE]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='iron_core/ids_properties/homogeneous_time', indices='', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(int).name() );

    auto val = dynamic_cast<uda::Scalar*>(data);

    REQUIRE( val != nullptr );
    REQUIRE( !val->isNull() );

    REQUIRE( val->as<int>() == 0 );
}

TEST_CASE( "Test iron_core number of segments", "[IMAS][JET][IRON_CORE]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='iron_core/segment/Shape_of', indices='', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(int).name() );

    auto val = dynamic_cast<uda::Scalar*>(data);

    REQUIRE( val != nullptr );
    REQUIRE( !val->isNull() );

    REQUIRE( val->as<int>() == 31 );
}

TEST_CASE( "Test iron_core segment name", "[IMAS][JET][IRON_CORE]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='iron_core/segment/#/name', indices='1', experiment='JET', dtype=17, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(char*).name() );

    auto str = dynamic_cast<uda::String*>(data);

    REQUIRE( str != nullptr );
    REQUIRE( !str->isNull() );

    REQUIRE( str->str() == "" );
}

TEST_CASE( "Test iron_core segment identifier", "[IMAS][JET][IRON_CORE]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='iron_core/segment/#/identifier', indices='1', experiment='JET', dtype=17, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(char*).name() );

    auto str = dynamic_cast<uda::String*>(data);

    REQUIRE( str != nullptr );
    REQUIRE( !str->isNull() );

    REQUIRE( str->str() == "" );
}

TEST_CASE( "Test iron_core segment b_field", "[IMAS][JET][IRON_CORE]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='iron_core/segment/#/b_field', indices='1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    REQUIRE( arr->size() == 27 );
    REQUIRE( arr->type().name() == typeid(float).name() );
    REQUIRE( arr->as<float>()[0] == Approx(0.07) );
    REQUIRE( arr->as<float>()[1] == Approx(0.25) );
    REQUIRE( arr->as<float>()[2] == Approx(0.6) );
}

TEST_CASE( "Test iron_core segment permeability_relative", "[IMAS][JET][IRON_CORE]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='iron_core/segment/#/permeability_relative', indices='1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    REQUIRE( arr->size() == 27 );
    REQUIRE( arr->type().name() == typeid(float).name() );
    REQUIRE( arr->as<float>()[0] == Approx(700) );
    REQUIRE( arr->as<float>()[1] == Approx(1250) );
    REQUIRE( arr->as<float>()[2] == Approx(2000) );
}

TEST_CASE( "Test iron_core segment geometry type", "[IMAS][JET][IRON_CORE]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='iron_core/segment/#/geometry/geometry_type', indices='1', experiment='JET', dtype=3, shot=" SHOT_NUM ", IDS_version='')", "");

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

TEST_CASE( "Test iron_core segment geometry outline r", "[IMAS][JET][IRON_CORE]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='iron_core/segment/#/geometry/outline/r', indices='1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    REQUIRE( arr->size() == 4 );
    REQUIRE( arr->type().name() == typeid(float).name() );
    REQUIRE( arr->as<float>()[0] == Approx(0.522000) );
    REQUIRE( arr->as<float>()[1] == Approx(0.800000) );
    REQUIRE( arr->as<float>()[2] == Approx(0.800000) );
    REQUIRE( arr->as<float>()[3] == Approx(0.522000) );
}

TEST_CASE( "Test iron_core segment geometry outline z", "[IMAS][JET][IRON_CORE]" )
{
#ifdef FATCLIENT
#  include "setup.inc"
#endif

    uda::Client client;

    const uda::Result& result = client.get("EXP2IMAS::read(element='iron_core/segment/#/geometry/outline/z', indices='1', experiment='JET', dtype=7, shot=" SHOT_NUM ", IDS_version='')", "");

    REQUIRE( result.errorCode() == 0 );
    REQUIRE( result.errorMessage().empty() );

    uda::Data* data = result.data();

    REQUIRE( data != nullptr );
    REQUIRE( !data->isNull() );
    REQUIRE( data->type().name() == typeid(float).name() );

    auto arr = dynamic_cast<uda::Array*>(data);

    REQUIRE( arr != nullptr );
    REQUIRE( !arr->isNull() );

    REQUIRE( arr->size() == 4 );
    REQUIRE( arr->type().name() == typeid(float).name() );
    REQUIRE( arr->as<float>()[0] == Approx(2.763000) );
    REQUIRE( arr->as<float>()[1] == Approx(2.763000) );
    REQUIRE( arr->as<float>()[2] == Approx(3.033000) );
    REQUIRE( arr->as<float>()[3] == Approx(3.033000) );
}
