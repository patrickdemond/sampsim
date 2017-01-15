/*=========================================================================

  Program:  sampsim
  Module:   model_object.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_model_object_h
#define __sampsim_model_object_h

#include "base_object.h"
#include "summary.h"
#include "utilities.h"

namespace Json { class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  /**
   * @class model_object
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief Abstract base class for all classes which describe the population
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
   * This class contains all functionality common to all model classes.
   */
  class model_object : public base_object
  {
  public:
    /**
     * Constructor
     */
    model_object() : selected( false ) {}

    /**
     * Get the number of individuals in the model
     * 
     * This is much faster than get_summary() and should be used if you only need the total number of
     * individuals in the model.
     */
    virtual unsigned int get_number_of_individuals() const = 0;

    /**
     * Get the number of diseased and non-diseased individuals in the model
     * 
     * Returns a vector of pairs containing the total number of individuals who do and do not have disease.
     * Nine pairs will be returned in the following order:
     *   including all individuals
     *   including adults only
     *   including children only
     *   including males only
     *   including females only
     *   including adult males only
     *   including adult females only
     *   including child males only
     *   including child females only
     * This method iterates over all child models every time it is called, so it should only be used when
     * re-counting is necessary.  Do not call this method until the create() method has been called.
     */
    virtual summary* get_summary() = 0;
    virtual std::vector< std::pair<unsigned int, unsigned int> >count_individuals() const = 0;

    /**
     * Returns whether the model is selected or not
     * 
     * Selection works in the following manner: selecting an object also selects its parent but not its
     * children.  Unselecting an object also unselects its children but not its parent.  This mechanism
     * therefore defines "selection" as true if any of its children are selected, and allows for
     * unselecting all children by unselecting the object.
     */
    bool is_selected() const { return this->selected; }
    
    /**
     * Select the model
     */
    virtual void select() = 0;

    /**
     * Unselect the model
     */
    virtual void unselect() = 0;

  protected:
    /**
     * Create all child models belonging to the model
     * 
     * This method will create the child models according to its internal parameters.  The method
     * creates the model but does not define its properties.  After calling this function all individuals
     * belonging to the model will exist but without parameters such as income, disease status,
     * disease risk, etc.
     */
    virtual void create() = 0;

    /**
     * Define all parameters for all child models belonging to the model
     * 
     * This method will determine all factors such as income, disease status, disease risk, etc for
     * all individuals belonging to the model.  If this method is called before the individuals
     * have been created nothing will happen.
     */
    virtual void define() = 0;

    /**
     * Whether the model is selected
     */
    bool selected;

    /**
     * A summary object which tracks a summary of the object's data
     */
    summary sum;
  };
}

/** @} end of doxygen group */

#endif
