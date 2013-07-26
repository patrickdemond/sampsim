/*=========================================================================

  Program:  sampsim
  Module:   tile.h
  Language: C++

=========================================================================*/

/**
 * @class tile
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief A tile which belongs to a population and contains buildings
 */

#ifndef __tile_h
#define __tile_h

#include "base_object.h"

#include "coordinate.h"

#include <vector>

namespace Json { class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class building;
  class population;

  // building list type
  typedef std::vector< building* > building_list_type;

  class tile : public base_object
  {
  public:
    tile( population *parent, const std::pair< int, int > index );
    ~tile();

    /**
     * Lineage methods
     */
    population* get_population() const { return this->parent; }

    /**
     * Generate the tile by generating all buildings in the tile
     */
    void generate();
    virtual void to_json( Json::Value& ) const;
    virtual void to_csv( std::ofstream&, std::ofstream& ) const;

    double get_mean_income() const { return this->mean_income; }
    void set_mean_income( const double mean ) { this->mean_income = mean; }
    double get_sd_income() const { return this->sd_income; }
    void set_sd_income( const double sd ) { this->sd_income = sd; }

    double get_mean_disease() const { return this->mean_disease; }
    void set_mean_disease( const double mean ) { this->mean_disease = mean; }
    double get_sd_disease() const { return this->sd_disease; }
    void set_sd_disease( const double sd ) { this->sd_disease = sd; }

    void set_population_density( const double population_density )
    { this->population_density = population_density; }

    /**
     * Determines the tile's centroid coordinate based on its index and width
     */
    coordinate get_centroid() const { return this->centroid; }

    /**
     * Determines the tile's extent based on its index and width
     */
    std::pair< coordinate, coordinate > get_extent() const { return this->extent; }

    /**
     * Counts the total population of the tile
     */
    int count_population() const;

    /**
     * Determines the surface area of the tile
     */
    double get_area() const;

  protected:
    /**
     * Sets the tile's 2D index within the population.
     * This method will also set the tile's extent and centroid.
     */
    void set_index( const std::pair< int, int > index );

  private:
    population *parent;
    std::pair< int, int > index;
    double mean_income;
    double sd_income;
    double mean_disease;
    double sd_disease;
    double population_density;
    std::pair< coordinate, coordinate > extent;
    coordinate centroid;

    building_list_type building_list;
  };
}

/** @} end of doxygen group */

#endif
