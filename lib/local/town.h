/*=========================================================================

  Program:  sampsim
  Module:   town.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_town_h
#define __sampsim_town_h

#include "model_object.h"

#include "distribution.h"
#include "line.h"
#include "utilities.h"

namespace Json{ class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class population;
  class tile;
  class trend;

  /**
   * @class town
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief A town which contains X by Y tiles
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
   * Towns are made up of tiles.  A population has one or more towns.  When a town is selected the
   * selection state of its buildings are unchanged.  When unselecting a town all buildings contained
   * within the town are also unselected.
   */
  class town : public model_object
  {
    friend class individual;
    friend class population;

  public:
    /**
     * Constructor
     * 
     * When a town is created its parent population and index must be defined.
     */
    town(
      population *parent,
      const unsigned int index
    );

    /**
     * Destructor
     */
    ~town();

    // defining pure abstract methods
    std::string get_name() const { return "town"; }
    void copy( const base_object* o ) { this->copy( static_cast<const town*>( o ) ); }
    void copy( const town* );
    void from_json( const Json::Value& );
    void to_json( Json::Value& ) const;
    void to_csv( std::ostream&, std::ostream& ) const;
    unsigned int get_number_of_individuals() const { return this->number_of_individuals; }
    void assert_summary();
    void rebuild_summary();
    void select();
    void unselect();
    void select_all();

    /**
     * Iterator access to child tiles
     * 
     * These methods provide iterator access to the list of tiles belonging to the town.
     */
    tile_list_type::iterator get_tile_list_begin()
    { return this->tile_list.begin(); }

    /**
     * Iterator access to child tiles
     * 
     * These methods provide iterator access to the list of tiles belonging to the town.
     */
    tile_list_type::iterator get_tile_list_end()
    { return this->tile_list.end(); }

    /**
     * Constant iterator access to child tiles
     * 
     * These methods provide constant iterator access to the list of tiles belonging to the town.
     */
    tile_list_type::const_iterator get_tile_list_cbegin() const
    { return this->tile_list.cbegin(); }

    /**
     * Constant iterator access to child tiles
     * 
     * These methods provide constant iterator access to the list of tiles belonging to the town.
     */
    tile_list_type::const_iterator get_tile_list_cend() const
    { return this->tile_list.cend(); }

    /**
     * Constant iterator access to disease pocket coordinates
     * 
     * These methods provide constant iterator access to the list of disease pocket coordinates
     */
    coordinate_list_type::const_iterator get_disease_pocket_list_cbegin() const
    { return this->disease_pocket_list.cbegin(); }

    /**
     * Constant iterator access to disease pocket coordinates
     * 
     * These methods provide constant iterator access to the list of disease pocket coordinates
     */
    coordinate_list_type::const_iterator get_disease_pocket_list_cend() const
    { return this->disease_pocket_list.cend(); }

    /**
     * Returns the population that the tile belongs to
     */
    population* get_population() const { return this->parent; }

    /**
     * Returns this town's index
     */
    unsigned int get_index() const { return this->index; }

    /**
     * Returns the number of tiles in the longitudinal (X) direction
     */
    unsigned int get_number_of_tiles_x() const { return this->number_of_tiles_x; }

    /**
     * Sets the number of tiles in the longitudinal (X) direction
     */
    void set_number_of_tiles_x( const unsigned int );

    /**
     * Returns the number of tiles in the latitudinal (Y) direction
     */
    unsigned int get_number_of_tiles_y() const { return this->number_of_tiles_y; }

    /**
     * Sets the number of tiles in the latitudinal (Y) direction
     */
    void set_number_of_tiles_y( const unsigned int );

    /**
     * Returns whether the town has a river
     */
    bool get_has_river() const { return this->has_river; }

    /**
     * Sets whether the town has a river
     */
    void set_has_river( const bool );

    /**
     * Get the lines defining the river's banks
     */
    line* get_river_banks() { return this->river_banks; }

    /**
     * Returns the population's mean household population
     */
    double get_mean_household_population() const { return this->mean_household_population; }

    /**
     * Sets the population's mean household population
     */
    void set_mean_household_population( const double );

    /**
     * Get the population distribution
     */
    distribution* get_population_distribution() { return &( this->population_distribution ); }

    /**
     * Creates the provided number of disease pockets.
     * 
     * Disease pockets affect the overall chance that an individual has a disease.  The factor is
     * determined as a sum of the inverse square distance of an individual from each pocket.
     */
    void set_number_of_disease_pockets( const unsigned int );

    /**
     * Returns a reference to the trend defining the town's mean income
     */
    trend* get_mean_income() { return this->mean_income; }

    /**
     * Returns a reference to the trend defining the standard deviation of the town's income
     */
    trend* get_sd_income() { return this->sd_income; }

    /**
     * Sets the trends defining the town's mean and standard deviation income
     */
    void set_income( trend *mean, trend *sd );

    /**
     * Returns a reference to the trend defining the town's mean disease risk factor
     */
    trend* get_mean_disease() { return this->mean_disease; }

    /**
     * Returns a reference to the trend defining the standard deviation of the town's
     * disease risk factor
     */
    trend* get_sd_disease() { return this->sd_disease; }

    /**
     * Returns a reference to the trend defining the town's mean exposure risk factor
     */
    trend* get_mean_exposure() { return this->mean_exposure; }

    /**
     * Returns a reference to the trend defining the standard deviation of the town's
     * exposure risk factor
     */
    trend* get_sd_exposure() { return this->sd_exposure; }

    /**
     * Sets the trends defining the town's mean and standard deviation disease risk factor
     */
    void set_disease( trend *mean, trend *sd );

    /**
     * Sets the trends defining the town's mean and standard deviation exposure risk factor
     */
    void set_exposure( trend *mean, trend *sd );

    /**
     * Returns a reference to the trend defining the town's population density
     */
    trend* get_population_density() { return this->population_density; }

    /**
     * Returns a coordinate at the centre of the town
     * 
     * This coordinate is based on the number of tiles in the X and Y directions as well as tile width.
     */
    coordinate get_centroid() const;

    /**
     * Returns the width of the town in the X direction
     * 
     * The X width of the town is determined by the width of a tile multiplied by the number of tiles
     * in the X direction.
     */
    double get_x_width() const;

    /**
     * Returns the width of the town in the Y direction
     * 
     * The Y width of the town is determined by the width of a tile multiplied by the number of tiles
     * in the Y direction.
     */
    double get_y_width() const;

    /**
     * Returns the surface area of the town
     * 
     * The surface area is determined by the area of each tile multiplied by the number of tiles
     * in the X and Y directions.
     */
    double get_area() const;

    /**
     * Returns the number of selected individuals (a cached value)
     */
    unsigned int get_number_of_selected_individuals() const
    { return this->number_of_selected_individuals; }

    /**
     * Returns the number of selected diseased individuals (a cached value)
     */
    unsigned int get_number_of_selected_diseased_individuals( unsigned int index ) const
    { return this->number_of_selected_diseased_individuals[index]; }

  protected:
    void create();
    void define();

  private:
    /**
     * A reference to the populationn that the town belongs to (not reference counted)
     */
    population *parent;

    /**
     * The town's 0-based index within its parent population
     */
    unsigned int index;

    /**
     * The number of tiles to create in the longitudinal (X) direction
     */
    unsigned int number_of_tiles_x;

    /**
     * The number of tiles to create in the latitudinal (Y) direction
     */
    unsigned int number_of_tiles_y;

    /**
     * Whether or not the town has a river
     */
    bool has_river;

    /**
     * The lines defining the river's banks
     */
    line river_banks[2];

    /**
     * The town's mean household population (size)
     */
    double mean_household_population;

    /**
     * The population distribution to use when creating individuals within households
     */
    distribution population_distribution;

    /**
     * The trend defining the town's mean income
     * 
     * Mean income is determined by tile position according to this trend.
     */
    trend *mean_income;

    /**
     * The trend defining the standard deviation of the town's income
     * 
     * Standard deviation of income is determined by tile position according to this trend.
     */
    trend *sd_income;

    /**
     * The trend defining the town's mean disease risk factor
     * 
     * Mean disease risk factor is determined by tile position according to this trend.
     */
    trend *mean_disease;

    /**
     * The trend defining the standard deviation of the town's disease risk factor
     * 
     * Standard deviation of disease risk factor is determined by tile position according to this trend.
     */
    trend *sd_disease;

    /**
     * The trend defining the town's mean exposure risk factor
     * 
     * Mean exposure risk factor is determined by tile position according to this trend.
     */
    trend *mean_exposure;

    /**
     * The trend defining the standard deviation of the town's exposure risk factor
     * 
     * Standard deviation of exposure risk factor is determined by tile position according to this trend.
     */
    trend *sd_exposure;

    /**
     * The trend defining the town's population density
     * 
     * Population density is determined by tile position according to this trend.
     */
    trend *population_density;

    /**
     * A container holding all tiles belonging to the town.  The town is responsible
     * for managing the memory needed for all of its child tiles.
     */
    tile_list_type tile_list;

    /**
     * A list of the coordinates of all disease pockets affecting the town
     */
    coordinate_list_type disease_pocket_list;

    /**
     * The number of individuals in this town.
     */
    unsigned int number_of_individuals;

    /**
     * A cache of the number of selected individuals
     */
    unsigned int number_of_selected_individuals;

    /**
     * A cache of the number of selected individuals who are diseased
     */
    std::vector<unsigned int> number_of_selected_diseased_individuals;
  };
}

/** @} end of doxygen group */

#endif
