/*=========================================================================

  Program:  sampsim
  Module:   enumeration_sample.h
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Functions used to set up and parse options for an enumeration sample
//

#include "options.h"
#include "sample/enumeration.h"
#include "utilities.h"

// no additional parameters
void setup_enumeration_sample( sampsim::options &opts )
{
  opts.add_option( "target_size", "50", "The target number of individuals in each enumeration." );
}

void parse_enumeration_sample( sampsim::options &opts, sampsim::sample::enumeration *sample )
{
  sample->set_target_size( opts.get_option_as_int( "target_size" ) );
}
