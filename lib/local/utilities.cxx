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
  clock_t sampsim::utilities::start_time = clock();
}
