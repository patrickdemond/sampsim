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

#include <algorithm>
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
  opts.add_flag( 'u', "unique", "Whether to force all points to be unique" );

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
        bool is_unique = opts.get_flag( "unique" );

        // make sure the number of maximums (if any) matches the number of dimensions
        if( 0 < maximums.size() && maximums.size() != dims )
        {
          cerr << "ERROR: The number of maximum divisions does not equal the number of dimensions." << endl
               << "If you wish to set the maximum divisions please make sure to provide a comma-separated "
               << "list with one number per dimesion indicated by the \"dims\" option." << endl;
          status = EXIT_FAILURE;
        }
        // if maximums and unique were selected, make sure there are enough points available
        else if( 0 < maximums.size() && is_unique )
        {
          int max_unique_points = accumulate( begin( maximums ), end( maximums ), 1, multiplies<int>() );

          // if the maximum is VERY high then it will be negative (so we can ignore the following test)
          if( 0 <= max_unique_points && points > max_unique_points )
          {
            cout << "points: " << points << ", max_unique_points: " << max_unique_points << endl;
            cerr << "ERROR: Impossible to generate " << points << " unique points." << endl
                 << "The maximum number of unique points is limited to the product of the maximum number of "
                 << "divisions (" << max_unique_points << ").  Make sure that the number of requested points "
                 << "is equal to or smaller." << endl;
            status = EXIT_FAILURE;
          }
        }

        if( EXIT_SUCCESS == status )
        {
          bool complete = false;
          vector<double> data( dims*points, 0.0 );
          vector<string> point_list( points, "" );
          auto it = point_list.begin();

          while( !complete )
          {
            latin_center( dims, points, &seed, data.data() );
            for( int pnt = 0; pnt < points && it != point_list.end(); pnt++ )
            {
              it->clear();
              for( int dim = 0; dim < dims; dim++ )
              {
                if( 0 < dim ) it->append( "," );

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

                if( index0 || index1 )
                {
                  // convert value to 0 or 1-based index
                  int index = round( factor * ( val - 1/(2.0*factor) ) );
                  it->append( to_string( index0 ? index : index+1 ) );
                }
                else
                {
                  it->append( to_string( val ) );
                }
              }

              it++; // move to the list's next point
            }

            sort( point_list.begin(), point_list.end() );

            if( is_unique )
            {
              // remove non-unique values
              it = unique( point_list.begin(), point_list.end() );
              if( it == point_list.end() ) complete = true;
            }
            else complete = true;
          }

          for( int pnt = 0; pnt < points; pnt++ ) cout << point_list[pnt] << endl;
        }
      }
    }
  }
  catch( exception &e )
  {
    cerr << "Uncaught exception: " << e.what() << endl;
  }

  return status;
}
