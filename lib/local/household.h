/*=========================================================================

  Program:  sampsim
  Module:   household.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_household_h
#define __sampsim_household_h

#include "model_object.h"
#include "utilities.h"

namespace Json { class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class building;
  class individual;
  class population;
  class town;
  class tile;

  /**
   * @class household
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief A household which belongs to a building and contains individuals
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
   * Households belong to one and only one building.  They contain a list of individuals which live
   * in the household.  When a household is selected the selection state of its individuals are
   * unchanged but its parent building is also selected.  When unselecting a household all
   * individuals contained within the household are also unselected but its parent building is
   * NOT unselected.
   */
  class household : public model_object
  {
    friend class building;

  public:
    /**
     * Constructor
     * 
     * When a household is created its parent building must be defined.
     */
    household( building *parent );

    /**
    * Destructor
    * 
    * Deletes all individuals belonging to this household.
    */
    ~household();

    /**
     * Iterator access to child individuals
     * 
     * These methods provide iterator access to the list of individuals belonging to this household.
     */
    individual_list_type::iterator get_individual_list_begin()
    { return this->individual_list.begin(); }

    /**
     * Iterator access to child individuals
     * 
     * These methods provide iterator access to the list of individuals belonging to this household.
     */
    individual_list_type::iterator get_individual_list_end()
    { return this->individual_list.end(); }

    /**
     * Constant iterator access to child individuals
     * 
     * These methods provide constant iterator access to the list of individuals belonging to this household.
     */
    individual_list_type::const_iterator get_individual_list_cbegin() const
    { return this->individual_list.cbegin(); }

    /**
     * Constant iterator access to child individuals
     * 
     * These methods provide constant iterator access to the list of individuals belonging to this household.
     */
    individual_list_type::const_iterator get_individual_list_cend() const
    { return this->individual_list.cend(); }

    /**
     * Returns the household's parent household
     */
    building* get_building() const { return this->parent; }

    /**
     * Returns the tile that the household belongs to
     */
    tile* get_tile() const;

    /**
     * Returns the town that the household belongs to
     */
    town* get_town() const;

    /**
     * Returns the population that the household belongs to
     */
    population* get_population() const;

    /**
     * Returns the household's income
     */
    double get_income() const { return this->income; }

    /**
     * Returns the household's disease risk factor
     */
    double get_disease_risk() const { return this->disease_risk; }

    /**
     * Returns whether the household is selected or not
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
     * 
     * This will also select the parent building.
     */
    void select();

    /**
     * Unselects the household
     * 
     * This will also unselect all children living in this household
     */
    void unselect();

    /**
     * Get the number of individuals in the household
     * 
     * Returns a sum of all individuals in the household.  This method counts the number of individuals
     * every time it is called, so it should only be used when re-counting is necessary.
     * A household contains no individuals (so no individuals) until its create() method is called.
     */
    unsigned int count_individuals() const;

  protected:
    /**
     * Create all individuals belonging to the household
     * 
     * This method will create the household according to its internal parameters.  The method
     * creates individuals but does not define their properties.  After calling this function all individuals
     * belonging to the household will exist but without parameters such as income, disease status,
     * disease risk, etc.
     */
    void create();

    /**
     * Define all parameters for all individuals belonging to the household
     * 
     * This method will determine all factors such as income, disease status, disease risk, etc for
     * all individuals belonging to the household.  If this method is called before the individuals
     * have been created nothing will happen.
     */
    void define();

    /**
     * Deserialize the household
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void from_json( const Json::Value& );

    /**
     * Serialize the household
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void to_json( Json::Value& ) const;

    /**
     * Output the household to two CSV files (households and individuals)
     * 
     * All objects must provide an implementation for printing to a CSV (comma separated value) format.
     * Two streams are expected, the first is for household data and the second for individual data.
     */
    virtual void to_csv( std::ostream&, std::ostream& ) const;

  private:
    /**
     * A reference to the building that the household belongs to (not reference counted)
     */
    building *parent;

    /**
     * The household's income
     */
    double income;

    /**
     * The household's disease risk factor
     */
    double disease_risk;

    /**
     * Whether the household is selected
     */
    bool selected;

    /**
     * A container holding all individuals belonging to this household.  The household is responsible
     * for managing the memory needed for all of its child individuals.
     */
    individual_list_type individual_list;
  };
}

/** @} end of doxygen group */

#endif
