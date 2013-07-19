/*=========================================================================

  Program:  sampsim
  Module:   population.h
  Language: C++

=========================================================================*/

/**
 * @class population
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief A population which contains X by Y tiles
 */

#ifndef __population_h
#define __population_h

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

  class population : public base_object
  {
  public:
    population();
    ~population();

    /**
     * Generate the sample by generating all tiles in the population
     */
    void generate();
    void write( std::string filename, bool flat_file = false );

    void set_seed( std::string seed );
    void set_number_tiles_x( const int );
    void set_number_tiles_y( const int );
    double get_tile_width() const { return this->tile_width; }
    void set_tile_width( const double );
    double get_mean_household_population() const { return this->mean_household_population; }
    void set_mean_household_population( const double );
    void set_income( const trend *mean, const trend *sd );
    void set_disease( const trend *mean, const trend *sd );
    void set_population_density( const trend *population_density );

    /**
     * Counts the total population of the population
     */
    int count_population() const;

    /**
     * Determines the surface area of the population
     */
    double get_area() const;

    virtual void to_json( Json::Value& );
    virtual void to_csv( std::ofstream&, std::ofstream& );

  protected:

  private:
    std::string seed;
    int number_tiles_x;
    int number_tiles_y;
    double tile_width;
    double mean_household_population;
    trend *mean_income;
    trend *sd_income;
    trend *mean_disease;
    trend *sd_disease;
    trend *population_density;
    std::map< std::pair< int, int >, tile* > tile_list;
  };
}

/** @} end of doxygen group */

#endif
