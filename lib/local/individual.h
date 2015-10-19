/*=========================================================================

  Program:  sampsim
  Module:   individual.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_individual_h
#define __sampsim_individual_h

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
  class household;
  class population;
  class town;
  class tile;

  /**
   * @class individual
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief An individual which belongs to a household
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
   * Individuals belong to one and only one building.  When an individual is selected its household
   * is also selected.  When unselecting an individual households are NOT unselected.
   */
  class individual : public model_object
  {
    friend class household;

  public:
    /**
     * Constructor
     * 
     * When an individual is created its parent household must be defined.
     */
    individual( household *parent );

    /**
     * Destructor
     */
    ~individual();

    /**
     * Copies another individual's values into the current object
     */
    void copy( const individual* );

    /**
     * Returns the name of the object's class
     */
    std::string get_name() const { return "individual"; }

    /**
     * Returns the individual's parent household
     */
    household* get_household() const { return this->parent; }

    /**
     * Returns the building that the individual belongs to
     */
    building* get_building() const;

    /**
     * Returns the tile that the individual belongs to
     */
    tile* get_tile() const;

    /**
     * Returns the town that the individual belongs to
     */
    town* get_town() const;

    /**
     * Returns the population that the individual belongs to
     */
    population* get_population() const;

    /**
     * Returns the individual's sex
     */
    sex_type get_sex() const { return this->sex; }

    /**
     * Sets the individual's sex
     */
    void set_sex( const sex_type sex ) { this->sex = sex; }

    /**
     * Returns the individual's age
     */
    age_type get_age() const { return this->age; }

    /**
     * Sets the individual's age
     */
    void set_age( const age_type age ) { this->age = age; }

    /**
     * Returns whether the individual has a disease
     */
    bool is_disease() const { return this->disease; }

    /**
     * Sets the individual's disease status
     */
    void set_disease( const bool disease ) { this->disease = disease; }

    /**
     * Returns whether the individual is selected or not
     * 
     * Selection works in the following manner: selecting an object also selects its parent but not its
     * children.  Unselecting an object also unselects its children but not its parent.  This mechanism
     * therefore defines "selection" as true if any of its children are selected, and allows for
     * unselecting all children by unselecting the object.  Only towns, buildings, households and
     * individuals may be selected/unselected.
     */
    bool is_selected() const { return this->selected; }

    /**
     * Selects the individual
     * 
     * This will also select the individual's parent household
     */
    void select();

    /**
     * Unselects the individual
     */
    void unselect() { this->selected = false; }

  protected:
    /**
     * Deserialize the individual
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void from_json( const Json::Value& );

    /**
     * Serialize the individual
     * 
     * All objects must provide an implementation for converting themselves to and from a
     * JSON-encoded string.  JSON is a lightweight data-interchange format (see http://json.org/).
     */
    virtual void to_json( Json::Value& ) const;

    /**
     * Output the individual to two CSV files (households and individuals)
     * 
     * All objects must provide an implementation for printing to a CSV (comma separated value) format.
     * Two streams are expected, the first is for household data and the second for individual data.
     */
    virtual void to_csv( std::ostream&, std::ostream& ) const;

  private:
    /**
     * A reference to the household that the individual belongs to (not reference counted)
     */
    household *parent;

    /**
     * The individual's age
     */
    age_type age;

    /**
     * The individual's sex
     */
    sex_type sex;

    /**
     * The individual's disease status
     */
    bool disease;

    /**
     * Whether the individual is selected
     */
    bool selected;
  };
}

/** @} end of doxygen group */

#endif
