/*=========================================================================

  Program:  sampsim
  Module:   summary.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_summary_h
#define __sampsim_summary_h

#include "base_object.h"
#include "utilities.h"

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
    void reset()
    {
      for( unsigned int i = 0; i < 8; i++ )
      {
        this->count[i] = 0;
        this->weighted_count[i] = 0.0;
      }
    }

    /**
     * Adds a model's summary totals to this one
     */
    void add( model_object* );

    /**
     * Method for getting count data
     */
    unsigned int get_count(
      const age_type age = ANY_AGE,
      const sex_type sex = ANY_SEX,
      const state_type state = ANY_STATE ) const
    {
      unsigned int count = 0;

      if( ( ANY_AGE == age || ADULT == age ) &&
          ( ANY_SEX == sex || MALE == sex ) &&
          ( ANY_STATE == state || HEALTHY == state ) )
        count += this->count[adult_male_healthy];
      if( ( ANY_AGE == age || ADULT == age ) &&
          ( ANY_SEX == sex || MALE == sex ) &&
          ( ANY_STATE == state || DISEASED == state ) )
        count += this->count[adult_male_diseased];
      if( ( ANY_AGE == age || ADULT == age ) &&
          ( ANY_SEX == sex || FEMALE == sex ) &&
          ( ANY_STATE == state || HEALTHY == state ) )
        count += this->count[adult_female_healthy];
      if( ( ANY_AGE == age || ADULT == age ) &&
          ( ANY_SEX == sex || FEMALE == sex ) &&
          ( ANY_STATE == state || DISEASED == state ) )
        count += this->count[adult_female_diseased];
      if( ( ANY_AGE == age || CHILD == age ) &&
          ( ANY_SEX == sex || MALE == sex ) &&
          ( ANY_STATE == state || HEALTHY == state ) )
        count += this->count[child_male_healthy];
      if( ( ANY_AGE == age || CHILD == age ) &&
          ( ANY_SEX == sex || MALE == sex ) &&
          ( ANY_STATE == state || DISEASED == state ) )
        count += this->count[child_male_diseased];
      if( ( ANY_AGE == age || CHILD == age ) &&
          ( ANY_SEX == sex || FEMALE == sex ) &&
          ( ANY_STATE == state || HEALTHY == state ) )
        count += this->count[child_female_healthy];
      if( ( ANY_AGE == age || CHILD == age ) &&
          ( ANY_SEX == sex || FEMALE == sex ) &&
          ( ANY_STATE == state || DISEASED == state ) )
        count += this->count[child_female_diseased];

      return count;
    };

    /**
     * Method for getting weighted count data
     */
    double get_weighted_count(
      const age_type age = ANY_AGE,
      const sex_type sex = ANY_SEX,
      const state_type state = ANY_STATE ) const
    {
      double weighted_count = 0;

      if( ( ANY_AGE == age || ADULT == age ) &&
          ( ANY_SEX == sex || MALE == sex ) &&
          ( ANY_STATE == state || HEALTHY == state ) )
        weighted_count += this->weighted_count[adult_male_healthy];
      if( ( ANY_AGE == age || ADULT == age ) &&
          ( ANY_SEX == sex || MALE == sex ) &&
          ( ANY_STATE == state || DISEASED == state ) )
        weighted_count += this->weighted_count[adult_male_diseased];
      if( ( ANY_AGE == age || ADULT == age ) &&
          ( ANY_SEX == sex || FEMALE == sex ) &&
          ( ANY_STATE == state || HEALTHY == state ) )
        weighted_count += this->weighted_count[adult_female_healthy];
      if( ( ANY_AGE == age || ADULT == age ) &&
          ( ANY_SEX == sex || FEMALE == sex ) &&
          ( ANY_STATE == state || DISEASED == state ) )
        weighted_count += this->weighted_count[adult_female_diseased];
      if( ( ANY_AGE == age || CHILD == age ) &&
          ( ANY_SEX == sex || MALE == sex ) &&
          ( ANY_STATE == state || HEALTHY == state ) )
        weighted_count += this->weighted_count[child_male_healthy];
      if( ( ANY_AGE == age || CHILD == age ) &&
          ( ANY_SEX == sex || MALE == sex ) &&
          ( ANY_STATE == state || DISEASED == state ) )
        weighted_count += this->weighted_count[child_male_diseased];
      if( ( ANY_AGE == age || CHILD == age ) &&
          ( ANY_SEX == sex || FEMALE == sex ) &&
          ( ANY_STATE == state || HEALTHY == state ) )
        weighted_count += this->weighted_count[child_female_healthy];
      if( ( ANY_AGE == age || CHILD == age ) &&
          ( ANY_SEX == sex || FEMALE == sex ) &&
          ( ANY_STATE == state || DISEASED == state ) )
        weighted_count += this->weighted_count[child_female_diseased];

      return weighted_count;
    };

    /**
     * Method for getting count faction data
     */
    double get_count_fraction( const age_type age = ANY_AGE, const sex_type sex = ANY_SEX ) const
    {
      return static_cast< double >( this->get_count( age, sex, DISEASED ) ) /
             static_cast< double >( this->get_count( age, sex, ANY_STATE ) );
    };

    /**
     * Method for getting weighted count faction data
     */
    double get_weighted_count_fraction( const age_type age = ANY_AGE, const sex_type sex = ANY_SEX ) const
    {
      return this->get_weighted_count( age, sex, DISEASED ) / this->get_weighted_count( age, sex, ANY_STATE );
    };

  private:
    enum 
    {
      adult_male_healthy = 0,
      adult_male_diseased,
      adult_female_healthy,
      adult_female_diseased,
      child_male_healthy,
      child_male_diseased,
      child_female_healthy,
      child_female_diseased
    };

    /**
     * A list of all child summaries added to this summary
     */
    std::vector< summary* > child_list;

    /**
     * Individual count values
     */
    unsigned int count[8];

    /**
     * Individual weighted count values
     */
    double weighted_count[8];
  };
}

/** @} end of doxygen group */

#endif
