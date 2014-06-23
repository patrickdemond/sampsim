/*=========================================================================

  Program:  sampsim
  Module:   test_pareto.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the pareto class
//

#include "UnitTest++.h"

#include "pareto.h"
#include "utilities.h"

using namespace std;

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_pareto )
{
  sampsim::pareto p1( 1.5, 1, 100 );
  cout << "Testing pareto for [" << p1.b() << "," << p1.max()
       << "] and shape factor of " << p1.a() << " ..." << endl;
  for( int i = 0; i < 1000; ++i )
  {
    double value = p1( sampsim::utilities::random_engine );
    CHECK( value >= p1.b() );
    CHECK( value <= p1.max() );
  }

  sampsim::pareto p2( 500, 0.1, 10000 );
  cout << "Testing pareto for [" << p2.b() << "," << p2.max()
       << "] and shape factor of " << p2.a() << " ..." << endl;
  for( int i = 0; i < 1000; ++i )
  {
    double value = p2( sampsim::utilities::random_engine );
    CHECK( value >= p2.b() );
    CHECK( value <= p2.max() );
  }

  sampsim::pareto p3( 10, 5, 1000000 );
  cout << "Testing pareto for [" << p3.b() << "," << p3.max()
       << "] and shape factor of " << p3.a() << " ..." << endl;
  for( int i = 0; i < 1000; ++i )
  {
    double value = p3( sampsim::utilities::random_engine );
    CHECK( value >= p3.b() );
    CHECK( value <= p3.max() );
  }
}
