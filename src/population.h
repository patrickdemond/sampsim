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

  // tile list type
  typedef std::map< std::pair< int, int >, tile* > tile_list_type;

  class population : public base_object
  {
  public:
    population();
    ~population();

    /**
     * Iterator access
     */
    tile_list_type::iterator get_tile_list_begin()
    { return this->tile_list.begin(); }
    tile_list_type::iterator get_tile_list_end()
    { return this->tile_list.end(); }
    tile_list_type::const_iterator get_tile_list_cbegin() const
    { return this->tile_list.cbegin(); }
    tile_list_type::const_iterator get_tile_list_cend() const
    { return this->tile_list.cend(); }

    /**
     * Generate the sample by generating all tiles in the population
     */
    void generate();
    void write( const std::string filename, const bool flat_file = false ) const;

    void set_seed( const std::string seed );
    void set_number_tiles_x( const int );
    void set_number_tiles_y( const int );
    double get_tile_width() const { return this->tile_width; }
    void set_tile_width( const double );
    void set_disease_weights(
      const double population,
      const double income,
      const double risk,
      const double age,
      const double sex )
    {
      this->disease_weights[0] = population;
      this->disease_weights[1] = income;
      this->disease_weights[2] = risk;
      this->disease_weights[3] = age;
      this->disease_weights[4] = sex;
    }
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

    virtual void to_json( Json::Value& ) const;
    virtual void to_csv( std::ofstream&, std::ofstream& ) const;

  protected:

  private:
    static const unsigned int number_of_weights = 5;
    std::string seed;
    int number_tiles_x;
    int number_tiles_y;
    double tile_width;
    double disease_weights[number_of_weights];
    double mean_household_population;
    trend *mean_income;
    trend *sd_income;
    trend *mean_disease;
    trend *sd_disease;
    trend *population_density;
    tile_list_type tile_list;
  };
}

/** @} end of doxygen group */

#endif
