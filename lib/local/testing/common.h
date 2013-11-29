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
  stringstream stream;
  stream << time( NULL );
  population->set_seed( stream.str() );
  population->set_mean_household_population( 4 );
  population->set_tile_width( 1 );
  population->set_number_tiles_x( 3 );
  population->set_number_tiles_y( 3 );
  population->get_population_density()->set_b00( 1 );
  population->generate();
}

#endif // __common_h
