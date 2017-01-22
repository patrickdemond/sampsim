/*=========================================================================

  Program:  sampsim
  Module:   summary.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_summary_h
#define __sampsim_summary_h

#include "base_object.h"

#include <vector>

namespace Json { class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class model_object;
  class individual;

  /**
   * @class summary
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief TODO
   * @details * TODO
   */
  class summary
  {
    friend individual;

  public:
    /**
     * Constructor
     */
    summary() { this->reset(); }

    /**
     * Returns all values to 0
     */
    void reset();

    /**
     * Adds a model's summary totals to this one
     */
    void add( model_object* );

    /**
     * Method for getting count data
     */
    unsigned int get_count( const int category = all, const int state = all ) const
    {
      unsigned int count = 0;

      if( all == category && all == state )
      {
        count += this->count[adult][diseased];
        count += this->count[adult][healthy];
        count += this->count[child][diseased];
        count += this->count[child][healthy];
      }
      else if( all == category )
      {
        count += this->count[adult][state];
        count += this->count[child][state];
      }
      else if( all == state )
      {
        count += this->count[category][diseased];
        count += this->count[category][healthy];
      }
      else
      {
        count += this->count[category][state];
      }

      return count;
    };

    /**
     * Method for getting count faction data
     */
    double get_count_fraction( const int category = all ) const
    {
      return static_cast< double >( this->get_count( category, diseased ) ) /
             static_cast< double >( this->get_count( category, all ) );
    };

    /**
     * Enumeration defining the number of categories and states
     */
    enum { category_size = 8, state_size = 2 };

    /**
     * Enumeration defining category and state indeces
     */
    enum
    {
      // categories
      adult = 0, child = 1,
      male = 2, female = 3,
      adult_male = 4, adult_female = 5,
      child_male = 6, child_female = 7,

      // states
      diseased = 0, healthy = 1,
      
      all = -1
    };

  private:
    /**
     * A list of all child summaries added to this summary
     */
    std::vector< summary* > child_list;

    /**
     * Individual count values
     */
    unsigned int count[category_size][state_size];

    /**
     * Individual weight values
     */
    double weight[category_size][state_size];
  };
}

/** @} end of doxygen group */

#endif
