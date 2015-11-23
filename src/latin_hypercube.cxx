/*=========================================================================

  Program:  sampsim
  Module:   latin_hypercube.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// An executable which generates random latin hypercube samples
//

#include "options.h"

#include <cmath>
#include <iostream>
#include <vector>

#include "latin_center.hpp"

using namespace std;

int main( const int argc, const char** argv )
{
  int status = EXIT_FAILURE;
  sampsim::options opts( argv[0] );
  opts.add_option( 's', "seed", "1", "Seed used by the random generator" );
  opts.add_option( 'd', "dims", "2", "The number of dimensions to generate" );
  opts.add_option( 'p', "points", "10", "The number of points to generate" );
  opts.add_flag( 'i', "index", "Whether to output indeces instead of values between 0 and 1" );
  opts.add_flag( 't', "transpose", "Whether to transpose the output" );

  try
  {
    opts.set_arguments( argc, argv );
    if( opts.process() )
    {
      // now either show the help or run the application
      if( opts.get_flag( "help" ) )
      {
        opts.print_usage();
      }
      else
      {
        int seed = opts.get_option_as_int( "seed" );
        int dims = opts.get_option_as_int( "dims" );
        int points = opts.get_option_as_int( "points" );
        bool index = opts.get_flag( "index" );
        bool transpose = opts.get_flag( "transpose" );

        std::vector<double> data( dims*points, 0.0 );
        latin_center( dims, points, &seed, data.data() );

        if( transpose )
        {
          for( int dim = 0; dim < dims; dim++ )
          {
            for( int pnt = 0; pnt < points; pnt++ )
            {
              if( 0 < pnt ) cout << ",";
              if( index ) cout << round( points * ( data[pnt + points*dim] - 1/(2.0*points) ) );
              else cout << data[pnt + points*dim];
            }
            cout << endl;
          }
        }
        else
        {
          for( int pnt = 0; pnt < points; pnt++ )
          {
            for( int dim = 0; dim < dims; dim++ )
            {
              if( 0 < dim ) cout << ",";
              if( index ) cout << round( points * ( data[pnt + points*dim] - 1/(2.0*points) ) );
              else cout << data[pnt + points*dim];
            }
            cout << endl;
          }
        }
      }

      status = EXIT_SUCCESS;
    }
  }
  catch( std::exception &e )
  {
    std::cerr << "Uncaught exception: " << e.what() << std::endl;
  }

  return status;
}
