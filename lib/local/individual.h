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

    // defining pure abstract methods
    std::string get_name() const { return "individual"; }
    void copy( const base_object* o ) { this->copy( static_cast<const individual*>( o ) ); }
    void copy( const individual* );
    void from_json( const Json::Value& );
    void to_json( Json::Value& ) const;
    void to_csv( std::ostream&, std::ostream& ) const;
    unsigned int get_number_of_individuals() const { return 1; }
    void assert_summary();
    void rebuild_summary();
    void select() { this->select( 1.0 ); }
    void select( const double sample_weight );
    void unselect();
    double get_sample_weight() const { return this->sample_weight; }
    void set_sample_weight( const double sample_weight ) { this->sample_weight = sample_weight; }

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
     * Returns the individual's state
     */
    state_type get_state( unsigned int index ) const { return this->state_list[index]; }

    /**
     * Sets the individual's disease status
     */
    void set_disease( unsigned int index, const bool disease )
    {
      this->state_list[index] = disease ? DISEASED : HEALTHY;
    }

    /**
     * Returns whether the individual has a disease
     */
    bool is_disease( unsigned int index ) const { return DISEASED == this->state_list[index]; }

    /**
     * Returns the individual's exposure
     */
    exposure_type get_exposure() const { return this->exposure; }

    /**
     * Sets the individual's exposure status
     */
    void set_exposure( const bool exposed ) { this->exposure = exposed ? EXPOSED : NOT_EXPOSED; }

    /**
     * Returns whether the individual has been exposed
     */
    bool is_exposed() const { return EXPOSED == this->exposure; }

    /**
     * Returns this individual's index
     */
    unsigned int get_index() const { return this->index; }

  protected:
    void create();
    void define();

  private:
    /**
     * A reference to the household that the individual belongs to (not reference counted)
     */
    household *parent;

    /**
     * The individual's 0-based index within its parent population
     */
    unsigned int index;

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
    std::vector<state_type> state_list;

    /**
     * The individual's exposure status
     */
    exposure_type exposure;

    /**
     * When selected, the individual's sample weight
     */
    double sample_weight;
  };
}

/** @} end of doxygen group */

#endif
