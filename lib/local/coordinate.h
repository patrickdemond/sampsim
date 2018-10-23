/*=========================================================================

  Program:  sampsim
  Module:   coordinate.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_coordinate_h
#define __sampsim_coordinate_h

#include "base_object.h"

#include <iostream>

namespace Json{ class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  /**
   * @class coordinate
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief A 2 dimensional coordinate
   * @details
   * This class represents a 2D coordinate point with some common vector operations.  Polar
   * coordinates are also supported by assuming a centre at the origin (0, 0).  It is also
   * possible to change the origin to any x-y coordinate.
   */
  class coordinate : public base_object
  {
  public:
    /**
     * Constructor
     */
    coordinate( const double x = 0, const double y = 0 ) : x(x), y(y), cx(0), cy(0) {}
    
    // defining pure abstract methods
    std::string get_name() const { return "coordinate"; }
    void copy( const base_object* o ) { this->copy( static_cast<const coordinate*>( o ) ); }
    void copy( const coordinate* );
    void from_json( const Json::Value& );
    void to_json( Json::Value& ) const;
    void to_csv( std::ostream&, std::ostream& ) const;

    /**
     * Comparison operator
     * 
     * Returns true if the two coordinates have the same x, y and centroid
     */
    bool operator == ( const coordinate c ) const;

    /**
     * Comparison operator
     * 
     * Returns true if the two coordinates do not have the same x, y or centroid
     */
    bool operator != ( const coordinate c ) const { return !( *this == c ); }

    /**
     * Addition operator
     * 
     * Vector addition of two coordinates.  For example: (x, y) = (x1 + x2, y1 + y2)
     */
    coordinate operator + ( const coordinate c ) const { return coordinate( this->x + c.x, this->y + c.y ); }

    /**
     * Subtraction operator
     * 
     * Vector subtraction of two coordinates.  For example: (x, y) = (x1 - x2, y1 - y2)
     */
    coordinate operator - ( const coordinate c ) const;

    /**
     * Linear multiplication operator
     * 
     * Multiplies a coordinate by a scalar value.  For example: (x, y) = (a*x0, a*y0)
     */
    coordinate operator * ( const double a ) const { return coordinate( this->x * a, this->y * a ); }

    /**
     * Linear division operator
     * 
     * Divides a coordinate by a scalar value.  For example: (x, y) = (x0/a, y0/a)
     */
    coordinate operator / ( const double a ) const { return coordinate( this->x / a, this->y / a ); }

    /**
     * Compound addition operator
     * 
     * Vector addition of a coordinate to an existing coordinate.  For example: (x, y) = (x1 + x2, y1 + y2)
     */
    coordinate& operator += ( const coordinate &c ) { this->x += c.x; this->y += c.y; return (*this); }

    /**
     * Compound subtraction operator
     * 
     * Vector subtraction of a coordinate from an existing coordinate.
     * For example: (x, y) = (x1 - x2, y1 - y2)
     */
    coordinate& operator -= ( const coordinate &c ) { this->x -= c.x; this->y -= c.y; return (*this); }

    /**
     * Compound linear multiplication operator
     * 
     * Multiplies an existing coordinate by a scalar value.  For example: (x, y) = (a*x0, a*y0)
     */
    coordinate& operator *= ( const double a ) { this->x *= a; this->y *= a; return (*this); }

    /**
     * Compound linear division operator
     * 
     * Divides an existing coordinate by a scalar value.  For example: (x, y) = (x0/a, y0/a)
     */
    coordinate& operator /= ( const double a ) { this->x /= a; this->y /= a; return (*this); }

    /**
     * Stream operator
     */
    friend std::ostream& operator << ( std::ostream& os, const coordinate &c )
    {
      os << '(' << c.x << ',' << c.y << ')';
      return os;
    }

    /**
     * Returns the scalar distance to another coordinate
     */
    double distance( const coordinate c ) const;

    /**
     * Returns the scalar squared distance to another coordinate
     * 
     * This method is faster than distance() so it should be used where comparing relative distances
     * is needed but absolute distance measurements isn't required.
     */
    double squared_distance( const coordinate c ) const;

    /**
     * Returns the length of the coordinate
     * 
     * The length of a coordinate is based on the coordinate's centroid.  This is equivalent to calling
     * distance( centroid ).
     */
    double length() const { return this->distance( coordinate( this->cx, this->cy ) ); }

    /**
     * Returns a coordinate of the centroid of this coordinate
     */
    coordinate get_centroid() { return coordinate( this->cx, this->cy ); }

    /**
     * Changes the location of the coordinate's centroid
     * 
     * Once changing the centroid the values returned by length(), get_r() and get_a() will change based
     * on the centroid's new position.
     */
    void set_centroid( coordinate c ) { this->cx = c.x; this->cy = c.y; }

    /**
     * Returns the radius portion of the coordinate in polar coordinates
     * 
     * This value depends on the coordinate's centroid.
     */
    double get_r() const { return this->length(); }

    /**
     * Returns the angle portion of the coordinate in polar coordinates
     * 
     * This value depends on the coordinate's centroid.
     */
    double get_a() const;

    /**
     * The coordinate's x value
     */
    double x;

    /**
     * The coordinate's y value
     */
    double y;

    /**
     * The coordinate's centroid's x value
     */
    double cx;

    /**
     * The coordinate's centroid's y value
     */
    double cy;
  };
}

/** @} end of doxygen group */

#endif
