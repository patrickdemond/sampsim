/*=========================================================================

  Program:  sampsim
  Module:   trend.h
  Language: C++

=========================================================================*/

/**
 * @class trend
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief A generic object defining an algebraic trend of the form:
 *        b00 + b01 X + b10 Y + b02 X^2 + b20 Y^2 + b11 XY
 */

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

  class trend : public base_object
  {
  public:
    trend(
      const double b00 = 1,
      const double b01 = 0,
      const double b10 = 0,
      const double b02 = 0,
      const double b20 = 0,
      const double b11 = 0 );
    ~trend() {}

    void copy( const trend* );

    double get_b00() const { return this->b00; }
    void set_b00( const double b00 ) { this->b00 = b00; }
    double get_b01() const { return this->b01; }
    void set_b01( const double b01 ) { this->b01 = b01; }
    double get_b10() const { return this->b10; }
    void set_b10( const double b10 ) { this->b10 = b10; }
    double get_b02() const { return this->b02; }
    void set_b02( const double b02 ) { this->b02 = b02; }
    double get_b20() const { return this->b20; }
    void set_b20( const double b20 ) { this->b20 = b20; }
    double get_b11() const { return this->b11; }
    void set_b11( const double b11 ) { this->b11 = b11; }

    /**
     * Returns the value of the trend at a particular coordinate
     */
    double get_value( const coordinate );

    std::string to_string() const;
    virtual void from_json( const Json::Value& );
    virtual void to_json( Json::Value& ) const;
    virtual void to_csv( std::ostream&, std::ostream& ) const;

  protected:

  private:
    double b00;
    double b01;
    double b10;
    double b02;
    double b20;
    double b11;
  };
}

/** @} end of doxygen group */

#endif
