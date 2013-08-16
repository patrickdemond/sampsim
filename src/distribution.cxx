/*=========================================================================

  Program:  sampsim
  Module:   distribution.cxx
  Language: C++

=========================================================================*/

#include "distribution.h"

#include "utilities.h"

#include <json/value.h>
#include <sstream>
#include <stdexcept>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void distribution::set_lognormal( const double mean, const double sd )
  {
    if( distribution::UNKNOWN != this->distribution_type )
      throw std::runtime_error( "Tried to set distribution type after it has already been defined" );
    this->distribution_type = distribution::LOGNORMAL;
    this->lognormal = std::lognormal_distribution<double>( mean, sd );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void distribution::set_normal( const double m, const double s )
  {
    if( distribution::UNKNOWN != this->distribution_type )
      throw std::runtime_error( "Tried to set distribution type after it has already been defined" );
    this->distribution_type = distribution::NORMAL;
    this->normal = std::normal_distribution<double>( m, s );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void distribution::set_poisson( const double mean )
  {
    if( distribution::UNKNOWN != this->distribution_type )
      throw std::runtime_error( "Tried to set distribution type after it has already been defined" );
    this->distribution_type = distribution::POISSON;
    this->poisson = std::poisson_distribution<int>( mean );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void distribution::set_weibull( const double a, const double b )
  {
    if( distribution::UNKNOWN != this->distribution_type )
      throw std::runtime_error( "Tried to set distribution type after it has already been defined" );
    this->distribution_type = distribution::WEIBULL;
    this->weibull = std::weibull_distribution<double>( a, b );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double distribution::generate_value()
  {
    if( distribution::LOGNORMAL == this->distribution_type )
      return this->lognormal( utilities::random_engine );
    else if( distribution::NORMAL == this->distribution_type )
      return this->normal( utilities::random_engine );
    else if( distribution::POISSON == this->distribution_type )
      return static_cast<double>( this->poisson( utilities::random_engine ) );
    else if( distribution::WEIBULL == this->distribution_type )
      return this->weibull( utilities::random_engine );

    throw std::runtime_error( "Cannot generate value using unknown distribution" );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void distribution::from_json( const Json::Value &json )
  {
    // TODO: implement
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void distribution::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["type"] = this->get_type_name();

    if( distribution::LOGNORMAL == this->distribution_type )
    {
      json["m"] = this->lognormal.m();
      json["s"] = this->lognormal.s();
    }
    else if( distribution::NORMAL == this->distribution_type )
    {
      json["mean"] = this->normal.mean();
      json["stddev"] = this->normal.stddev();
    }
    else if( distribution::POISSON == this->distribution_type )
    {
      json["mean"] = this->poisson.mean();
    }
    else if( distribution::WEIBULL == this->distribution_type )
    {
      json["a"] = this->weibull.a();
      json["b"] = this->weibull.b();
    }
  }
}
