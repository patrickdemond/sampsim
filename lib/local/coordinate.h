/*=========================================================================

  Program:  sampsim
  Module:   coordinate.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_coordinate_h
#define __sampsim_coordinate_h

#include "base_object.h"

namespace Json{ class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  /**
   * 
   */
  class coordinate : public base_object
  {
  public:
    /**
     * 
     */
    coordinate( const double x = 0, const double y = 0 ) : x(x), y(y), cx(0), cy(0) {}

    /**
     * 
     */
    bool operator == ( const coordinate c ) const { return this->x == c.x && this->y == c.y; }

    /**
     * 
     */
    bool operator != ( const coordinate c ) const { return !( *this == c ); }

    /**
     * 
     */
    coordinate operator + ( const coordinate c ) const { return coordinate( this->x + c.x, this->y + c.y ); }

    /**
     * 
     */
    coordinate operator - ( const coordinate c ) const { return coordinate( this->x - c.x, this->y - c.y ); }

    /**
     * 
     */
    coordinate operator * ( const double a ) const { return coordinate( this->x * a, this->y * a ); }

    /**
     * 
     */
    coordinate operator / ( const double a ) const { return coordinate( this->x / a, this->y / a ); }

    /**
     * 
     */
    coordinate& operator += ( const coordinate &c ) { this->x += c.x; this->y += c.y; return (*this); }

    /**
     * 
     */
    coordinate& operator -= ( const coordinate &c ) { this->x -= c.x; this->y -= c.y; return (*this); }

    /**
     * 
     */
    coordinate& operator *= ( const double a ) { this->x *= a; this->y *= a; return (*this); }

    /**
     * 
     */
    coordinate& operator /= ( const double a ) { this->x /= a; this->y /= a; return (*this); }

    /**
     * 
     */
    virtual void from_json( const Json::Value& );

    /**
     * 
     */
    virtual void to_json( Json::Value& ) const;

    /**
     * 
     */
    virtual void to_csv( std::ostream&, std::ostream& ) const;

    /**
     * 
     */
    double distance( const coordinate c ) const;

    /**
     * 
     */
    double length() const { return this->distance( coordinate( this->cx, this->cy ) ); }

    /**
     * 
     */
    coordinate get_centroid() { return coordinate( this->cx, this->cy ); }

    /**
     * 
     */
    void set_centroid( coordinate c ) { this->cx = c.x; this->cy = c.y; }

    /**
     * 
     */
    double get_r() const { return this->length(); }

    /**
     * 
     */
    double get_a() const;

    /**
     * 
     */
    double x;

    /**
     * 
     */
    double y;

    /**
     * 
     */
    double cx;

    /**
     * 
     */
    double cy;
  };
}

/** @} end of doxygen group */

#endif
