/*=========================================================================

  Program:  sampsim
  Module:   simulation.h
  Language: C++

=========================================================================*/

/**
 * @class simulation
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief A simulation which contains X by Y tiles
 */

#ifndef __simulation_h
#define __simulation_h

#include "base_object.h"

#include <map>
#include <string>

namespace Json{ class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class tile;
  class trend;

  class simulation : public base_object
  {
  public:
    simulation();
    ~simulation();

    /**
     * Generate the sample by generating all tiles in the simulation
     */
    void generate();
    void write( std::string filename, bool flat_file = false );

    void set_number_tiles_x( const int );
    void set_number_tiles_y( const int );
    double get_tile_width() const { return this->tile_width; }
    void set_tile_width( const double );
    double get_mean_household_population() const { return this->mean_household_population; }
    void set_mean_household_population( const double );
    void set_income( const trend *mean, const trend *sd );
    void set_population_density( const trend *population_density );

    /**
     * Counts the total population of the simulation
     */
    int get_population() const;

    /**
     * Determines the surface area of the simulation
     */
    double get_area() const;

    virtual void to_json( Json::Value& );
    virtual void to_csv( std::ofstream&, std::ofstream& );

  protected:

  private:
    int number_tiles_x;
    int number_tiles_y;
    double tile_width;
    double mean_household_population;
    trend *mean_income;
    trend *sd_income;
    trend *population_density;
    std::map< std::pair< int, int >, tile* > tile_list;
  };
}

/** @} end of doxygen group */

#endif
