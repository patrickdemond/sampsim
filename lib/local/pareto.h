/*=========================================================================

  Program:  sampsim
  Module:   pareto.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_pareto_h
#define __sampsim_pareto_h

#include <random>

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  /**
   * @class pareto
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief A class which provides random numbers from a Paredo distribution
   * @details
   * The Paredo distribution returns values using the following formula:
   *   b / ( 1-p )^( 1/a )
   * where b is the minimum value in the distribution, p is a random number in [0,1) and
   * a is the shape parameter which defines the shape of the distribution's tail.
   */
  class pareto
  {
  public:

    /**
     * Constructor
     */
    pareto() : b_val( 1.0 ), a_val( 1.0 ), max_val( 0 ) {}
    pareto( double b, double a ) : b_val( b ), a_val( a ), max_val( 0 ) {}
    pareto( double b, double a, double max ) : b_val( b ), a_val( a ), max_val( max ) {}

    /**
     * Generating function
     */
    double operator()( std::mt19937 &random_engine )
    {
      double rand = static_cast< double >( random_engine() - random_engine.min() ) /
                    static_cast< double >( random_engine.max() - random_engine.min() + 1 );
      double val = this->b_val / pow( 1.0 - rand, 1 / this->a_val );
      if( 0 < this->max_val && this->max_val < val ) val = this->max_val;
      return val;
    }

    /**
     * Returns the b parameter (minimum value in the distribution)
     */
    double b() const { return this->b_val; }

    /**
     * Returns the a parameter (shape parameter which defines the shape of the tail)
     */
    double a() const { return this->a_val; }

    /**
     * Returns the max parameter (maximum cut-off value in the distribution)
     */
    double max() const { return this->max_val; }

  protected:

    /**
     * The minimum value in the distribution.
     */
    double b_val;

    /**
     * The shape parameter which defines the shape of the distribution's tail.
     */
    double a_val;

    /**
     * The maximum value that the distribution should return.  If set to 0 then no maximum value
     * will be applied.
     */
    double max_val;
  };
}

/** @} end of doxygen group */

#endif
