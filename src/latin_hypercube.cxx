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
  opts.add_option( 'm', "maximums", vector< string >(), "The maximum number of divisions for each dimension" );
  opts.add_flag( 'i', "index0", "Whether to output 0-based indeces instead of values between 0 and 1" );
  opts.add_flag( 'j', "index1", "Whether to output 1-based indeces instead of values between 0 and 1" );

  try
  {
    opts.set_arguments( argc, argv );
    if( opts.process() )
    {
      status = EXIT_SUCCESS;

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
        vector< int > maximums = opts.get_option_as_int_list( "maximums" );
        bool index0 = opts.get_flag( "index0" );
        bool index1 = opts.get_flag( "index1" );

        std::vector<double> data( dims*points, 0.0 );
        latin_center( dims, points, &seed, data.data() );

        // make sure the number of maximums (if any) matches the number of dimensions
        if( 0 < maximums.size() && maximums.size() != dims )
        {
          cerr << "ERROR: The number of maximum divisions does not equal the number of dimensions." << endl
               << "If you wish to set the maximum divisions please make sure to provide a comma-separated "
               << "list with one number per dimesion indicated by the \"dims\" option." << endl;
          status = EXIT_FAILURE;
        }
        else
        {
          for( int pnt = 0; pnt < points; pnt++ )
          {
            for( int dim = 0; dim < dims; dim++ )
            {
              // get the point and convert if the maximum number of divisions for this dimension is set
              double val = data[pnt + points*dim];
              double factor = points;
              if( 0 < maximums.size() && maximums[dim] != points )
              {
                double m = static_cast<double>( maximums[dim] );
                double p = static_cast<double>( points );
                val = ceil( (val + 1/(2*p)) * m - 1E-10)/m - 1/(2*m);
                factor = m;
              }

              if( 0 < dim ) cout << ",";
              if( index0 || index1 )
              {
                int index = round( factor * ( val - 1/(2.0*factor) ) );
                cout << ( index0 ? index : index+1 );
              }
              else cout << val;
            }
            cout << endl;
          }
        }
      }
    }
  }
  catch( std::exception &e )
  {
    std::cerr << "Uncaught exception: " << e.what() << std::endl;
  }

  return status;
}
