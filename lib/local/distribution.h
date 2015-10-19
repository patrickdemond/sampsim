/*=========================================================================

  Program:  sampsim
  Module:   distribution.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_distribution_h
#define __sampsim_distribution_h

#include "base_object.h"

#include "pareto.h"

#include <random>

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  /**
   * @class distribution
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief A distribution object which encapsulates multiple statistical distributions
   * @details
   * Distribution objects provide random values which conform to a particular distribution such
   * as normal, log-normal, pareto, poisson and weibull distributions.
   */
  class distribution : public base_object
  {
  public:

    /**
     * A list of all supported distribution types
     */
    enum type
    {
      UNKNOWN = 0,
      LOGNORMAL,
      NORMAL,
      PARETO,
      POISSON,
      WEIBULL
    };

    /**
     * Converts a string representation of a distribution to a type code
     */
    inline static distribution::type get_type( const std::string name )
    {
      if( "lognormal" == name ) return LOGNORMAL;
      else if( "normal" == name ) return NORMAL;
      else if( "pareto" == name ) return PARETO;
      else if( "poisson" == name ) return POISSON;
      else if( "weibull" == name ) return WEIBULL;
      return UNKNOWN;
    }

    /**
     * Converts a type code representation of a distribution to a string
     */
    inline static std::string get_type_name( const distribution::type distribution_type )
    {
      if( LOGNORMAL == distribution_type ) return "lognormal";
      else if( NORMAL == distribution_type ) return "normal";
      else if( PARETO == distribution_type ) return "pareto";
      else if( POISSON == distribution_type ) return "poisson";
      else if( WEIBULL == distribution_type ) return "weibull";
      return "unknown";
    }

    /**
     * Constructor
     */
    distribution() : distribution_type( distribution::UNKNOWN ) {}

    /**
     * Copies another distribution's values into the current object
     */
    void copy( const distribution* );

    /**
     * Returns the name of the object's class
     */
    std::string get_name() const { return "distribution"; }

    /**
     * Returns the distribution's type code
     */
    distribution::type get_type() const { return this->distribution_type; }

    /**
     * Returns the distribution's type code as a string
     */
    std::string get_type_name() const { return distribution::get_type_name( this->distribution_type ); }

    /**
     * Sets the distribution type to a log-normal distribution
     * 
     * The input parameters relate to the characteristics of that underlying normal distribution.
     * See http://www.cplusplus.com/reference/random/lognormal_distribution/ for more details.
     */
    virtual void set_lognormal( const double mean = 0.0, const double sd = 1.0 );

    /**
     * Sets the distribution type to a normal distribution
     * 
     * The input parameters relate to the mean (μ) and stddev (σ).
     * See http://www.cplusplus.com/reference/random/normal_distribution/ for more details.
     */
    virtual void set_normal( const double m = 0.0, const double s = 1.0 );

    /**
     * Sets the distribution type to a pareto distribution
     * 
     * The input parameters relate to the pareto minimum value (b), shape parameter (a) and
     * maximum (truncated) value (max).
     * See http://en.wikipedia.org/wiki/Pareto_distribution for more details.
     */
    virtual void set_pareto( const double b = 1.0, const double a = 1.0, const double max = 0 );

    /**
     * Sets the distribution type to a poisson distribution
     * 
     * The input parameters relate to the mean (μ).
     * See http://www.cplusplus.com/reference/random/poisson_distribution/ for more details.
     */
    virtual void set_poisson( const double mean = 1.0 );

    /**
     * Sets the distribution type to a weibull distribution
     * 
     * The input parameters relate to the two weibull parameters.
     * See http://www.cplusplus.com/reference/random/weibull_distribution/ for more details.
     */
    virtual void set_weibull( const double a = 1.0, const double b = 1.0 );

    /**
     * Generates a random value conforming to the distribution's current type
     */
    double generate_value();

    /**
     * Returns the trend as a string representation
     */
    std::string to_string();

    /**
     * Deserialize the distribution
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void from_json( const Json::Value& );

    /**
     * Serialize the distribution
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void to_json( Json::Value& ) const;

    /**
     * Output the distribution to two CSV files (households and individuals)
     * 
     * All objects must provide an implementation for printing to a CSV (comma separated value) format.
     * Two streams are expected, the first is for household data and the second for individual data.
     */
    virtual void to_csv( std::ostream&, std::ostream& ) const;

  protected:

    /**
     * The distribution's current type.
     */
    distribution::type distribution_type;

    /**
     * A reference to the internal object which provides random values from a lognormal distribution
     */
    std::lognormal_distribution<double> lognormal;

    /**
     * A reference to the internal object which provides random values from a normal distribution
     */
    std::normal_distribution<double> normal;

    /**
     * A function which returns random values from a pareto distribution
     */
    sampsim::pareto pareto;

    /**
     * A reference to the internal object which provides random values from a poisson distribution
     */
    std::poisson_distribution<int> poisson;

    /**
     * A reference to the internal object which provides random values from a weibull distribution
     */
    std::weibull_distribution<double> weibull;
  };
}

/** @} end of doxygen group */

#endif
