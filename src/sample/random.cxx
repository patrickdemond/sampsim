/*=========================================================================

  Program:  sampsim
  Module:   random.cxx
  Language: C++

=========================================================================*/

#include "random.h"

#include "utilities.h"

#include <stdexcept>

namespace sampsim
{
namespace sample
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void random::generate()
  {
    utilities::output( "generating random sample" );

    // check to make sure the age and sex restrictions are valid
    if( UNKNOWN_AGE_TYPE == this->get_age_type() )
      throw std::runtime_error( "Cannot generate random, age type is unknown" );
    if( UNKNOWN_SEX_TYPE == this->get_sex_type() )
      throw std::runtime_error( "Cannot generate random, sex type is unknown" );

    utilities::random_engine.seed( atoi( this->get_seed().c_str() ) );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void random::from_json( const Json::Value &json )
  {
    sample::from_json( json );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void random::to_json( Json::Value &json ) const
  {
    sample::to_json( json );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void random::to_csv( std::ostream &household_stream, std::ostream &individual_stream ) const
  {
    sample::to_csv( household_stream, individual_stream );
  }
}
}
