/*=========================================================================

  Program:  sampsim
  Module:   summary.cxx
  Language: C++

=========================================================================*/

#include "summary.h"

#include "model_object.h"

#include <fstream>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void summary::reset()
  {
    this->count.resize( utilities::rr.size() );
    this->weighted_count.resize( utilities::rr.size() );
    for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ )
    {
      this->count[rr] = std::array< unsigned, 16 >{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      this->weighted_count[rr] = std::array< double, 16 >{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  unsigned int summary::get_count(
    const unsigned int rr,
    const age_type age,
    const sex_type sex,
    const state_type state,
    const exposure_type exposure ) const
  {
    unsigned int total = 0;

    if( ( ANY_AGE == age || ADULT == age ) &&
        ( ANY_SEX == sex || MALE == sex ) &&
        ( ANY_STATE == state || HEALTHY == state ) &&
        ( ANY_EXPOSURE == exposure || NOT_EXPOSED == exposure ) )
      total += this->count[rr][adult_male_healthy_not_exposed];
    if( ( ANY_AGE == age || ADULT == age ) &&
        ( ANY_SEX == sex || MALE == sex ) &&
        ( ANY_STATE == state || HEALTHY == state ) &&
        ( ANY_EXPOSURE == exposure || EXPOSED == exposure ) )
      total += this->count[rr][adult_male_healthy_exposed];
    if( ( ANY_AGE == age || ADULT == age ) &&
        ( ANY_SEX == sex || MALE == sex ) &&
        ( ANY_STATE == state || DISEASED == state ) &&
        ( ANY_EXPOSURE == exposure || NOT_EXPOSED == exposure ) )
      total += this->count[rr][adult_male_diseased_not_exposed];
    if( ( ANY_AGE == age || ADULT == age ) &&
        ( ANY_SEX == sex || MALE == sex ) &&
        ( ANY_STATE == state || DISEASED == state ) &&
        ( ANY_EXPOSURE == exposure || EXPOSED == exposure ) )
      total += this->count[rr][adult_male_diseased_exposed];
    if( ( ANY_AGE == age || ADULT == age ) &&
        ( ANY_SEX == sex || FEMALE == sex ) &&
        ( ANY_STATE == state || HEALTHY == state ) &&
        ( ANY_EXPOSURE == exposure || NOT_EXPOSED == exposure ) )
      total += this->count[rr][adult_female_healthy_not_exposed];
    if( ( ANY_AGE == age || ADULT == age ) &&
        ( ANY_SEX == sex || FEMALE == sex ) &&
        ( ANY_STATE == state || HEALTHY == state ) &&
        ( ANY_EXPOSURE == exposure || EXPOSED == exposure ) )
      total += this->count[rr][adult_female_healthy_exposed];
    if( ( ANY_AGE == age || ADULT == age ) &&
        ( ANY_SEX == sex || FEMALE == sex ) &&
        ( ANY_STATE == state || DISEASED == state ) &&
        ( ANY_EXPOSURE == exposure || NOT_EXPOSED == exposure ) )
      total += this->count[rr][adult_female_diseased_not_exposed];
    if( ( ANY_AGE == age || ADULT == age ) &&
        ( ANY_SEX == sex || FEMALE == sex ) &&
        ( ANY_STATE == state || DISEASED == state ) &&
        ( ANY_EXPOSURE == exposure || EXPOSED == exposure ) )
      total += this->count[rr][adult_female_diseased_exposed];
    if( ( ANY_AGE == age || CHILD == age ) &&
        ( ANY_SEX == sex || MALE == sex ) &&
        ( ANY_STATE == state || HEALTHY == state ) &&
        ( ANY_EXPOSURE == exposure || NOT_EXPOSED == exposure ) )
      total += this->count[rr][child_male_healthy_not_exposed];
    if( ( ANY_AGE == age || CHILD == age ) &&
        ( ANY_SEX == sex || MALE == sex ) &&
        ( ANY_STATE == state || HEALTHY == state ) &&
        ( ANY_EXPOSURE == exposure || EXPOSED == exposure ) )
      total += this->count[rr][child_male_healthy_exposed];
    if( ( ANY_AGE == age || CHILD == age ) &&
        ( ANY_SEX == sex || MALE == sex ) &&
        ( ANY_STATE == state || DISEASED == state ) &&
        ( ANY_EXPOSURE == exposure || NOT_EXPOSED == exposure ) )
      total += this->count[rr][child_male_diseased_not_exposed];
    if( ( ANY_AGE == age || CHILD == age ) &&
        ( ANY_SEX == sex || MALE == sex ) &&
        ( ANY_STATE == state || DISEASED == state ) &&
        ( ANY_EXPOSURE == exposure || EXPOSED == exposure ) )
      total += this->count[rr][child_male_diseased_exposed];
    if( ( ANY_AGE == age || CHILD == age ) &&
        ( ANY_SEX == sex || FEMALE == sex ) &&
        ( ANY_STATE == state || HEALTHY == state ) &&
        ( ANY_EXPOSURE == exposure || NOT_EXPOSED == exposure ) )
      total += this->count[rr][child_female_healthy_not_exposed];
    if( ( ANY_AGE == age || CHILD == age ) &&
        ( ANY_SEX == sex || FEMALE == sex ) &&
        ( ANY_STATE == state || HEALTHY == state ) &&
        ( ANY_EXPOSURE == exposure || EXPOSED == exposure ) )
      total += this->count[rr][child_female_healthy_exposed];
    if( ( ANY_AGE == age || CHILD == age ) &&
        ( ANY_SEX == sex || FEMALE == sex ) &&
        ( ANY_STATE == state || DISEASED == state ) &&
        ( ANY_EXPOSURE == exposure || NOT_EXPOSED == exposure ) )
      total += this->count[rr][child_female_diseased_not_exposed];
    if( ( ANY_AGE == age || CHILD == age ) &&
        ( ANY_SEX == sex || FEMALE == sex ) &&
        ( ANY_STATE == state || DISEASED == state ) &&
        ( ANY_EXPOSURE == exposure || EXPOSED == exposure ) )
      total += this->count[rr][child_female_diseased_exposed];

    return total;
  }
  
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double summary::get_weighted_count(
    const unsigned int rr,
    const age_type age,
    const sex_type sex,
    const state_type state,
    const exposure_type exposure ) const
  {
    double total = 0;

    if( ( ANY_AGE == age || ADULT == age ) &&
        ( ANY_SEX == sex || MALE == sex ) &&
        ( ANY_STATE == state || HEALTHY == state ) &&
        ( ANY_EXPOSURE == exposure || NOT_EXPOSED == exposure ) )
      total += this->weighted_count[rr][adult_male_healthy_not_exposed];
    if( ( ANY_AGE == age || ADULT == age ) &&
        ( ANY_SEX == sex || MALE == sex ) &&
        ( ANY_STATE == state || HEALTHY == state ) &&
        ( ANY_EXPOSURE == exposure || EXPOSED == exposure ) )
      total += this->weighted_count[rr][adult_male_healthy_exposed];
    if( ( ANY_AGE == age || ADULT == age ) &&
        ( ANY_SEX == sex || MALE == sex ) &&
        ( ANY_STATE == state || DISEASED == state ) &&
        ( ANY_EXPOSURE == exposure || NOT_EXPOSED == exposure ) )
      total += this->weighted_count[rr][adult_male_diseased_not_exposed];
    if( ( ANY_AGE == age || ADULT == age ) &&
        ( ANY_SEX == sex || MALE == sex ) &&
        ( ANY_STATE == state || DISEASED == state ) &&
        ( ANY_EXPOSURE == exposure || EXPOSED == exposure ) )
      total += this->weighted_count[rr][adult_male_diseased_exposed];
    if( ( ANY_AGE == age || ADULT == age ) &&
        ( ANY_SEX == sex || FEMALE == sex ) &&
        ( ANY_STATE == state || HEALTHY == state ) &&
        ( ANY_EXPOSURE == exposure || NOT_EXPOSED == exposure ) )
      total += this->weighted_count[rr][adult_female_healthy_not_exposed];
    if( ( ANY_AGE == age || ADULT == age ) &&
        ( ANY_SEX == sex || FEMALE == sex ) &&
        ( ANY_STATE == state || HEALTHY == state ) &&
        ( ANY_EXPOSURE == exposure || EXPOSED == exposure ) )
      total += this->weighted_count[rr][adult_female_healthy_exposed];
    if( ( ANY_AGE == age || ADULT == age ) &&
        ( ANY_SEX == sex || FEMALE == sex ) &&
        ( ANY_STATE == state || DISEASED == state ) &&
        ( ANY_EXPOSURE == exposure || NOT_EXPOSED == exposure ) )
      total += this->weighted_count[rr][adult_female_diseased_not_exposed];
    if( ( ANY_AGE == age || ADULT == age ) &&
        ( ANY_SEX == sex || FEMALE == sex ) &&
        ( ANY_STATE == state || DISEASED == state ) &&
        ( ANY_EXPOSURE == exposure || EXPOSED == exposure ) )
      total += this->weighted_count[rr][adult_female_diseased_exposed];
    if( ( ANY_AGE == age || CHILD == age ) &&
        ( ANY_SEX == sex || MALE == sex ) &&
        ( ANY_STATE == state || HEALTHY == state ) &&
        ( ANY_EXPOSURE == exposure || NOT_EXPOSED == exposure ) )
      total += this->weighted_count[rr][child_male_healthy_not_exposed];
    if( ( ANY_AGE == age || CHILD == age ) &&
        ( ANY_SEX == sex || MALE == sex ) &&
        ( ANY_STATE == state || HEALTHY == state ) &&
        ( ANY_EXPOSURE == exposure || EXPOSED == exposure ) )
      total += this->weighted_count[rr][child_male_healthy_exposed];
    if( ( ANY_AGE == age || CHILD == age ) &&
        ( ANY_SEX == sex || MALE == sex ) &&
        ( ANY_STATE == state || DISEASED == state ) &&
        ( ANY_EXPOSURE == exposure || NOT_EXPOSED == exposure ) )
      total += this->weighted_count[rr][child_male_diseased_not_exposed];
    if( ( ANY_AGE == age || CHILD == age ) &&
        ( ANY_SEX == sex || MALE == sex ) &&
        ( ANY_STATE == state || DISEASED == state ) &&
        ( ANY_EXPOSURE == exposure || EXPOSED == exposure ) )
      total += this->weighted_count[rr][child_male_diseased_exposed];
    if( ( ANY_AGE == age || CHILD == age ) &&
        ( ANY_SEX == sex || FEMALE == sex ) &&
        ( ANY_STATE == state || HEALTHY == state ) &&
        ( ANY_EXPOSURE == exposure || NOT_EXPOSED == exposure ) )
      total += this->weighted_count[rr][child_female_healthy_not_exposed];
    if( ( ANY_AGE == age || CHILD == age ) &&
        ( ANY_SEX == sex || FEMALE == sex ) &&
        ( ANY_STATE == state || HEALTHY == state ) &&
        ( ANY_EXPOSURE == exposure || EXPOSED == exposure ) )
      total += this->weighted_count[rr][child_female_healthy_exposed];
    if( ( ANY_AGE == age || CHILD == age ) &&
        ( ANY_SEX == sex || FEMALE == sex ) &&
        ( ANY_STATE == state || DISEASED == state ) &&
        ( ANY_EXPOSURE == exposure || NOT_EXPOSED == exposure ) )
      total += this->weighted_count[rr][child_female_diseased_not_exposed];
    if( ( ANY_AGE == age || CHILD == age ) &&
        ( ANY_SEX == sex || FEMALE == sex ) &&
        ( ANY_STATE == state || DISEASED == state ) &&
        ( ANY_EXPOSURE == exposure || EXPOSED == exposure ) )
      total += this->weighted_count[rr][child_female_diseased_exposed];

    return total;
  }
  
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double summary::get_relative_risk(
    const unsigned int rr,
    const age_type age,
    const sex_type sex ) const
  {
    double a = this->get_count( rr, age, sex, DISEASED, EXPOSED ),
           b = this->get_count( rr, age, sex, HEALTHY, EXPOSED ),
           c = this->get_count( rr, age, sex, DISEASED, NOT_EXPOSED ),
           d = this->get_count( rr, age, sex, HEALTHY, NOT_EXPOSED );
    return ( a / (a+b) ) / ( c / (c+d) );
  }
  
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double summary::get_count_fraction(
    const unsigned int rr,
    const age_type age,
    const sex_type sex ) const
  {
    return static_cast< double >( this->get_count( rr, age, sex, DISEASED ) ) /
           static_cast< double >( this->get_count( rr, age, sex, ANY_STATE ) );
  }
  
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double summary::get_weighted_relative_risk(
    const unsigned int rr,
    const age_type age,
    const sex_type sex ) const
  {
    double a = this->get_weighted_count( rr, age, sex, DISEASED, EXPOSED ),
           b = this->get_weighted_count( rr, age, sex, HEALTHY, EXPOSED ),
           c = this->get_weighted_count( rr, age, sex, DISEASED, NOT_EXPOSED ),
           d = this->get_weighted_count( rr, age, sex, HEALTHY, NOT_EXPOSED );
    return ( a / (a+b) ) / ( c / (c+d) );
  }
  
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double summary::get_weighted_count_fraction(
    const unsigned int rr,
    const age_type age,
    const sex_type sex ) const
  {
    return this->get_weighted_count( rr, age, sex, DISEASED ) /
           this->get_weighted_count( rr, age, sex, ANY_STATE );
  }
  
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void summary::add( summary *sum )
  {
    for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ )
    {
      for( unsigned int i = 0; i < 16; i++ )
      {
        this->count[rr][i] += sum->count[rr][i];
        this->weighted_count[rr][i] += sum->weighted_count[rr][i];
      }
    }
  }
  void summary::add( model_object *model ) { this->add( &(model->sum) ); }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void summary::write( std::ostream &stream ) const
  {
    stream << "type,group,individual_rr,diseased,total,rr,prevalence" << std::endl;

    for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ )
    {
      stream << "population,total"
             << "," << utilities::rr[rr]
             << "," << this->get_count( rr, ANY_AGE, ANY_SEX, DISEASED )
             << "," << this->get_count( rr )
             << "," << this->get_relative_risk( rr )
             << "," << this->get_count_fraction( rr ) << std::endl;

      stream << "population," << sampsim::get_age_type_name( ADULT )
             << "," << utilities::rr[rr]
             << "," << this->get_count( rr, ADULT, ANY_SEX, DISEASED )
             << "," << this->get_count( rr, ADULT )
             << "," << this->get_relative_risk( rr, ADULT )
             << "," << this->get_count_fraction( rr, ADULT ) << std::endl;
      stream << "population," << sampsim::get_age_type_name( CHILD )
             << "," << utilities::rr[rr]
             << "," << this->get_count( rr, CHILD, ANY_SEX, DISEASED )
             << "," << this->get_count( rr, CHILD )
             << "," << this->get_relative_risk( rr, CHILD )
             << "," << this->get_count_fraction( rr, CHILD ) << std::endl;

      stream << "population," << sampsim::get_sex_type_name( MALE )
             << "," << utilities::rr[rr]
             << "," << this->get_count( rr, ANY_AGE, MALE, DISEASED )
             << "," << this->get_count( rr, ANY_AGE, MALE )
             << "," << this->get_relative_risk( rr, ANY_AGE, MALE )
             << "," << this->get_count_fraction( rr, ANY_AGE, MALE ) << std::endl;
      stream << "population," << sampsim::get_sex_type_name( FEMALE )
             << "," << utilities::rr[rr]
             << "," << this->get_count( rr, ANY_AGE, FEMALE, DISEASED )
             << "," << this->get_count( rr, ANY_AGE, FEMALE )
             << "," << this->get_relative_risk( rr, ANY_AGE, FEMALE )
             << "," << this->get_count_fraction( rr, ANY_AGE, FEMALE ) << std::endl;

      stream << "population," << sampsim::get_age_type_name( ADULT ) << " " << sampsim::get_sex_type_name( MALE )
             << "," << utilities::rr[rr]
             << "," << this->get_count( rr, ADULT, MALE, DISEASED )
             << "," << this->get_count( rr, ADULT, MALE, ANY_STATE )
             << "," << this->get_relative_risk( rr, ADULT, MALE )
             << "," << this->get_count_fraction( rr, ADULT, MALE ) << std::endl;
      stream << "population," << sampsim::get_age_type_name( ADULT ) << " " << sampsim::get_sex_type_name( FEMALE )
             << "," << utilities::rr[rr]
             << "," << this->get_count( rr, ADULT, FEMALE, DISEASED )
             << "," << this->get_count( rr, ADULT, FEMALE, ANY_STATE )
             << "," << this->get_relative_risk( rr, ADULT, FEMALE )
             << "," << this->get_count_fraction( rr, ADULT, FEMALE ) << std::endl;
      stream << "population," << sampsim::get_age_type_name( CHILD ) << " " << sampsim::get_sex_type_name( MALE )
             << "," << utilities::rr[rr]
             << "," << this->get_count( rr, CHILD, MALE, DISEASED )
             << "," << this->get_count( rr, CHILD, MALE, ANY_STATE )
             << "," << this->get_relative_risk( rr, CHILD, MALE )
             << "," << this->get_count_fraction( rr, CHILD, MALE ) << std::endl;
      stream << "population," << sampsim::get_age_type_name( CHILD ) << " " << sampsim::get_sex_type_name( FEMALE )
             << "," << utilities::rr[rr]
             << "," << this->get_count( rr, CHILD, FEMALE, DISEASED )
             << "," << this->get_count( rr, CHILD, FEMALE, ANY_STATE )
             << "," << this->get_relative_risk( rr, CHILD, FEMALE )
             << "," << this->get_count_fraction( rr, CHILD, FEMALE ) << std::endl;
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void summary::write( std::vector< summary* > summary_list, bool weighted, std::ostream &stream )
  {
    stream << "type,group,individual_rr,diseased,total,rr,rr_stdev,prevalence,stdev";
    if( weighted ) stream << ",wrr,wrr_stdev,wprevalence,wstdev";
    stream << std::endl;

    sampsim::summary total_summary;
    double mean_array[utilities::rr.size()][9],
           stdev_array[utilities::rr.size()][9],
           squared_sum_array[utilities::rr.size()][9],
           wmean_array[utilities::rr.size()][9],
           wstdev_array[utilities::rr.size()][9],
           wsquared_sum_array[utilities::rr.size()][9],
           rr_mean_array[utilities::rr.size()][9],
           rr_stdev_array[utilities::rr.size()][9],
           rr_squared_sum_array[utilities::rr.size()][9],
           rr_wmean_array[utilities::rr.size()][9],
           rr_wstdev_array[utilities::rr.size()][9],
           rr_wsquared_sum_array[utilities::rr.size()][9];

    // add up all summaries in the list and store in total_summary
    for( auto summary_it = summary_list.begin(); summary_it != summary_list.end(); ++summary_it )
      total_summary.add( *summary_it );

    // determine standard deviations for all prevalences
    age_type a;
    sex_type s;
    for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ )
    {
      for( unsigned int index = 0; index < 9; index++ )
      {
        if( 0 <= index && index < 3 ) a = ANY_AGE;
        else if( 3 <= index && index < 6 ) a = ADULT;
        else if( 6 <= index && index < 9 ) a = CHILD;

        if( 0 == index % 3 ) s = ANY_SEX;
        else if( 1 == index % 3 ) s = MALE;
        else if( 2 == index % 3 ) s = FEMALE;

        double diff;
        rr_mean_array[rr][index] = total_summary.get_relative_risk( rr, a, s );
        mean_array[rr][index] = total_summary.get_count_fraction( rr, a, s );
        if( weighted )
        {
          rr_wmean_array[rr][index] = total_summary.get_weighted_relative_risk( rr, a, s );
          wmean_array[rr][index] = total_summary.get_weighted_count_fraction( rr, a, s );
        }

        for( auto summary_it = summary_list.cbegin(); summary_it != summary_list.cend(); ++summary_it )
        {
          summary *sum = *summary_it;
          diff = sum->get_count_fraction( rr, a, s ) - mean_array[rr][index];
          squared_sum_array[rr][index] += diff*diff;
          diff = sum->get_relative_risk( rr, a, s ) - rr_mean_array[rr][index];
          rr_squared_sum_array[rr][index] += diff*diff;
          if( weighted )
          {
            diff = sum->get_weighted_count_fraction( rr, a, s ) - wmean_array[rr][index];
            wsquared_sum_array[rr][index] += diff*diff;
            diff = sum->get_relative_risk( rr, a, s ) - rr_wmean_array[rr][index];
            rr_wsquared_sum_array[rr][index] += diff*diff;
          }
        }
        stdev_array[rr][index] = sqrt(
          squared_sum_array[rr][index] / static_cast<double>( summary_list.size() - 1 )
        );
        rr_stdev_array[rr][index] = sqrt(
          rr_squared_sum_array[rr][index] / static_cast<double>( summary_list.size() - 1 )
        );
        if( weighted )
        {
          wstdev_array[rr][index] = sqrt(
            wsquared_sum_array[rr][index] / static_cast<double>( summary_list.size() - 1 )
          );
          rr_wstdev_array[rr][index] = sqrt(
            rr_wsquared_sum_array[rr][index] / static_cast<double>( summary_list.size() - 1 )
          );
        }
      }

      stream << "sample,total," << utilities::rr[rr]
             << "," << total_summary.get_count( rr, ANY_AGE, ANY_SEX, DISEASED )
             << "," << total_summary.get_count( rr, ANY_AGE, ANY_SEX )
             << "," << rr_mean_array[rr][0] << "," << rr_stdev_array[rr][0]
             << "," << mean_array[rr][0] << "," << stdev_array[rr][0];
      if( weighted ) stream << "," << rr_wmean_array[rr][0] << "," << rr_wstdev_array[rr][0]
                            << "," << wmean_array[rr][0] << "," << wstdev_array[rr][0];
      stream << std::endl;

      stream << "sample,adult," << utilities::rr[rr]
             << "," << total_summary.get_count( rr, ADULT, ANY_SEX, DISEASED )
             << "," << total_summary.get_count( rr, ADULT, ANY_SEX )
             << "," << rr_mean_array[rr][3] << "," << rr_stdev_array[rr][3]
             << "," << mean_array[rr][3] << "," << stdev_array[rr][3];
      if( weighted ) stream << "," << rr_wmean_array[rr][3] << "," << rr_wstdev_array[rr][3]
                            << "," << wmean_array[rr][3] << "," << wstdev_array[rr][3];
      stream << std::endl;

      stream << "sample,child," << utilities::rr[rr]
             << "," << total_summary.get_count( rr, CHILD, ANY_SEX, DISEASED )
             << "," << total_summary.get_count( rr, CHILD, ANY_SEX )
             << "," << rr_mean_array[rr][6] << "," << rr_stdev_array[rr][6]
             << "," << mean_array[rr][6] << "," << stdev_array[rr][6];
      if( weighted ) stream << "," << rr_wmean_array[rr][6] << "," << rr_wstdev_array[rr][6]
                            << "," << wmean_array[rr][6] << "," << wstdev_array[rr][6];
      stream << std::endl;

      stream << "sample,male," << utilities::rr[rr]
             << "," << total_summary.get_count( rr, ANY_AGE, MALE, DISEASED )
             << "," << total_summary.get_count( rr, ANY_AGE, MALE )
             << "," << rr_mean_array[rr][1] << "," << rr_stdev_array[rr][1]
             << "," << mean_array[rr][1] << "," << stdev_array[rr][1];
      if( weighted ) stream << "," << rr_wmean_array[rr][1] << "," << rr_wstdev_array[rr][1]
                            << "," << wmean_array[rr][1] << "," << wstdev_array[rr][1];
      stream << std::endl;

      stream << "sample,female," << utilities::rr[rr]
             << "," << total_summary.get_count( rr, ANY_AGE, FEMALE, DISEASED )
             << "," << total_summary.get_count( rr, ANY_AGE, FEMALE )
             << "," << rr_mean_array[rr][2] << "," << rr_stdev_array[rr][2]
             << "," << mean_array[rr][2] << "," << stdev_array[rr][2];
      if( weighted ) stream << "," << rr_wmean_array[rr][2] << "," << rr_wstdev_array[rr][2]
                            << "," << wmean_array[rr][2] << "," << wstdev_array[rr][2];
      stream << std::endl;

      stream << "sample,male adult," << utilities::rr[rr]
             << "," << total_summary.get_count( rr, ADULT, MALE, DISEASED )
             << "," << total_summary.get_count( rr, ADULT, MALE )
             << "," << rr_mean_array[rr][4] << "," << rr_stdev_array[rr][4]
             << "," << mean_array[rr][4] << "," << stdev_array[rr][4];
      if( weighted ) stream << "," << rr_wmean_array[rr][4] << "," << rr_wstdev_array[rr][4]
                            << "," << wmean_array[rr][4] << "," << wstdev_array[rr][4];
      stream << std::endl;

      stream << "sample,female adult," << utilities::rr[rr]
             << "," << total_summary.get_count( rr, ADULT, FEMALE, DISEASED )
             << "," << total_summary.get_count( rr, ADULT, FEMALE )
             << "," << rr_mean_array[rr][5] << "," << rr_stdev_array[rr][5]
             << "," << mean_array[rr][5] << "," << stdev_array[rr][5];
      if( weighted ) stream << "," << rr_wmean_array[rr][5] << "," << rr_wstdev_array[rr][5]
                            << "," << wmean_array[rr][5] << "," << wstdev_array[rr][5];
      stream << std::endl;

      stream << "sample,male child," << utilities::rr[rr]
             << "," << total_summary.get_count( rr, CHILD, MALE, DISEASED )
             << "," << total_summary.get_count( rr, CHILD, MALE )
             << "," << rr_mean_array[rr][7] << "," << rr_stdev_array[rr][7]
             << "," << mean_array[rr][7] << "," << stdev_array[rr][7];
      if( weighted ) stream << "," << rr_wmean_array[rr][7] << "," << rr_wstdev_array[rr][7]
                            << "," << wmean_array[rr][7] << "," << wstdev_array[rr][7];
      stream << std::endl;

      stream << "sample,female child," << utilities::rr[rr]
             << "," << total_summary.get_count( rr, CHILD, FEMALE, DISEASED )
             << "," << total_summary.get_count( rr, CHILD, FEMALE )
             << "," << rr_mean_array[rr][8] << "," << rr_stdev_array[rr][8]
             << "," << mean_array[rr][8] << "," << stdev_array[rr][8];
      if( weighted ) stream << "," << rr_wmean_array[rr][8] << "," << rr_wstdev_array[rr][8]
                            << "," << wmean_array[rr][8] << "," << wstdev_array[rr][8];
      stream << std::endl;
    }
  }
}
