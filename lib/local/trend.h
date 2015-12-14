/*=========================================================================

  Program:  sampsim
  Module:   trend.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_trend_h
#define __sampsim_trend_h

#include "base_object.h"

#include "coordinate.h"
#include "distribution.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace Json { class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class coordinate;

  /**
   * @class trend
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief A generic object defining an algebraic trend
   * @details
   * Trends are expressed in the form:
   *   b00 + b01 X + b10 Y + b02 X^2 + b20 Y^2 + b11 XY
   * where the constants, b00, b01, b10, b02, b20 and b11 are expressed in terms of a linear regression
   * coefficient and a residual variance:
   *   B(s) = Normal( b + S(x), σ^2 )
   * where b is the default value, σ is the residual variance and S(x) is the value returned by the
   * linear regression at a given value, x, such that the mean value is unchanged for x = 0
   */
  class trend : public base_object
  {
  public:
    /**
     * Constructor
     */
    trend(
      const double b00 = 1,
      const double b01 = 0,
      const double b10 = 0,
      const double b02 = 0,
      const double b20 = 0,
      const double b11 = 0 );

    // defining pure abstract methods
    std::string get_name() const { return "trend"; }
    void copy( const base_object* o ) { this->copy( static_cast<const trend*>( o ) ); }
    void copy( const trend* );
    void from_json( const Json::Value& );
    void to_json( Json::Value& ) const;
    void to_csv( std::ostream&, std::ostream& ) const;

    /**
     * Returns the regression factor defining how much the regression affects each coefficient
     */
    double get_regression_factor() const { return this->regression_factor; }

    /**
     * Sets the regression factor defining how much the regression affects each coefficient
     */
    void set_regression_factor( const double factor );

    /**
     * Returns the value of the trend at a particular coordinate at a particular regression point
     */
    double get_value( const coordinate );

    /**
     * Returns a calculated constant by index
     */
    double get_constant( const unsigned int index );

    /**
     * Returns a base value by index
     */
    double get_base( const unsigned int index ) const;

    /**
     * Returns a regression coefficient by index
     */
    double get_regression( const unsigned int index ) const;

    /**
     * Returns a residual variance by index
     */
    double get_variance( const unsigned int index ) const;

    /**
     * Sets a coefficient by index
     */
    void set_coefficient(
      const unsigned int index,
      const double value,
      const double regression = 0.0,
      const double variance = 0.0 );

    /**
     * Convenience methods
     */
    void set_coefficient( const unsigned int index, const double arr[3] )
    { this->set_coefficient( arr[0], arr[1], arr[2] ); }
    void set_coefficient( const unsigned int index, const std::vector< double > vec )
    { this->set_coefficient( vec[0], vec[1], vec[2] ); }

    /**
     * Convenience methods
     */
    double get_b00() { return this->get_constant( 0 ); }
    double get_b01() { return this->get_constant( 1 ); }
    double get_b10() { return this->get_constant( 2 ); }
    double get_b02() { return this->get_constant( 3 ); }
    double get_b20() { return this->get_constant( 4 ); }
    double get_b11() { return this->get_constant( 5 ); }
    double get_b00_base() const { return this->get_base( 0 ); }
    double get_b01_base() const { return this->get_base( 1 ); }
    double get_b10_base() const { return this->get_base( 2 ); }
    double get_b02_base() const { return this->get_base( 3 ); }
    double get_b20_base() const { return this->get_base( 4 ); }
    double get_b11_base() const { return this->get_base( 5 ); }
    double get_b00_regression() const { return this->get_regression( 0 ); }
    double get_b01_regression() const { return this->get_regression( 1 ); }
    double get_b10_regression() const { return this->get_regression( 2 ); }
    double get_b02_regression() const { return this->get_regression( 3 ); }
    double get_b20_regression() const { return this->get_regression( 4 ); }
    double get_b11_regression() const { return this->get_regression( 5 ); }
    double get_b00_variance() const { return this->get_variance( 0 ); }
    double get_b01_variance() const { return this->get_variance( 1 ); }
    double get_b10_variance() const { return this->get_variance( 2 ); }
    double get_b02_variance() const { return this->get_variance( 3 ); }
    double get_b20_variance() const { return this->get_variance( 4 ); }
    double get_b11_variance() const { return this->get_variance( 5 ); }
    void set_b00( const double value, const double regression = 0.0, const double variance = 0.0 )
    { this->set_coefficient( 0, value, regression, variance ); }
    void set_b01( const double value, const double regression = 0.0, const double variance = 0.0 )
    { this->set_coefficient( 1, value, regression, variance ); }
    void set_b10( const double value, const double regression = 0.0, const double variance = 0.0 )
    { this->set_coefficient( 2, value, regression, variance ); }
    void set_b02( const double value, const double regression = 0.0, const double variance = 0.0 )
    { this->set_coefficient( 3, value, regression, variance ); }
    void set_b20( const double value, const double regression = 0.0, const double variance = 0.0 )
    { this->set_coefficient( 4, value, regression, variance ); }
    void set_b11( const double value, const double regression = 0.0, const double variance = 0.0 )
    { this->set_coefficient( 5, value, regression, variance ); }
    void set_b00( const double arr[3] ) { this->set_b00( arr[0], arr[1], arr[2] ); }
    void set_b01( const double arr[3] ) { this->set_b01( arr[0], arr[1], arr[2] ); }
    void set_b10( const double arr[3] ) { this->set_b10( arr[0], arr[1], arr[2] ); }
    void set_b02( const double arr[3] ) { this->set_b02( arr[0], arr[1], arr[2] ); }
    void set_b20( const double arr[3] ) { this->set_b20( arr[0], arr[1], arr[2] ); }
    void set_b11( const double arr[3] ) { this->set_b11( arr[0], arr[1], arr[2] ); }
    void set_b00( const std::vector< double > vec ) { this->set_b00( vec[0], vec[1], vec[2] ); }
    void set_b01( const std::vector< double > vec ) { this->set_b01( vec[0], vec[1], vec[2] ); }
    void set_b10( const std::vector< double > vec ) { this->set_b10( vec[0], vec[1], vec[2] ); }
    void set_b02( const std::vector< double > vec ) { this->set_b02( vec[0], vec[1], vec[2] ); }
    void set_b20( const std::vector< double > vec ) { this->set_b20( vec[0], vec[1], vec[2] ); }
    void set_b11( const std::vector< double > vec ) { this->set_b11( vec[0], vec[1], vec[2] ); }

    /**
     * Returns the trend as a string representation
     */
    std::string to_string();

  private:
    /**
     * Sets up the distributions used for each of the 6 constants based on the current base value,
     * regression coefficient, residual variance and regression factor
     */
    void initialize_distributions();

    /**
     * The regression factor defining how much the regression affects each coefficient
     */
    double regression_factor;

    /**
     * The base value, regression coefficient, residual variance and current value of the 6 constants
     */
    double b[6][4];

    /**
     * The distribution used when determining the coefficient values by linear regression
     */
    distribution dist[6];
  };
}

/** @} end of doxygen group */

#endif
