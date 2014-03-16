/*=========================================================================

  Program:  sampsim
  Module:   test_coordinate.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the coordinate class
//

#include "UnitTest++.h"

#include "utilities.h"

using namespace std;

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_coordinate )
{
  sampsim::coordinate c, c1( 1, 1 ), c2( 1, 1 ), c3( 2, 2 );

  cout << "Testing coordinate equal..." << endl;
  CHECK( c1 == c2 );

  cout << "Testing coordinate not equal..." << endl;
  CHECK( c1 != c3 );

  cout << "Testing coordinate addition..." << endl;
  c = c1 + c3;
  CHECK_EQUAL( 3, c.x );
  CHECK_EQUAL( 3, c.y );
  c += c1;
  CHECK_EQUAL( 4, c.x );
  CHECK_EQUAL( 4, c.y );

  cout << "Testing coordinate subtraction..." << endl;
  c = c1 - c3;
  CHECK_EQUAL( -1, c.x );
  CHECK_EQUAL( -1, c.y );
  c -= c1;
  CHECK_EQUAL( -2, c.x );
  CHECK_EQUAL( -2, c.y );

  cout << "Testing coordinate multiplication..." << endl;
  c = c3 * 10;
  CHECK_EQUAL( 20, c.x );
  CHECK_EQUAL( 20, c.y );
  c *= 2;
  CHECK_EQUAL( 40, c.x );
  CHECK_EQUAL( 40, c.y );

  cout << "Testing coordinate division..." << endl;
  c = c1 / 2;
  CHECK_EQUAL( 0.5, c.x );
  CHECK_EQUAL( 0.5, c.y );
  c /= 2;
  CHECK_EQUAL( 0.25, c.x );
  CHECK_EQUAL( 0.25, c.y );

  cout << "Testing coordinate distance..." << endl;
  CHECK_EQUAL( sqrt( 2 ), c3.distance( c1 ) );

  cout << "Testing coordinate length..." << endl;
  CHECK_EQUAL( sqrt( 8 ), c3.length() );

  cout << "Testing polar coordinates..." << endl;
  c.x = 10;
  c.y = 10;
  c.set_centroid( sampsim::coordinate( 10, 0 ) );
  CHECK_EQUAL( 10, c.get_r() );
  CHECK_EQUAL( M_PI/2, c.get_a() );
}
