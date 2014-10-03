/*=========================================================================

  Program:  sampsim
  Module:   line.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_line_h
#define __sampsim_line_h

#include "base_object.h"

#include <cmath>
#include "coordinate.h"

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
    
    /**
     * Returns the name of the object's class
     */
    std::string get_name() const { return "line"; }

    /**
     * Comparison operator
     * 
     * Returns true if the two lines have the same x, y and centroid
     */
    bool operator == ( const line l ) const
    {
      // the point may be on two different parts of the line
      return ( this->angle == l.angle ) &&
             ( tan( this->angle ) ==
               ( ( l.intercept.x - this->intercept.x ) / ( l.intercept.y - this->intercept.y ) ) );
    }

    /**
     * Comparison operator
     * 
     * Returns true if the two lines do not have the same x, y or centroid
     */
    bool operator != ( const line l ) const { return !( *this == l ); }

    /**
     * Deserialize the line
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void from_json( const Json::Value& );

    /**
     * Serialize the line
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void to_json( Json::Value& ) const;

    /**
     * Output the line to two CSV files (households and individuals)
     * 
     * All objects must provide an implementation for printing to a CSV (comma separated value) format.
     * Two streams are expected, the first is for household data and the second for individual data.
     */
    virtual void to_csv( std::ostream&, std::ostream& ) const {}

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
    bool line_inside_bounds( std::pair< coordinate, coordinate > extent );

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
