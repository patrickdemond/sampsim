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
    void reset();

    /**
     * Adds a summary's totals to this one
     */
    void add( summary* );

    /**
     * Adds a model's summary totals to this one
     */
    void add( model_object* );

    /**
     * Writes the summary to a text file
     */
    void write( std::ostream& ) const;

    /**
     * Writes the summary of a list of summaries to a text file
     */
    static void write( std::vector< summary* >, bool weighted, std::ostream& );

    /**
     * Method for getting count data
     */
    unsigned int get_count(
      const unsigned int rr,
      const age_type age = ANY_AGE,
      const sex_type sex = ANY_SEX,
      const state_type state = ANY_STATE,
      const exposure_type expusore = ANY_EXPOSURE ) const;

    /**
     * Method for getting weighted count data
     */
    double get_weighted_count(
      const unsigned int rr,
      const age_type age = ANY_AGE,
      const sex_type sex = ANY_SEX,
      const state_type state = ANY_STATE,
      const exposure_type exposure = ANY_EXPOSURE ) const;

    /**
     * Method for getting relative risk data
     */
    double get_relative_risk(
      const unsigned int rr,
      const age_type age = ANY_AGE,
      const sex_type sex = ANY_SEX ) const;

    /**
     * Method for getting count fraction data
     */
    double get_count_fraction(
      const unsigned int rr,
      const age_type age = ANY_AGE,
      const sex_type sex = ANY_SEX ) const;

    /**
     * Method for getting weighted relative risk data
     */
    double get_weighted_relative_risk(
      const unsigned int rr,
      const age_type age = ANY_AGE,
      const sex_type sex = ANY_SEX ) const;

    /**
     * Method for getting weighted count fraction data
     */
    double get_weighted_count_fraction(
      const unsigned int rr,
      const age_type age = ANY_AGE,
      const sex_type sex = ANY_SEX ) const;

  private:
    enum 
    {
      adult_male_healthy_not_exposed = 0,
      adult_male_healthy_exposed,
      adult_male_diseased_not_exposed,
      adult_male_diseased_exposed,
      adult_female_healthy_not_exposed,
      adult_female_healthy_exposed,
      adult_female_diseased_not_exposed,
      adult_female_diseased_exposed,
      child_male_healthy_not_exposed,
      child_male_healthy_exposed,
      child_male_diseased_not_exposed,
      child_male_diseased_exposed,
      child_female_healthy_not_exposed,
      child_female_healthy_exposed,
      child_female_diseased_not_exposed,
      child_female_diseased_exposed
    };

    /**
     * Converts age/sex/state/exposure values into a count index
     */
    static int get_count_index(
      const age_type age, const sex_type sex, const state_type state, const exposure_type exposure )
    {
      int index = 0;
      if( CHILD == age ) index += 8;
      if( FEMALE == sex ) index += 4;
      if( DISEASED == state ) index += 2;
      if( EXPOSED == exposure ) index += 1;
      return index;
    }

    /**
     * A list of all child summaries added to this summary
     */
    std::vector< summary* > child_list;

    /**
     * Individual count values
     */
    std::vector< std::array< unsigned, 16 > > count;

    /**
     * Individual weighted count values
     */
    std::vector< std::array< double, 16 > > weighted_count;
  };
}

/** @} end of doxygen group */

#endif
