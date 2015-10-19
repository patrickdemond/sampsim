/*=========================================================================

  Program:  sampsim
  Module:   building.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_building_h
#define __sampsim_building_h

#include "model_object.h"

#include "utilities.h"

namespace Json { class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class household;
  class population;
  class town;
  class tile;

  /**
   * @class building
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief A building which belongs to a tile and contains households
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
   * Buildings belong to one and only one tile based on its position.  They contain a list of
   * households which are in the building.  When a building is selected the selection state of
   * its households are unchanged but its parent town is also selected.  When unselecting a
   * building all households contained within the building are also unselected but its parent
   * town is NOT unselected.
   */
  class building : public model_object
  {
    friend class tile;

  public:
    /**
     * Constructor
     * 
     * When a building is created its parent tile must be defined.
     */
    building( tile *parent );

    /**
    * Destructor
    * 
    * Deletes all households belonging to this building.
    */
    ~building();

    /**
     * Returns the name of the object's class
     */
    std::string get_name() const { return "building"; }

    /**
     * Iterator access to child households
     * 
     * These methods provide iterator access to the list of households belonging to this building.
     */
    household_list_type::iterator get_household_list_begin()
    { return this->household_list.begin(); }

    /**
     * Iterator access to child households
     * 
     * These methods provide iterator access to the list of households belonging to this building.
     */
    household_list_type::iterator get_household_list_end()
    { return this->household_list.end(); }

    /**
     * Constant iterator access to child households
     * 
     * These methods provide constant iterator access to the list of households belonging to this building.
     */
    household_list_type::const_iterator get_household_list_cbegin() const
    { return this->household_list.cbegin(); }

    /**
     * Constant iterator access to child households
     * 
     * These methods provide constant iterator access to the list of households belonging to this building.
     */
    household_list_type::const_iterator get_household_list_cend() const
    { return this->household_list.cend(); }

    /**
     * Returns the building's parent tile
     */
    tile* get_tile() const { return this->parent; }

    /**
     * Returns the building's parent tile
     */
    town* get_town() const;

    /**
     * Returns the population that the building belongs to
     */
    population* get_population() const;

    /**
     * Get the number of diseased and total number of individuals in the population
     * 
     * Returns a pair containing the total number of individuals who have disease and the total number of
     * individuals in total in the building.
     * This method iterates over all towns (and all tiles in those towns, etc) every time it is called, so
     * it should only be used when re-counting is necessary.  A building contains no households (so no
     * individuals) until its create() method is called.
     */
    std::pair<unsigned int, unsigned int> count_individuals() const;

    /**
     * Get the position of the building
     * 
     * The building's position is determined by the create() method and always falls inside its
     * parent tile's bounds.
     */
    coordinate get_position() const { return this->position; }

    /**
     * Returns the building's pocket factor
     * 
     * This parameter is calculated based on the building's location relative to all pockets in the
     * simulation.
     */
    double get_pocket_factor() const { return this->pocket_factor; }

    /**
     * Returns whether the building is selected or not
     * 
     * Selection works in the following manner: selecting an object also selects its parent but not its
     * children.  Unselecting an object also unselects its children but not its parent.  This mechanism
     * therefore defines "selection" as true if any of its children are selected, and allows for
     * unselecting all children by unselecting the object.  Only towns, buildings, households and
     * individuals may be selected/unselected.
     */
    bool is_selected() const { return this->selected; }

    /**
     * Selects the building
     * 
     * This will also select the parent town.
     */
    void select();

    /**
     * Unselectes the building
     * 
     * This will also unselect all households contained within this building.
     */
    void unselect();

    /**
     * A static function which describes how to sort buildings by their x position.
     * 
     * This function is used by the building_tree class in order to sort buildings by their position.
     */
    static bool sort_by_x( building* a, building* b ) { return a->get_position().x < b->get_position().x; }

    /**
     * A static function which describes how to sort buildings by their y position.
     * 
     * This function is used by the building_tree class in order to sort buildings by their position.
     */
    static bool sort_by_y( building* a, building* b ) { return a->get_position().y < b->get_position().y; }

    /**
     * Determines whether the building is in a river or not (banks do not count as "in" a river)
     */
    bool in_river() const;

    /**
     * Copies another building's values into the current object
     */
    void copy( const building* );

  protected:
    /**
     * Create all households belonging to the building
     * 
     * This method will create the building according to its internal parameters.  The method
     * creates households but does not define their properties.  After calling this function all individuals
     * belonging to the building will exist but without parameters such as income, disease status,
     * disease risk, etc.
     */
    void create();

    /**
     * Define all parameters for all households belonging to the building
     * 
     * This method will determine all factors such as income, disease status, disease risk, etc for
     * all individuals belonging to the building.  If this method is called before the individuals
     * have been created nothing will happen.
     */
    void define();

    /**
     * Deserialize the building
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void from_json( const Json::Value& );

    /**
     * Serialize the building
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void to_json( Json::Value& ) const;

    /**
     * Output the building to two CSV files (households and individuals)
     * 
     * All objects must provide an implementation for printing to a CSV (comma separated value) format.
     * Two streams are expected, the first is for household data and the second for individual data.
     */
    virtual void to_csv( std::ostream&, std::ostream& ) const;

  private:
    /**
     * A reference to the tile that the building belongs to (not reference counted)
     */
    tile *parent;

    /**
     * The position of the building
     */
    coordinate position;

    /**
     * Whether the building is selected
     */
    bool selected;

    /**
     * The building's pocket factor
     */
    double pocket_factor;

    /**
     * A container holding all households belonging to this building.  The building is responsible
     * for managing the memory needed for all of its child households.
     */
    household_list_type household_list;
  };
}

/** @} end of doxygen group */

#endif
