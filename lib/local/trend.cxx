/*=========================================================================

  Program:  sampsim
  Module:   trend.cxx
  Language: C++

=========================================================================*/

#include "trend.h"

#include <json/value.h>
#include <sstream>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  trend::trend(
    const double b00,
    const double b01,
    const double b10,
    const double b02,
    const double b20,
    const double b11 )
  {
    this->b[0][0] = b00;
    this->b[1][0] = b01;
    this->b[2][0] = b10;
    this->b[3][0] = b02;
    this->b[4][0] = b20;
    this->b[5][0] = b11;

    for( unsigned int index = 0; index < 6; index++ )
    {
      this->b[index][1] = 0.0;
      this->b[index][2] = 0.0;
      this->b[index][3] = std::numeric_limits<double>::quiet_NaN();
    }

    // now set the regression factor to 0 (no regression)
    this->set_regression_factor( 0.0 );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void trend::copy( const trend* t )
  {
    for( unsigned int index = 0; index < 6; index++ )
      std::copy( std::begin( t->b[index] ), std::end( t->b[index] ), std::begin( this->b[index] ) );
    this->set_regression_factor( t->regression_factor );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void trend::set_regression_factor( const double factor )
  {
    this->regression_factor = factor;
    this->initialize_distributions();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void trend::initialize_distributions()
  {
    this->debug( "initialize_distributions()" );
    for( unsigned int index = 0; index < 6; index++ )
    {
      this->dist[index].set_normal(
        this->b[index][0] + this->b[index][1] * this->regression_factor,
        this->b[index][2] );
      this->b[index][3] = std::numeric_limits<double>::quiet_NaN();
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double trend::get_value( const coordinate c )
  {
    double value = this->get_b00() +
                   this->get_b01() * c.x +
                   this->get_b10() * c.y +
                   this->get_b02() * c.x * c.x +
                   this->get_b20() * c.y * c.y +
                   this->get_b11() * c.x * c.y;
    this->debug( "get_value( coordinate = (%f,%f) ) = %s = %f", c.x, c.y, this->to_string().c_str(), value );
    return value;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double trend::get_constant( const unsigned int index )
  {
    if( 6 <= index )
    {
      std::stringstream stream;
      stream << "ERROR: trend coefficient index \"" << index << "\" is out of bounds";
      throw std::runtime_error( stream.str() );
    }

    // cache the value of the constant
    if( isnan( this->b[index][3] ) )
    {
      this->b[index][3] = this->dist[index].generate_value();
      this->debug( "get_constant( index = %d ) caching value of %f", index, this->b[index][3] );
    }

    return this->b[index][3];
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double trend::get_base( const unsigned int index ) const
  {
    if( 6 <= index )
    {
      std::stringstream stream;
      stream << "ERROR: trend coefficient index \"" << index << "\" is out of bounds";
      throw std::runtime_error( stream.str() );
    }

    return this->b[index][0];
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double trend::get_regression( const unsigned int index ) const
  {
    if( 6 <= index )
    {
      std::stringstream stream;
      stream << "ERROR: trend coefficient index \"" << index << "\" is out of bounds";
      throw std::runtime_error( stream.str() );
    }

    return this->b[index][1];
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double trend::get_variance( const unsigned int index ) const
  {
    if( 6 <= index )
    {
      std::stringstream stream;
      stream << "ERROR: trend coefficient index \"" << index << "\" is out of bounds";
      throw std::runtime_error( stream.str() );
    }

    return this->b[index][2];
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void trend::set_coefficient(
     const unsigned int index,
     const double value,
     const double regression,
     const double variance )
  {
    if( 6 <= index )
    {
      std::stringstream stream;
      stream << "ERROR: trend coefficient index \"" << index << "\" is out of bounds";
      throw std::runtime_error( stream.str() );
    }

    this->debug(
      "set_coefficient( index=%d, value=%f, regression=%f, variance=%f )",
      index, value, regression, variance );
    this->b[index][0] = value;
    this->b[index][1] = regression;
    this->b[index][2] = variance;
    this->initialize_distributions();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string trend::to_string()
  {
    bool first = true;
    std::stringstream stream;

    double b00 = this->get_b00();
    if( 0 != b00 )
    {
      if( first ) first = false;
      else stream << " + ";
      stream << b00;
    }

    double b01 = this->get_b01();
    if( 0 != b01 )
    {
      if( first ) first = false;
      else stream << " + ";
      stream << b01 << "x";
    }

    double b10 = this->get_b10();
    if( 0 != b10 )
    {
      if( first ) first = false;
      else stream << " + ";
      stream << b10 << "y";
    }

    double b02 = this->get_b02();
    if( 0 != b02 )
    {
      if( first ) first = false;
      else stream << " + ";
      stream << b02 << "x^2";
    }

    double b20 = this->get_b20();
    if( 0 != b20 )
    {
      if( first ) first = false;
      else stream << " + ";
      stream << b20 << "y^2";
    }

    double b11 = this->get_b11();
    if( 0 != b11 )
    {
      if( first ) first = false;
      else stream << " + ";
      stream << b11 << "xy";
    }

    return 0 == stream.str().length() ? "0" : stream.str();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void trend::from_json( const Json::Value &json )
  {
    this->b[0][0] = json["b00"][0].asDouble();
    this->b[0][1] = json["b00"][1].asDouble();
    this->b[0][2] = json["b00"][2].asDouble();
    this->b[1][0] = json["b01"][0].asDouble();
    this->b[1][1] = json["b01"][1].asDouble();
    this->b[1][2] = json["b01"][2].asDouble();
    this->b[2][0] = json["b10"][0].asDouble();
    this->b[2][1] = json["b10"][1].asDouble();
    this->b[2][2] = json["b10"][2].asDouble();
    this->b[3][0] = json["b02"][0].asDouble();
    this->b[3][1] = json["b02"][1].asDouble();
    this->b[3][2] = json["b02"][2].asDouble();
    this->b[4][0] = json["b20"][0].asDouble();
    this->b[4][1] = json["b20"][1].asDouble();
    this->b[4][2] = json["b20"][2].asDouble();
    this->b[5][0] = json["b11"][0].asDouble();
    this->b[5][1] = json["b11"][1].asDouble();
    this->b[5][2] = json["b11"][2].asDouble();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void trend::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["b00"] = Json::Value( Json::arrayValue );
    json["b00"].resize( 3 );
    json["b00"][0] = this->b[0][0];
    json["b00"][1] = this->b[0][1];
    json["b00"][2] = this->b[0][2];
    json["b01"] = Json::Value( Json::arrayValue );
    json["b01"].resize( 3 );
    json["b01"][0] = this->b[1][0];
    json["b01"][1] = this->b[1][1];
    json["b01"][2] = this->b[1][2];
    json["b10"] = Json::Value( Json::arrayValue );
    json["b10"].resize( 3 );
    json["b10"][0] = this->b[2][0];
    json["b10"][1] = this->b[2][1];
    json["b10"][2] = this->b[2][2];
    json["b02"] = Json::Value( Json::arrayValue );
    json["b02"].resize( 3 );
    json["b02"][0] = this->b[3][0];
    json["b02"][1] = this->b[3][1];
    json["b02"][2] = this->b[3][2];
    json["b20"] = Json::Value( Json::arrayValue );
    json["b20"].resize( 3 );
    json["b20"][0] = this->b[4][0];
    json["b20"][1] = this->b[4][1];
    json["b20"][2] = this->b[4][2];
    json["b11"] = Json::Value( Json::arrayValue );
    json["b11"].resize( 3 );
    json["b11"][0] = this->b[5][0];
    json["b11"][1] = this->b[5][1];
    json["b11"][2] = this->b[5][2];
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void trend::to_csv( std::ostream &household_stream, std::ostream &individual_stream ) const
  {
    // trends are not included in either the household or individual stream, so we have nothing to do
  }
}
