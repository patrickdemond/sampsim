/*=========================================================================

  Program:  sampsim
  Module:   building_tree.cxx
  Language: C++

=========================================================================*/

#include "building_tree.h"

#include "building.h"

#include <stdexcept>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_tree::building_tree( building_list_type building_list )
  {
    this->root = building_tree::build( 0, building_list );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_tree::~building_tree()
  {
    building_tree::destroy( this->root );
    this->root = NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building* building_tree::find_nearest( coordinate c )
  {
    return building_tree::find( this->root, c.x, c.y )->building;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_tree::leaf* building_tree::build( long long int depth, building_list_type building_list )
  {
    leaf* node = new leaf( depth );
    int size = building_list.size();

    // sanity check
    if( 0 == size ) throw std::runtime_error( "Tried to build building_tree with an empty list" );
    // one building in the list means we've reached the end
    else if( 1 == size ) node->building = building_list.front();
    else
    {
      const building_list_type::iterator begin = building_list.begin();
      const building_list_type::iterator end = building_list.end();
      bool even = 0 == depth % 2;

      std::sort( begin, building_list.end(), even ? building::sort_by_x : building::sort_by_y );
      int median_index = floor( static_cast< double >( size ) / 2.0 ) - 1;
      node->median = even
                  ? building_list[median_index]->get_position().x
                  : building_list[median_index]->get_position().y;

      int left_size = median_index + 1;
      int right_size = size - left_size;

      building_list_type left_building_list( left_size );
      std::copy( begin, end - right_size, left_building_list.begin() );

      building_list_type right_building_list( right_size );
      std::copy( end - right_size, end, right_building_list.begin() );

      node->left = building_tree::build( depth + 1, left_building_list );
      node->right = building_tree::build( depth + 1, right_building_list );
    }

    return node;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building_tree::destroy( leaf* node )
  {
    if( NULL != node->left ) building_tree::destroy( node->left );
    if( NULL != node->right ) building_tree::destroy( node->right );
    utilities::safe_delete( node );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_tree::leaf* building_tree::find( leaf* node, double x, double y )
  {
    if( NULL == node->building )
      return building_tree::find( x < node->median ? node->left : node->right, y, x );
    else return node;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string building_tree::to_string( leaf* node )
  {
    std::string spacer = std::string( node->depth * 2, ' ' );
    std::stringstream stream;
    stream << spacer;
    if( NULL == node->building ) stream << node->median << std::endl;
    else 
    {
      coordinate p = node->building->get_position();
      stream << "( " << p.x << ", " << p.y << " )" << std::endl;
    }

    if( NULL != node->left ) stream << spacer << "(left)" << std::endl << building_tree::to_string( node->left );
    if( NULL != node->right ) stream << spacer << "(right)" << std::endl << building_tree::to_string( node->right );
    
    return stream.str();
  }
}
