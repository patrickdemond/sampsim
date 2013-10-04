/*=========================================================================

  Program:  sampsim
  Module:   test_trend.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the trend class
//

#include "UnitTest++.h"

#include "trend.h"
#include "utilities.h"

using namespace std;

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_trend )
{
  sampsim::trend t1( 1, 2, 3, 4, 5, 6 );

  cout << "Testing trend copying..." << endl;
  sampsim::trend t2;
  t2.copy( &t1 );
  CHECK_EQUAL( t1.get_b00(), t2.get_b00() );
  CHECK_EQUAL( t1.get_b01(), t2.get_b01() );
  CHECK_EQUAL( t1.get_b10(), t2.get_b10() );
  CHECK_EQUAL( t1.get_b02(), t2.get_b02() );
  CHECK_EQUAL( t1.get_b20(), t2.get_b20() );
  CHECK_EQUAL( t1.get_b11(), t2.get_b11() );

  for( int i = 0; i < 1000; ++i )
  {
    sampsim::coordinate c( 100 * ( sampsim::utilities::random() - 0.5 ),
                           100 * ( sampsim::utilities::random() - 0.5 ) );
    cout << "Testing trend value at (" << c.x << ", " << c.y << ")..." << endl;
    double value = t1.get_b00() +
                   t1.get_b01() * c.x +
                   t1.get_b10() * c.y +
                   t1.get_b02() * c.x * c.x +
                   t1.get_b20() * c.y * c.y +
                   t1.get_b11() * c.x * c.y;
    CHECK_EQUAL( value, t1.get_value( c ) );
  }
}
