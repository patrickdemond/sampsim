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
  /**
   * @class summary
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief TODO
   * @details * TODO
   */
  class summary
  {
  public:
    /**
     * Constructor
     */
    summary();

    /**
     * Mark the summary as expired, meaning that data will need to be re-calculated
     */
    void expire();

    /**
     * 
     */
    void calculate();

  private:
    /**
     * Whether or not the data is out of date
     */
    bool out_of_date;

    /**
     * Individual count values
     */
    unsigned int count;
    unsigned int selected_count;
    unsigned int adult_count;
    unsigned int adult_selected_count;
    unsigned int child_count;
    unsigned int child_selected_count;
    unsigned int male_count;
    unsigned int male_selected_count;
    unsigned int female_count;
    unsigned int female_selected_count;
    unsigned int adult_male_count;
    unsigned int adult_male_selected_count;
    unsigned int adult_female_count;
    unsigned int adult_female_selected_count;
    unsigned int child_male_count;
    unsigned int child_male_selected_count;
    unsigned int child_female_count;
    unsigned int child_female_selected_count;

    /**
     * Individual weight values
     */
    double weight;
    double selected_weight;
    double adult_weight;
    double adult_selected_weight;
    double child_weight;
    double child_selected_weight;
    double male_weight;
    double male_selected_weight;
    double female_weight;
    double female_selected_weight;
    double adult_male_weight;
    double adult_male_selected_weight;
    double adult_female_weight;
    double adult_female_selected_weight;
    double child_male_weight;
    double child_male_selected_weight;
    double child_female_weight;
    double child_female_selected_weight;
  };
}

/** @} end of doxygen group */

#endif
