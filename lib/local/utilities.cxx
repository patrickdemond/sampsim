/*=========================================================================

  Program:  sampsim
  Module:   utilities.cxx
  Language: C++

=========================================================================*/

#include "utilities.h"

#include <ctime>
#include <random>

namespace sampsim
{
  std::mt19937 sampsim::utilities::random_engine;
  sampsim::utilities::safe_delete_type sampsim::utilities::safe_delete;
  bool sampsim::utilities::verbose = false; 
  bool sampsim::utilities::quiet = false; 
  unsigned int sampsim::utilities::write_sample_number = 1;
  clock_t sampsim::utilities::start_time = clock();

  // define how many diseases (and their relative-risk value) here
  double rr_values[] = { 1.0, 1.5, 2.0, 3.0 };
  std::vector< double > sampsim::utilities::rr( rr_values, rr_values + sizeof(rr_values)/sizeof(double) );
}
