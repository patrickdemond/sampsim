/*=========================================================================

  Program:  sampsim
  Module:   common.h
  Language: C++

  Author: Patrick Emond <emondpd@mcmaster.ca>

=========================================================================*/
//
// .SECTION Description
// Common functions used by unit tests.
//

#ifndef __sampsim_common_h
#define __sampsim_common_h

#include "population.h"
#include "trend.h"
#include "utilities.h"

#include <sstream>

using namespace std;

inline void create_test_population( sampsim::population *population )
{
  sampsim::utilities::verbose = true;
  stringstream stream;
  stream << time( NULL );
  population->set_seed( stream.str() );
  population->set_number_of_towns( 4 );
  population->set_town_size_min( 10000 );
  population->set_town_size_max( 1000000 );
  population->set_town_size_shape( 1.0 );
  population->set_mean_household_population( 4 );
  population->set_tile_width( 1 );
  population->set_number_of_tiles_x( 10 );
  population->set_number_of_tiles_y( 10 );
  population->set_number_of_disease_pockets( 3 );
  population->generate();
}

#endif // __common_h
