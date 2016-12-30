/*=========================================================================

  Program:  sampsim
  Module:   tile.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_tile_h
#define __sampsim_tile_h

#include "model_object.h"

#include "distribution.h"
#include "utilities.h"

namespace Json { class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class building;
  class population;
  class town;

  /**
   * @class tile
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief A tile which belongs to a town and contains buildings
   * @details
   * Populations are organized into a tree such that all nodes are responsible for creating,
   * generating and deleting their children.  The structure is as follows:
   * - population
   *   + list of towns in population
   *     - list of n by m tiles
   *       + list of buildings in tile
   *         - list of households in building
   *           + list of individuals belonging to household
   * 
   * Tiles are divided into a grid of n by m square tiles of equal width.  They are indexed by their
   * longitudinal and latitudinal position (X and Y 0-based index).  The number of tiles in both
   * directions and the width of all tiles is defined by the population.
   */
    class tile : public model_object
  {
    friend class town;

  public:
    /**
     * Constructor
     * 
     * When a tile is created its parent town and X-Y index must be defined.
     */
    tile( town *parent, const std::pair< unsigned int, unsigned int > index );

    /**
     * Destructor
     * 
     * Deletes all buildings found within this tile.
     */
    ~tile();

    // defining pure abstract methods
    std::string get_name() const { return "tile"; }
    void copy( const base_object* o ) { this->copy( static_cast<const tile*>( o ) ); }
    void copy( const tile* );
    void from_json( const Json::Value& );
    void to_json( Json::Value& ) const;
    void to_csv( std::ostream&, std::ostream& ) const;
    unsigned int get_number_of_individuals() const { return this->number_of_individuals; }
    std::vector< std::pair<unsigned int, unsigned int> >count_individuals() const;
    void select();
    void unselect();

    /**
     * Iterator access to child buildings
     * 
     * These methods provide iterator access to the list of buildings belonging to this tile.
     */
    building_list_type::iterator get_building_list_begin()
    { return this->building_list.begin(); }

    /**
     * Iterator access to child buildings
     * 
     * These methods provide iterator access to the list of buildings belonging to this tile.
     */
    building_list_type::iterator get_building_list_end()
    { return this->building_list.end(); }

    /**
     * Constant iterator access to child buildings
     * 
     * These methods provide constant iterator access to the list of buildings belonging to this tile.
     */
    building_list_type::const_iterator get_building_list_cbegin() const
    { return this->building_list.cbegin(); }

    /**
     * Constant iterator access to child buildings
     * 
     * These methods provide constant iterator access to the list of buildings belonging to this tile.
     */
    building_list_type::const_iterator get_building_list_cend() const
    { return this->building_list.cend(); }

    /**
     * Returns the town that the tile belongs to
     */
    town* get_town() const { return this->parent; }

    /**
     * Returns the population that the tile belongs to
     */
    population* get_population() const;

    /**
     * Returns the tile's mean income
     */
    double get_mean_income() const { return this->mean_income; }

    /**
     * Sets the tile's mean income
     */
    void set_mean_income( const double mean ) { this->mean_income = mean; }

    /**
     * Returns the standard deviation of the tile's income
     */
    double get_sd_income() const { return this->sd_income; }

    /**
     * Sets the standard deviation of the tile's income
     */
    void set_sd_income( const double sd ) { this->sd_income = sd; }

    /**
     * Returns the tile's mean disease risk factor
     */
    double get_mean_disease() const { return this->mean_disease; }

    /**
     * Sets the tile's mean disease risk factor
     */
    void set_mean_disease( const double mean ) { this->mean_disease = mean; }

    /**
     * Returns the standard deviation of the tile's disease risk factor
     */
    double get_sd_disease() const { return this->sd_disease; }

    /**
     * Sets the standard deviation of the tile's disease risk factor
     */
    void set_sd_disease( const double sd ) { this->sd_disease = sd; }

    /**
     * Sets the tile's population density (in individuals per square kilometer)
     */
    void set_population_density( const double population_density )
    { this->population_density = population_density; }

    /**
     * Returns a coordinate at the centre of this tile
     * 
     * This coordinate is determined by the index passed to the constructor.
     */
    coordinate get_centroid() const { return this->centroid; }

    /**
     * Returns a pair of coordinates defining the lower and upper extent of the tile
     * 
     * These coordinates are determined by the index passed to the constructor.
     */
    std::pair< coordinate, coordinate > get_extent() const { return this->extent; }

    /**
     * Returns the surface area of the tile
     * 
     * The surface area is determined by the square of the tile's width.  All tiles in a town are the
     * same size and so have the same surface area.
     */
    double get_area() const;

    /**
     * Returns a reference to the tile's income distribution
     */
    distribution* get_income_distribution() { return &( this->income_distribution ); }

    /**
     * Returns a reference to the tile's disease risk distribution
     */
    distribution* get_disease_risk_distribution() { return &( this->disease_risk_distribution ); }

    /**
     * Determines whether the tile has a river crossing through its boundaries
     */
    bool get_has_river();

  protected:
    void create();
    void define();

  private:
    /**
     * Sets the tile's 2D index within the town
     * 
     * This method will also set the tile's extent and centroid.  It's called by the constructor
     * using the passed index.
     */
    void set_index( const std::pair< unsigned int, unsigned int > index );

    /**
     * A reference to the town that the tile belongs to (not reference counted)
     */
    town *parent;

    /**
     * The tile's X-Y coordinate within its parent town
     */
    std::pair< unsigned int, unsigned int > index;

    /**
     * The tile's mean income
     */
    double mean_income;

    /**
     * The standard deviation of the tile's income
     */
    double sd_income;

    /**
     * The tile's mean disease risk factor
     */
    double mean_disease;

    /**
     * The standard deviation of the tile's disease risk factor
     */
    double sd_disease;

    /**
     * The tile's population density in individuals per square kilometer
     */
    double population_density;

    /**
     * A coordinate pair of the tile's lower and upper bounds
     */
    std::pair< coordinate, coordinate > extent;

    /**
     * A coordinate of the centre of the tile
     */
    coordinate centroid;

    /**
     * A container holding all buildings belonging to this tile.  The tile is responsibe for managing
     * the memory needed for all of its child buildings.
     */
    building_list_type building_list;

    /**
     * The tile's income distribution
     */
    distribution income_distribution;

    /**
     * The tile's disease risk distribution
     */
    distribution disease_risk_distribution;

    /**
     * A cache of whether this tile has a river running through it
     */
    bool has_river;

    /**
     * Whether the has_river cache has been determined yet
     */
    bool has_river_cached;

    /**
     * The number of individuals in this tile.  This is determined the first time that count_individuals()
     * is called (and used as a cache for performance reasons).
     */
    unsigned int number_of_individuals;
  };
}

/** @} end of doxygen group */

#endif
