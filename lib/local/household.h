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

    // defining pure abstract methods
    std::string get_name() const { return "household"; }
    void copy( const base_object* o ) { this->copy( static_cast<const household*>( o ) ); }
    void copy( const household* );
    void from_json( const Json::Value& );
    void to_json( Json::Value& ) const;
    void to_csv( std::ostream&, std::ostream& ) const;
    unsigned int get_number_of_individuals() const;
    void assert_summary();
    void rebuild_summary();
    void select();
    void unselect();
    void select_all();

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
     * Returns the household's exposure risk factor
     */
    double get_exposure_risk() const { return this->exposure_risk; }

    /**
     * Returns this household's index
     */
    unsigned int get_index() const { return this->index; }

  protected:
    void create();
    void define();

  private:
    /**
     * A reference to the building that the household belongs to (not reference counted)
     */
    building *parent;

    /**
     * The household's 0-based index within its parent population
     */
    unsigned int index;

    /**
     * The household's income
     */
    double income;

    /**
     * The household's disease risk factor
     */
    double disease_risk;

    /**
     * The household's exposure risk factor
     */
    double exposure_risk;

    /**
     * A container holding all individuals belonging to this household.  The household is responsible
     * for managing the memory needed for all of its child individuals.
     */
    individual_list_type individual_list;
  };
}

/** @} end of doxygen group */

#endif
