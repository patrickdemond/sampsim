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
      this->count.resize( utilities::rr.size() );
      this->weighted_count.resize( utilities::rr.size() );
      for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ )
      {
        this->count[rr] = std::array< unsigned, 8 >{ 0, 0, 0, 0, 0, 0, 0, 0 };
        this->weighted_count[rr] = std::array< double, 8 >{ 0, 0, 0, 0, 0, 0, 0, 0 };
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
      const unsigned int rr,
      const age_type age = ANY_AGE,
      const sex_type sex = ANY_SEX,
      const state_type state = ANY_STATE ) const
    {
      unsigned int total = 0;

      if( ( ANY_AGE == age || ADULT == age ) &&
          ( ANY_SEX == sex || MALE == sex ) &&
          ( ANY_STATE == state || HEALTHY == state ) )
        total += this->count[rr][adult_male_healthy];
      if( ( ANY_AGE == age || ADULT == age ) &&
          ( ANY_SEX == sex || MALE == sex ) &&
          ( ANY_STATE == state || DISEASED == state ) )
        total += this->count[rr][adult_male_diseased];
      if( ( ANY_AGE == age || ADULT == age ) &&
          ( ANY_SEX == sex || FEMALE == sex ) &&
          ( ANY_STATE == state || HEALTHY == state ) )
        total += this->count[rr][adult_female_healthy];
      if( ( ANY_AGE == age || ADULT == age ) &&
          ( ANY_SEX == sex || FEMALE == sex ) &&
          ( ANY_STATE == state || DISEASED == state ) )
        total += this->count[rr][adult_female_diseased];
      if( ( ANY_AGE == age || CHILD == age ) &&
          ( ANY_SEX == sex || MALE == sex ) &&
          ( ANY_STATE == state || HEALTHY == state ) )
        total += this->count[rr][child_male_healthy];
      if( ( ANY_AGE == age || CHILD == age ) &&
          ( ANY_SEX == sex || MALE == sex ) &&
          ( ANY_STATE == state || DISEASED == state ) )
        total += this->count[rr][child_male_diseased];
      if( ( ANY_AGE == age || CHILD == age ) &&
          ( ANY_SEX == sex || FEMALE == sex ) &&
          ( ANY_STATE == state || HEALTHY == state ) )
        total += this->count[rr][child_female_healthy];
      if( ( ANY_AGE == age || CHILD == age ) &&
          ( ANY_SEX == sex || FEMALE == sex ) &&
          ( ANY_STATE == state || DISEASED == state ) )
        total += this->count[rr][child_female_diseased];

      return total;
    };

    /**
     * Method for getting weighted count data
     */
    double get_weighted_count(
      const unsigned int rr,
      const age_type age = ANY_AGE,
      const sex_type sex = ANY_SEX,
      const state_type state = ANY_STATE ) const
    {
      double total = 0;

      if( ( ANY_AGE == age || ADULT == age ) &&
          ( ANY_SEX == sex || MALE == sex ) &&
          ( ANY_STATE == state || HEALTHY == state ) )
        total += this->weighted_count[rr][adult_male_healthy];
      if( ( ANY_AGE == age || ADULT == age ) &&
          ( ANY_SEX == sex || MALE == sex ) &&
          ( ANY_STATE == state || DISEASED == state ) )
        total += this->weighted_count[rr][adult_male_diseased];
      if( ( ANY_AGE == age || ADULT == age ) &&
          ( ANY_SEX == sex || FEMALE == sex ) &&
          ( ANY_STATE == state || HEALTHY == state ) )
        total += this->weighted_count[rr][adult_female_healthy];
      if( ( ANY_AGE == age || ADULT == age ) &&
          ( ANY_SEX == sex || FEMALE == sex ) &&
          ( ANY_STATE == state || DISEASED == state ) )
        total += this->weighted_count[rr][adult_female_diseased];
      if( ( ANY_AGE == age || CHILD == age ) &&
          ( ANY_SEX == sex || MALE == sex ) &&
          ( ANY_STATE == state || HEALTHY == state ) )
        total += this->weighted_count[rr][child_male_healthy];
      if( ( ANY_AGE == age || CHILD == age ) &&
          ( ANY_SEX == sex || MALE == sex ) &&
          ( ANY_STATE == state || DISEASED == state ) )
        total += this->weighted_count[rr][child_male_diseased];
      if( ( ANY_AGE == age || CHILD == age ) &&
          ( ANY_SEX == sex || FEMALE == sex ) &&
          ( ANY_STATE == state || HEALTHY == state ) )
        total += this->weighted_count[rr][child_female_healthy];
      if( ( ANY_AGE == age || CHILD == age ) &&
          ( ANY_SEX == sex || FEMALE == sex ) &&
          ( ANY_STATE == state || DISEASED == state ) )
        total += this->weighted_count[rr][child_female_diseased];

      return total;
    };

    /**
     * Method for getting count faction data
     */
    double get_count_fraction(
      const unsigned int rr, const age_type age = ANY_AGE, const sex_type sex = ANY_SEX ) const
    {
      return static_cast< double >( this->get_count( rr, age, sex, DISEASED ) ) /
             static_cast< double >( this->get_count( rr, age, sex, ANY_STATE ) );
    };

    /**
     * Method for getting weighted count faction data
     */
    double get_weighted_count_fraction(
      const unsigned int rr, const age_type age = ANY_AGE, const sex_type sex = ANY_SEX ) const
    {
      return this->get_weighted_count( rr, age, sex, DISEASED ) /
             this->get_weighted_count( rr, age, sex, ANY_STATE );
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
    std::vector< std::array< unsigned, 8 > > count;

    /**
     * Individual weighted count values
     */
    std::vector< std::array< double, 8 > > weighted_count;
  };
}

/** @} end of doxygen group */

#endif
