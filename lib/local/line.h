/*=========================================================================

  Program:  sampsim
  Module:   line.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_line_h
#define __sampsim_line_h

#include "base_object.h"

#include "coordinate.h"
#include "utilities.h"

#include <cmath>

namespace Json{ class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  /**
   * @class line
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief A 2 dimensional line
   * @details
   * This class represents a 2D line.  The line is defined by an intercept and angle
   * between -pi/2 and +pi/2
   */
  class line : public base_object
  {
  public:
    /**
     * Constructor
     */
    line( coordinate i = coordinate(), double a = 0.0 ) : intercept(i), angle(a) {}
    
    // defining pure abstract methods
    std::string get_name() const { return "line"; }
    void copy( const base_object* o ) { this->copy( static_cast<const line*>( o ) ); }
    void copy( const line* );
    void from_json( const Json::Value& );
    void to_json( Json::Value& ) const;
    void to_csv( std::ostream&, std::ostream& ) const {}

    /**
     * Comparison operator
     * 
     * Returns true if the two lines have the same x, y and centroid
     */
    bool operator == ( const line l ) const
    {
      // the point may be on two different parts of the line
      return safe_equals( this->angle, l.angle ) &&
             safe_equals( tan( this->angle ),
               safe_subtract( l.intercept.x, this->intercept.x ) /
               safe_subtract( l.intercept.y, this->intercept.y ) );
    }

    /**
     * Comparison operator
     * 
     * Returns true if the two lines do not have the same x, y or centroid
     */
    bool operator != ( const line l ) const { return !( *this == l ); }

    /**
     * Returns the distance from a point to the line
     * 
     * The line is defined by providing a line that it passes through (the intercept) and
     * its angle.
     */
    double distance_from_point( coordinate point ) const;

    /**
     * Determines whether the line crosses through an extent
     */
    bool line_inside_bounds( extent_type extent );

    /**
     * Determines whether the coordinate is inside a strip formed by two lines
     */
    static bool point_inside_strip( coordinate, line, line );

    /**
     * Convenience method
     */
    static bool point_inside_strip( coordinate c, line lines[2] )
    {
      return point_inside_strip( c, lines[0], lines[1] );
    }

    /**
     * The line's intercept
     */
    coordinate intercept;

    /**
     * The line's angle (should be in -pi/2 to pi/2)
     */
    double angle;
  };
}

/** @} end of doxygen group */

#endif
