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

#ifndef __sampsim_tile_h
#define __sampsim_tile_h

#include "base_object.h"

#include "coordinate.h"
#include "distribution.h"
#include "utilities.h"

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

  class tile : public base_object
  {
  public:
    tile( population *parent, const std::pair< int, int > index );
    ~tile();

    /**
     * Iterator access
     */
    building_list_type::iterator get_building_list_begin()
    { return this->building_list.begin(); }
    building_list_type::iterator get_building_list_end()
    { return this->building_list.end(); }
    building_list_type::const_iterator get_building_list_cbegin() const
    { return this->building_list.cbegin(); }
    building_list_type::const_iterator get_building_list_cend() const
    { return this->building_list.cend(); }

    /**
     * Lineage methods
     */
    population* get_population() const { return this->parent; }

    /**
     * Generate the tile by generating all buildings in the tile
     */
    void generate();
    virtual void from_json( const Json::Value& );
    virtual void to_json( Json::Value& ) const;
    virtual void to_csv( std::ostream&, std::ostream& ) const;

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

    distribution* get_income_distribution() { return &( this->income_distribution ); }
    distribution* get_disease_risk_distribution() { return &( this->disease_risk_distribution ); }

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

    // distributions
    distribution income_distribution;
    distribution disease_risk_distribution;
  };
}

/** @} end of doxygen group */

#endif
