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
      this->count[rr] = std::array< unsigned, 8 >{ 0, 0, 0, 0, 0, 0, 0, 0 };
      this->weighted_count[rr] = std::array< double, 8 >{ 0, 0, 0, 0, 0, 0, 0, 0 };
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  unsigned int summary::get_count(
    const unsigned int rr,
    const age_type age,
    const sex_type sex,
    const state_type state ) const
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
  }
  
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  double summary::get_weighted_count(
    const unsigned int rr,
    const age_type age,
    const sex_type sex,
    const state_type state ) const
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
      for( unsigned int i = 0; i < 8; i++ )
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
    for( unsigned int rr = 0; rr < utilities::rr.size(); rr++ )
    {
      stream << "total"
             << " RR" << utilities::rr[rr] << " count: "
             << this->get_count( rr, ANY_AGE, ANY_SEX, DISEASED )
             << " diseased of " << this->get_count( rr ) << " total "
             << "(prevalence " << this->get_count_fraction( rr ) << ")" << std::endl;

      stream << sampsim::get_age_type_name( ADULT )
             << " RR" << utilities::rr[rr] << " count: "
             << this->get_count( rr, ADULT, ANY_SEX, DISEASED )
             << " diseased of " << this->get_count( rr, ADULT ) << " total "
             << "(prevalence " << this->get_count_fraction( rr, ADULT ) << ")" << std::endl;
      stream << sampsim::get_age_type_name( CHILD )
             << " RR" << utilities::rr[rr] << " count: "
             << this->get_count( rr, CHILD, ANY_SEX, DISEASED )
             << " diseased of " << this->get_count( rr, CHILD ) << " total "
             << "(prevalence " << this->get_count_fraction( rr, CHILD ) << ")" << std::endl;

      stream << sampsim::get_sex_type_name( MALE )
             << " RR" << utilities::rr[rr] << " count: "
             << this->get_count( rr, ANY_AGE, MALE, DISEASED )
             << " diseased of " << this->get_count( rr, ANY_AGE, MALE ) << " total "
             << "(prevalence " << this->get_count_fraction( rr, ANY_AGE, MALE ) << ")" << std::endl;
      stream << sampsim::get_sex_type_name( FEMALE )
             << " RR" << utilities::rr[rr] << " count: "
             << this->get_count( rr, ANY_AGE, FEMALE, DISEASED )
             << " diseased of " << this->get_count( rr, ANY_AGE, FEMALE ) << " total "
             << "(prevalence " << this->get_count_fraction( rr, ANY_AGE, FEMALE ) << ")" << std::endl;

      stream << sampsim::get_age_type_name( ADULT ) << " " << sampsim::get_sex_type_name( MALE )
             << " RR" << utilities::rr[rr] << " count: "
             << this->get_count( rr, ADULT, MALE, DISEASED )
             << " diseased of " << this->get_count( rr, ADULT, MALE, ANY_STATE ) << " total "
             << "(prevalence " << this->get_count_fraction( rr, ADULT, MALE ) << ")" << std::endl;
      stream << sampsim::get_age_type_name( ADULT ) << " " << sampsim::get_sex_type_name( FEMALE )
             << " RR" << utilities::rr[rr] << " count: "
             << this->get_count( rr, ADULT, FEMALE, DISEASED )
             << " diseased of " << this->get_count( rr, ADULT, FEMALE, ANY_STATE ) << " total "
             << "(prevalence " << this->get_count_fraction( rr, ADULT, FEMALE ) << ")" << std::endl;
      stream << sampsim::get_age_type_name( CHILD ) << " " << sampsim::get_sex_type_name( MALE )
             << " RR" << utilities::rr[rr] << " count: "
             << this->get_count( rr, CHILD, MALE, DISEASED )
             << " diseased of " << this->get_count( rr, CHILD, MALE, ANY_STATE ) << " total "
             << "(prevalence " << this->get_count_fraction( rr, CHILD, MALE ) << ")" << std::endl;
      stream << sampsim::get_age_type_name( CHILD ) << " " << sampsim::get_sex_type_name( FEMALE )
             << " RR" << utilities::rr[rr] << " count: "
             << this->get_count( rr, CHILD, FEMALE, DISEASED )
             << " diseased of " << this->get_count( rr, CHILD, FEMALE, ANY_STATE ) << " total "
             << "(prevalence " << this->get_count_fraction( rr, CHILD, FEMALE ) << ")" << std::endl;
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void summary::write( std::vector< summary* > summary_list, bool weighted, std::ostream &stream )
  {
    sampsim::summary total_summary;
    double mean_array[utilities::rr.size()][9],
           stdev_array[utilities::rr.size()][9],
           squared_sum_array[utilities::rr.size()][9];
    double wmean_array[utilities::rr.size()][9],
           wstdev_array[utilities::rr.size()][9],
           wsquared_sum_array[utilities::rr.size()][9];

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
        mean_array[rr][index] = total_summary.get_count_fraction( rr, a, s );
        if( weighted )
          wmean_array[rr][index] = total_summary.get_weighted_count_fraction( rr, a, s );

        for( auto summary_it = summary_list.cbegin(); summary_it != summary_list.cend(); ++summary_it )
        {
          summary *sum = *summary_it;
          diff = sum->get_count_fraction( rr, a, s ) - mean_array[rr][index];
          squared_sum_array[rr][index] += diff*diff;
          if( weighted )
          {
            diff = sum->get_weighted_count_fraction( rr, a, s ) - wmean_array[rr][index];
            wsquared_sum_array[rr][index] += diff*diff;
          }
        }
        stdev_array[rr][index] = sqrt(
          squared_sum_array[rr][index] / static_cast<double>( summary_list.size() - 1 )
        );
        if( weighted )
        {
          wstdev_array[rr][index] = sqrt(
            wsquared_sum_array[rr][index] / static_cast<double>( summary_list.size() - 1 )
          );
        }
      }

      stream << "sampled total RR" << utilities::rr[rr] << " count: "
             << total_summary.get_count( rr, ANY_AGE, ANY_SEX, DISEASED )
             << " diseased of " << total_summary.get_count( rr, ANY_AGE, ANY_SEX )
             << " total (prevalence " << mean_array[rr][0] << " (" << stdev_array[rr][0] << "))";
      if( weighted )
        stream << " (weighted prevalence " << wmean_array[rr][0] << " (" << wstdev_array[rr][0] << "))";
      stream << std::endl;

      stream << "sampled adult RR" << utilities::rr[rr] << " count: "
             << total_summary.get_count( rr, ADULT, ANY_SEX, DISEASED )
             << " diseased of " << total_summary.get_count( rr, ADULT, ANY_SEX )
             << " total (prevalence " << mean_array[rr][3] << " (" << stdev_array[rr][3] << "))";
      if( weighted )
        stream << " (weighted prevalence " << wmean_array[rr][3] << " (" << wstdev_array[rr][3] << "))";
      stream << std::endl;

      stream << "sampled child RR" << utilities::rr[rr] << " count: "
             << total_summary.get_count( rr, CHILD, ANY_SEX, DISEASED )
             << " diseased of " << total_summary.get_count( rr, CHILD, ANY_SEX )
             << " total (prevalence " << mean_array[rr][6] << " (" << stdev_array[rr][6] << "))";
      if( weighted )
        stream << " (weighted prevalence " << wmean_array[rr][6] << " (" << wstdev_array[rr][6] << "))";
      stream << std::endl;

      stream << "sampled male RR" << utilities::rr[rr] << " count: "
             << total_summary.get_count( rr, ANY_AGE, MALE, DISEASED )
             << " diseased of " << total_summary.get_count( rr, ANY_AGE, MALE )
             << " total (prevalence " << mean_array[rr][1] << " (" << stdev_array[rr][1] << "))";
      if( weighted )
        stream << " (weighted prevalence " << wmean_array[rr][1] << " (" << wstdev_array[rr][1] << "))";
      stream << std::endl;

      stream << "sampled female RR" << utilities::rr[rr] << " count: "
             << total_summary.get_count( rr, ANY_AGE, FEMALE, DISEASED )
             << " diseased of " << total_summary.get_count( rr, ANY_AGE, FEMALE )
             << " total (prevalence " << mean_array[rr][2] << " (" << stdev_array[rr][2] << "))";
      if( weighted )
        stream << " (weighted prevalence " << wmean_array[rr][2] << " (" << wstdev_array[rr][2] << "))";
      stream << std::endl;

      stream << "sampled male adult RR" << utilities::rr[rr] << " count: "
             << total_summary.get_count( rr, ADULT, MALE, DISEASED )
             << " diseased of " << total_summary.get_count( rr, ADULT, MALE )
             << " total (prevalence " << mean_array[rr][4] << " (" << stdev_array[rr][4] << "))";
      if( weighted )
        stream << " (weighted prevalence " << wmean_array[rr][4] << " (" << wstdev_array[rr][4] << "))";
      stream << std::endl;

      stream << "sampled female adult RR" << utilities::rr[rr] << " count: "
             << total_summary.get_count( rr, ADULT, FEMALE, DISEASED )
             << " diseased of " << total_summary.get_count( rr, ADULT, FEMALE )
             << " total (prevalence " << mean_array[rr][5] << " (" << stdev_array[rr][5] << "))";
      if( weighted )
        stream << " (weighted prevalence " << wmean_array[rr][5] << " (" << wstdev_array[rr][5] << "))";
      stream << std::endl;

      stream << "sampled male child RR" << utilities::rr[rr] << " count: "
             << total_summary.get_count( rr, CHILD, MALE, DISEASED )
             << " diseased of " << total_summary.get_count( rr, CHILD, MALE )
             << " total (prevalence " << mean_array[rr][7] << " (" << stdev_array[rr][7] << "))";
      if( weighted )
        stream << " (weighted prevalence " << wmean_array[rr][7] << " (" << wstdev_array[rr][7] << "))";
      stream << std::endl;

      stream << "sampled female child RR" << utilities::rr[rr] << " count: "
             << total_summary.get_count( rr, CHILD, FEMALE, DISEASED )
             << " diseased of " << total_summary.get_count( rr, CHILD, FEMALE )
             << " total (prevalence " << mean_array[rr][8] << " (" << stdev_array[rr][8] << "))";
      if( weighted )
        stream << " (weighted prevalence " << wmean_array[rr][8] << " (" << wstdev_array[rr][8] << "))";
      stream << std::endl;
    }
  }
}
