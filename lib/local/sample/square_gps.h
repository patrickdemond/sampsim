/*=========================================================================

  Program:  sampsim
  Module:   square_gps.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_sample_square_gps_h
#define __sampsim_sample_square_gps_h

#include "sample/sized_sample.h"

#include <vector>

namespace Json{ class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{

/**
 * @addtogroup sample
 * @{
 */

class town;

namespace sample
{
  /**
   * @class square_gps
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief Random sampler
   * @details
   * A concrete implementation of the sample class.  This sampler selects random "GPS"
   * positions in the town and then randomly selects one of the buildings within a square
   * that the GPS point valls into.  This proceedure is repeated until the sample
   * size is met.
   */
  class square_gps : public sized_sample
  {
  public:
    /**
     * Construction
     */
     square_gps() : number_of_squares( 0 ), square_width_x( 0 ), square_width_y( 0 ) {}

    /**
     * Returns the name of the object's class
     */
    std::string get_name() const { return "square_gps"; }

    /**
     * Returns the name of the sampling method
     */
    virtual std::string get_type() const { return "Square GPS"; }

    /**
     * Sets the number of squares (per direction) to divide towns into
     */
    void set_number_of_squares( unsigned int number_of_squares );

    /**
     * Returns the number of squares (per direction) to divide towns into
     */
    unsigned int get_number_of_squares() { return this->number_of_squares; }

    /**
     * Deserialize the sample
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    void from_json( const Json::Value& );

    /**
     * Serialize the sample
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    void to_json( Json::Value& ) const;

    /**
     * Returns the header for generated CSV files
     * 
     * The header includes all parameters used by the sampling method
     */
    virtual std::string get_csv_header() const;

  protected:
    /**
     * Algorithm which selects buildings based on the sampling method
     */
    virtual building* select_next_building( building_tree& );

    /**
     * Determines the x and y width of the squares to use based on a town's dimensions
     * and the number of squares requested.
     */
    virtual void determine_square_widths( sampsim::town *town );

  private:
    /**
     * Resets the internal array which tracks which squares have been selected
     */
    void reset_selected_squares();

    /**
     * Marks a square as selected
     * This method returns whether the square has already been selected.
     */
    bool select_square( const unsigned int index_x, const unsigned int index_y );

    /**
     * Returns whether all squares have been selected
     */
    bool all_squares_selected() const;

    /**
     * The number of squares (per direction) to divide towns into
     */
    unsigned int number_of_squares;

    /**
     * The width of squares in the x plane as determined by the town's dimensions
     * and the number of squares requested.
     */
    double square_width_x;

    /**
     * The width of squares in the y plane as determined by the town's dimensions
     * and the number of squares requested.
     */
    double square_width_y;

    /**
     * Array of all squares which have been selected
     */
    std::vector< std::vector< bool > > selected_squares;
  };
}

/** @} end of doxygen group */

}

/** @} end of doxygen group */

#endif
