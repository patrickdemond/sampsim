/*=========================================================================

  Program:  sampsim
  Module:   distribution.h
  Language: C++

=========================================================================*/

/**
 * @class distribution
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief A distribution object which encapsulates multiple statistical distributions
 */

#ifndef __sampsim_distribution_h
#define __sampsim_distribution_h

#include "base_object.h"

#include <random>

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class distribution : public base_object
  {
  public:
    enum type
    {
      UNKNOWN = 0,
      LOGNORMAL,
      NORMAL,
      POISSON,
      WEIBULL
    };

    inline static distribution::type get_type( const std::string name )
    {
      if( "lognormal" == name ) return LOGNORMAL;
      else if( "normal" == name ) return NORMAL;
      else if( "poisson" == name ) return POISSON;
      else if( "weibull" == name ) return WEIBULL;
      return UNKNOWN;
    }

    inline static std::string get_type_name( const distribution::type distribution_type )
    {
      if( LOGNORMAL == distribution_type ) return "lognormal";
      else if( NORMAL == distribution_type ) return "normal";
      else if( POISSON == distribution_type ) return "poisson";
      else if( WEIBULL == distribution_type ) return "weibull";
      return "unknown";
    }

    distribution() : distribution_type( distribution::UNKNOWN ) {}
    ~distribution() {}

    distribution::type get_type() const { return this->distribution_type; }
    std::string get_type_name() const { return distribution::get_type_name( this->distribution_type ); }

    virtual void set_lognormal( const double mean = 0.0, const double sd = 1.0 );
    virtual void set_normal( const double m = 0.0, const double s = 1.0 );
    virtual void set_poisson( const double mean = 1.0 );
    virtual void set_weibull( const double a = 1.0, const double b = 1.0 );

    double generate_value();
    virtual void to_json( Json::Value& ) const;

  protected:
    distribution::type distribution_type;

    // distributions
    std::lognormal_distribution<double> lognormal;
    std::normal_distribution<double> normal;
    std::poisson_distribution<int> poisson;
    std::weibull_distribution<double> weibull;
  };
}

/** @} end of doxygen group */

#endif
