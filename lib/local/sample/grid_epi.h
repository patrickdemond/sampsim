/*=========================================================================

  Program:  sampsim
  Module:   grid_epi.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_sample_grid_epi_h
#define __sampsim_sample_grid_epi_h

#include "sample/epi.h"

#include <list>

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

namespace sample
{
  /**
   * @class grid_epi
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief EPI method which picks the initial building using a grid
   * @details
   * A contrete implementation of the sample class.  This EPI method uses a grid to determine
   * which buildings are considered to be inside the list of buildings defined by the initial
   * starting angle.
   */
  class grid_epi : public epi
  {
  public:
    /**
     * Constructor
     */
    grid_epi() : number_of_squares( 0 ), square_width_x( 0 ), square_width_y( 0 ) {}

    // defining pure abstract methods
    std::string get_name() const { return "grid_epi"; }
    void copy( const base_object* o ) { this->copy( static_cast<const grid_epi*>( o ) ); }
    void copy( const grid_epi* );
    void from_json( const Json::Value& );
    void to_json( Json::Value& ) const;

    /**
     * Returns the name of the sampling method
     */
    virtual std::string get_type() const { return "grid EPI"; }

    /**
     * Sets the number of squares (per direction) to divide towns into
     */
    void set_number_of_squares( unsigned int number_of_squares );

    /**
     * Returns the number of squares (per direction) to divide towns into
     */
    unsigned int get_number_of_squares() { return this->number_of_squares; }

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
    virtual building* select_next_building( sampsim::building_tree& );

    /**
     * Determine the initial list of buildings to choose from
     */
    virtual void determine_initial_building_list( sampsim::building_tree& );

    /**
     * Determines the x and y width of the squares to use based on a town's dimensions
     * and the number of squares requested.
     */
    virtual void determine_square_widths( sampsim::town *town );

  private:
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
  };
}

/** @} end of doxygen group */

}

/** @} end of doxygen group */

#endif
