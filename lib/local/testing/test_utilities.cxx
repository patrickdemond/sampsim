/*=========================================================================

  Program:  sampsim
  Module:   test_utilities.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the utilities class
//

#include "UnitTest++.h"

#include "utilities.h"

using namespace std;

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_utilities )
{
  cout << "Testing the exec function..." << endl;
  CHECK_EQUAL( "3", sampsim::utilities::exec( "expr 1 + 2" ) );

  cout << "Testing the file_exists function..." << endl;
  string filename = sampsim::utilities::exec( "which ls" );
  CHECK( sampsim::utilities::file_exists( filename ) );
  CHECK( !sampsim::utilities::file_exists( "/6aY5GnCINoypXPB4ol004FNhiCgZ3h1x" ) );

  cout << "Testing the get_file_extension function..." << endl;
  CHECK_EQUAL( ".bar" , sampsim::utilities::get_file_extension( "foo.bar" ) );

  cout << "Testing the get_filename_path function..." << endl;
  CHECK_EQUAL( "/tmp/dir.here", sampsim::utilities::get_filename_path( "/tmp/dir.here/test.txt" ) );

  cout << "Testing the get_filename_name function..." << endl;
  CHECK_EQUAL( "test.txt", sampsim::utilities::get_filename_name( "/tmp/dir.here/test.txt" ) );

  cout << "Testing the explode function..." << endl;
  vector< std::string > parts = sampsim::utilities::explode( ":this:is:a:test", ":" );
  CHECK_EQUAL( "this", parts[0] );
  CHECK_EQUAL( "is", parts[1] );
  CHECK_EQUAL( "a", parts[2] );
  CHECK_EQUAL( "test", parts[3] );

  string test;
  cout << "Testing the ltrim function..." << endl;
  test = " \n\t test string \t\n ";
  CHECK_EQUAL( "test string \t\n ", sampsim::utilities::ltrim( test ) );

  cout << "Testing the rtrim function..." << endl;
  test = " \n\t test string \t\n ";
  CHECK_EQUAL( " \n\t test string", sampsim::utilities::rtrim( test ) );

  cout << "Testing the trim function..." << endl;
  test = " \n\t test string \t\n ";
  CHECK_EQUAL( "test string", sampsim::utilities::trim( test ) );
}
