/*=========================================================================

  Program:  sampsim
  Module:   pareto.cxx
  Language: C++

=========================================================================*/

#include "pareto.h"

#include "utilities.h"

#include <json/value.h>
#include <sstream>
#include <stdexcept>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void pareto::set_lognormal( const double mean, const double sd )
  {
    this->pareto_type = pareto::LOGNORMAL;
    this->lognormal = std::lognormal_pareto<double>( mean, sd );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void pareto::set_normal( const double m, const double s )
  {
    this->pareto_type = pareto::NORMAL;
    this->normal = std::normal_pareto<double>( m, s );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void pareto::set_pareto( const double b, const double a, const double max )
  {
    this->pareto_type = pareto::POISSON;
    this->pareto = pareto( b, a, max );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void pareto::set_poisson( const double mean )
  {
    this->pareto_type = pareto::POISSON;
    this->poisson = std::poisson_pareto<int>( mean );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void pareto::set_weibull( const double a, const double b )
  {
    this->pareto_type = pareto::WEIBULL;
    this->weibull = std::weibull_pareto<double>( a, b );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double pareto::generate_value()
  {
    if( pareto::LOGNORMAL == this->pareto_type )
      return this->lognormal( utilities::random_engine );
    else if( pareto::NORMAL == this->pareto_type )
      return this->normal( utilities::random_engine );
    else if( pareto::PAREDO == this->pareto_type )
      return this->pareto( utilities::random_engine );
    else if( pareto::POISSON == this->pareto_type )
      return static_cast<double>( this->poisson( utilities::random_engine ) );
    else if( pareto::WEIBULL == this->pareto_type )
      return this->weibull( utilities::random_engine );

    throw std::runtime_error( "Cannot generate value using unknown pareto" );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void pareto::from_json( const Json::Value &json )
  {
    pareto::type type = pareto::get_type( json["type"].asString() );

    if( pareto::LOGNORMAL == type )
    {
      this->set_lognormal( json["m"].asDouble(), json["s"].asDouble() );
    }
    else if( pareto::NORMAL == type )
    {
      this->set_normal( json["mean"].asDouble(), json["stddev"].asDouble() );
    }
    else if( pareto::PAREDO == type )
    {
      this->set_pareto( json["b"].asDouble(), json["a"].asDouble(), json["max"].asDouble() );
    }
    else if( pareto::POISSON == type )
    {
      this->set_poisson( json["mean"].asDouble() );
    }
    else if( pareto::WEIBULL == type )
    {
      this->set_weibull( json["a"].asDouble(), json["b"].asDouble() );
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void pareto::to_json( Json::Value &json ) const
  {
    json = Json::Value( Json::objectValue );
    json["type"] = this->get_type_name();

    if( pareto::LOGNORMAL == this->pareto_type )
    {
      json["m"] = this->lognormal.m();
      json["s"] = this->lognormal.s();
    }
    else if( pareto::NORMAL == this->pareto_type )
    {
      json["mean"] = this->normal.mean();
      json["stddev"] = this->normal.stddev();
    }
    else if( pareto::PAREDO == this->pareto_type )
    {
      json["b"] = this->poisson.b();
      json["a"] = this->poisson.a();
      json["max"] = this->poisson.max();
    }
    else if( pareto::POISSON == this->pareto_type )
    {
      json["mean"] = this->poisson.mean();
    }
    else if( pareto::WEIBULL == this->pareto_type )
    {
      json["a"] = this->weibull.a();
      json["b"] = this->weibull.b();
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void pareto::to_csv( std::ostream &household_stream, std::ostream &individual_stream ) const
  {
    // paretos are not included in either the household or individual stream, so we have nothing to do
  }
}
