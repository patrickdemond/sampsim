/*=========================================================================

  Program:  sampsim
  Module:   town.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_town_h
#define __sampsim_town_h

#include "model_object.h"

#include "distribution.h"
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
  public:
    /**
     * Constructor
     * 
     * When a town is created its parent population and index must be defined.
     */
    town( population *parent, const unsigned int index );

    /**
     * Destructor
     */
    ~town();

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
     * Generate the town and create all tiles belonging to it
     * 
     * This method will generate the town according to its population's parameters.  A grid of N by M
     * tiles will be created forming a tesselation of squares all of equal size.
     */
    void generate();

    /**
     * Sets the number of tiles in the longitudinal (X) direction
     */
    void set_number_of_tiles_x( const unsigned int );

    /**
     * Sets the number of tiles in the latitudinal (Y) direction
     */
    void set_number_of_tiles_y( const unsigned int );

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
     * Get the number of individuals in:the town
     * 
     * Returns a sum of all individuals in all households in all buildings in all tiles in the town.
     * This method iterates over all tiles (and all buildings in those tiles, etc) every time it is called,
     * so it should only be used when re-counting is necessary.  A town contains no tiles (so no
     * individuals) until its generate() method is called.
     */
    unsigned int count_individuals() const;

    /**
     * Returns a coordinate at the centre of the town
     * 
     * This coordinate is based on the number of tiles in the X and Y directions as well as tile width.
     */
    coordinate get_centroid() const;

    /**:
     * Returns the surface area of the town
     * 
     * The surface area is determined by the area of each tile multiplied by the number of tiles
     * in the X and Y directions.
     */
    double get_area() const;

    /**
     * Deserialize the town
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void from_json( const Json::Value& );

    /**
     * Serialize the town
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void to_json( Json::Value& ) const;

    /**
     * Output the town to two CSV files (households and individuals)
     * 
     * All objects must provide an implementation for printing to a CSV (comma separated value) format.
     * Two streams are expected, the first is for household data and the second for individual data.
     */
    virtual void to_csv( std::ostream&, std::ostream& ) const;

    /**
     * Returns whether the town is selected or not
     * 
     * Selection works in the following manner: selecting an object also selects its parent but not its
     * children.  Unselecting an object also unselects its children but not its parent.  This mechanism
     * therefore defines "selection" as true if any of its children are selected, and allows for
     * unselecting all children by unselecting the object.  Only towns, buildings, households and
     * individuals may be selected/unselected.
     */
    bool is_selected() const { return this->selected; }

    /**
     * Selects the household
     */
    void select() { this->selected = true; }

    /**
     * Unselects the household
     * 
     * This will also unselect all children living in this household
     */
    void unselect();

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
     * The number of tiles to generate in the longitudinal (X) direction
     */
    unsigned int number_of_tiles_x;

    /**
     * The number of tiles to generate in the latitudinal (Y) direction
     */
    unsigned int number_of_tiles_y;

    /**
     * The town's mean household population (size)
     */
    double mean_household_population;

    /**
     * The population distribution to use when creating individuals within households
     */
    distribution population_distribution;

    /**
     * Whether the town is selected
     */
    bool selected;

    /**
     * A container holding all tiles belonging to the town.  The town is responsible
     * for managing the memory needed for all of its child tiles.
     */
    tile_list_type tile_list;

    /**
     * A list of the coordinates of all disease pockets affecting the town
     */
    coordinate_list_type disease_pocket_list;
  };
}

/** @} end of doxygen group */

#endif
