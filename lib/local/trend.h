/*=========================================================================

  Program:  sampsim
  Module:   trend.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_trend_h
#define __sampsim_trend_h

#include "base_object.h"

#include "coordinate.h"

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
   *   - b00 + b01 X + b10 Y + b02 X^2 + b20 Y^2 + b11 XY
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

    /**
     * Copies another trend's values into the current object
     */
    void copy( const trend* );

    /**
     * Returns the b00 constant
     */
    double get_b00() const { return this->b00; }

    /**
     * Sets the b00 constant
     */
    void set_b00( const double b00 ) { this->b00 = b00; }

    /**
     * Returns the b01 contant
     */
    double get_b01() const { return this->b01; }

    /**
     * Sets the b01 constant
     */
    void set_b01( const double b01 ) { this->b01 = b01; }

    /**
     * Returns the b10 constant
     */
    double get_b10() const { return this->b10; }

    /**
     * Sets the b10 constant
     */
    void set_b10( const double b10 ) { this->b10 = b10; }

    /**
     * Returns the b02 constant
     */
    double get_b02() const { return this->b02; }

    /**
     * Sets the b02 constant
     */
    void set_b02( const double b02 ) { this->b02 = b02; }

    /**
     * Returns the b20 constant
     */
    double get_b20() const { return this->b20; }

    /**
     * Sets the b20 constant
     */
    void set_b20( const double b20 ) { this->b20 = b20; }

    /**
     * Returns the b11 constant
     */
    double get_b11() const { return this->b11; }

    /**
     * Sets the b11 constant
     */
    void set_b11( const double b11 ) { this->b11 = b11; }

    /**
     * Returns the value of the trend at a particular coordinate
     */
    double get_value( const coordinate );

    /**
     * Returns the trend as a string representation
     */
    std::string to_string() const;

    /**
     * Deserialize the population
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void from_json( const Json::Value& );

    /**
     * Serialize the population
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void to_json( Json::Value& ) const;

    /**
     * Output the population to two CSV files (households and individuals)
     * 
     * All objects must provide an implementation for printing to a CSV (comma separated value) format.
     * Two streams are expected, the first is for household data and the second for individual data.
     */
    virtual void to_csv( std::ostream&, std::ostream& ) const;

  private:
    /**
     * The value of the b00 constant
     */
    double b00;

    /**
     * The value of the b01 constant
     */
    double b01;

    /**
     * The value of the b10 constant
     */
    double b10;

    /**
     * The value of the b02 constant
     */
    double b02;

    /**
     * The value of the b20 constant
     */
    double b20;

    /**
     * The value of the b11 constant
     */
    double b11;
  };
}

/** @} end of doxygen group */

#endif
