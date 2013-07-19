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
 * @brief A tile which belongs to a simulation and contains buildings
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
  class simulation;

  class tile : public base_object
  {
  public:
    tile( simulation *parent, std::pair< int, int > index );
    ~tile();
    simulation* get_parent() { return this->parent; }

    /**
     * Generate the tile by generating all buildings in the tile
     */
    void generate();
    virtual void to_json( Json::Value& );

    void set_income( std::pair< double, double > income ) { this->income = income; }
    void set_income( double mean, double sd )
    { this->set_income( std::pair< double, double >( mean, sd ) ); }
    void set_population_density( double population_density )
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
    int get_population() const;

    /**
     * Determines the surface area of the tile
     */
    double get_area() const;

  protected:
    /**
     * Sets the tile's 2D index within the simulation.
     * This method will also set the tile's extent and centroid.
     */
    void set_index( const std::pair< int, int > index );

  private:
    simulation *parent;
    std::pair< int, int > index;
    std::pair< double, double > income;
    double population_density;
    std::pair< coordinate, coordinate > extent;
    coordinate centroid;

    std::vector< building* > building_list;
  };
}

/** @} end of doxygen group */

#endif
