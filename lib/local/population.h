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

#ifndef __sampsim_population_h
#define __sampsim_population_h

#include "model_object.h"

#include "coordinate.h"
#include "distribution.h"
#include "utilities.h"

namespace Json{ class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class tile;
  class trend;

  class population : public model_object
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
    bool read( const std::string filename );
    void write( const std::string filename, const bool flat_file = false ) const;

    bool get_sample_mode() { return this->sample_mode; }
    void set_sample_mode( bool sample_mode ) { this->sample_mode = sample_mode; }
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
    trend* get_mean_income() { return this->mean_income; }
    trend* get_sd_income() { return this->sd_income; }
    void set_income( const trend *mean, const trend *sd );
    trend* get_mean_disease() { return this->mean_disease; }
    trend* get_sd_disease() { return this->sd_disease; }
    void set_disease( const trend *mean, const trend *sd );
    trend* get_population_density() { return this->population_density; }
    void set_population_density( const trend *population_density );

    /**
     * Counts the total population of the population
     */
    int count_population() const;

    /**
     * Determines the centroid and surface area of the population
     */
    coordinate get_centroid() const;
    double get_area() const;

    virtual void from_json( const Json::Value& );
    virtual void to_json( Json::Value& ) const;
    virtual void to_csv( std::ostream&, std::ostream& ) const;

    distribution* get_population_distribution() { return &( this->population_distribution ); }

  protected:

  private:
    static const unsigned int NUMBER_OF_WEIGHTS = 5;

    bool sample_mode;
    std::string seed;
    int number_tiles_x;
    int number_tiles_y;
    double tile_width;
    double disease_weights[NUMBER_OF_WEIGHTS];
    double mean_household_population;
    trend *mean_income;
    trend *sd_income;
    trend *mean_disease;
    trend *sd_disease;
    trend *population_density;
    tile_list_type tile_list;

    // distributions
    distribution population_distribution;
  };
}

/** @} end of doxygen group */

#endif
