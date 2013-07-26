/*=========================================================================

  Program:  sampsim
  Module:   coordinate.h
  Language: C++

=========================================================================*/

/**
 * @class coordinate
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief A coordinate pair
 */

#ifndef __coordinate_h
#define __coordinate_h

#include "base_object.h"

#include <cmath>

namespace Json{ class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class coordinate : public base_object
  {
  public:
    coordinate( const double x = 0, const double y = 0 ) : x(x), y(y) {}
    ~coordinate() {}

    bool operator == ( const coordinate c ) const { return this->x == c.x && this->y == c.y; }
    bool operator != ( const coordinate c ) const { return !( *this == c ); }
    coordinate operator + ( const coordinate c ) const { return coordinate( this->x + c.x, this->y + c.y ); }
    coordinate operator - ( const coordinate c ) const { return coordinate( this->x - c.x, this->y - c.y ); }
    coordinate operator * ( const double a ) const { return coordinate( this->x * a, this->y * a ); }
    coordinate operator / ( const double a ) const { return coordinate( this->x / a, this->y / a ); }
    coordinate& operator += ( const coordinate &c ) { this->x += c.x; this->y += c.y; return (*this); }
    coordinate& operator -= ( const coordinate &c ) { this->x -= c.x; this->y -= c.y; return (*this); }
    coordinate& operator *= ( const double a ) { this->x *= a; this->y *= a; return (*this); }
    coordinate& operator /= ( const double a ) { this->x /= a; this->y /= a; return (*this); }
    double length() const { return sqrt( this->x*this->x + this->y+this->y ); }
    double distance( const coordinate c ) const { return ( *this - c ).length(); }

    virtual void to_json( Json::Value& ) const;
    virtual void to_csv( std::ofstream& ) const;

    double x;
    double y;

  protected:

  private:
  };
}

/** @} end of doxygen group */

#endif
