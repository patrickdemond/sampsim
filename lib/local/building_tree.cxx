/*=========================================================================

  Program:  sampsim
  Module:   building_tree.cxx
  Language: C++

=========================================================================*/

#include "building_tree.h"

#include <limits>

namespace sampsim
{
  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_tree::building_tree( building_list_type building_list )
  {
    this->root_node = building_tree::build( building_list );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_tree::~building_tree()
  {
    building_tree::destroy( this->root_node );
    this->root_node = NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  sampsim::building* building_tree::find_nearest( coordinate search_coord )
  {
    double sqdist = std::numeric_limits<double>::max();
    node* nearest_node = building_tree::find_nearest_node( this->root_node, search_coord, sqdist );
    return NULL == nearest_node ? NULL : nearest_node->building;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string building_tree::to_string( node* current_node )
  {
    std::string spacer = std::string( current_node->depth * 2, ' ' );
    std::stringstream stream;
    coordinate p = current_node->get_position();
    stream << "( " << p.x << ", " << p.y << " )" << std::endl;

    if( NULL != current_node->left )
      stream << spacer << "left: " << building_tree::to_string( current_node->left );
    if( NULL != current_node->right )
      stream << spacer << "right: " << building_tree::to_string( current_node->right );
    
    return stream.str();
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_tree::node* building_tree::build(
    building_list_type building_list, node* parent_node )
  {
    // only create a new node if we don't have an empty list
    int size = building_list.size();
    node* current_node = 0 == size ? NULL : new node( parent_node );

    // one building in the list means we've reached the end
    if( 1 == size ) current_node->building = building_list.front();
    else if( 1 < size )
    {
      const building_list_type::iterator begin = building_list.begin();
      const building_list_type::iterator end = building_list.end();

      std::sort(
        begin, building_list.end(),
        0 == current_node->depth % 2 ? building::sort_by_x : building::sort_by_y );
      int median_index = floor( static_cast< double >( size ) / 2.0 );
      current_node->building = building_list[median_index];

      int left_size = median_index;
      int right_size = size - left_size - 1;

      building_list_type left_building_list( left_size );
      std::copy( begin, end - right_size - 1, left_building_list.begin() );

      building_list_type right_building_list( right_size );
      std::copy( end - right_size, end, right_building_list.begin() );

      current_node->left = building_tree::build( left_building_list, current_node );
      current_node->right = building_tree::build( right_building_list, current_node );
    }

    return current_node;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building_tree::destroy( node* current_node )
  {
    if( NULL != current_node->left ) building_tree::destroy( current_node->left );
    if( NULL != current_node->right ) building_tree::destroy( current_node->right );
    utilities::safe_delete( current_node );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_tree::node* building_tree::find_nearest_node(
    node* root_node, coordinate search_coord, double& nearest_sqdist )
  {
    if( NULL == root_node )
      throw std::runtime_error( "Tried to find nearest node without a root node" );

    node* nearest_node = NULL;

    // start by finding the nearest leaf
    node* current_node = building_tree::find_leaf( root_node, search_coord.x, search_coord.y );
    node* previous_node = NULL;

    // now travel up the tree back to the root searching for a node which is closer
    while( current_node )
    {
      coordinate current_coord = current_node->get_position();
      if( current_coord.squared_distance( search_coord ) < nearest_sqdist )
      { // found a new nearest neighbour
        nearest_node = current_node;
        nearest_sqdist = current_coord.squared_distance( search_coord );

        // if the distance > the splitting hyperplane then we need to go down the otherbranch of the tree
        node* opposite_node = current_node->left == previous_node ? current_node->right : current_node->left;
        if( opposite_node )
        {
          coordinate plane_coord = opposite_node->get_position();

          // the distance to the hyperplane only requires a single dimension
          double plane_sqdist = 0 == opposite_node->depth % 2
                              ? ( plane_coord.y - current_coord.y ) * ( plane_coord.y - current_coord.y )
                              : ( plane_coord.x - current_coord.x ) * ( plane_coord.x - current_coord.x );


          if( plane_sqdist < nearest_sqdist )
          {
            node* branch_node =
              building_tree::find_nearest_node( opposite_node, search_coord, nearest_sqdist );
            if( branch_node ) nearest_node = branch_node;
          }
        }
      }

      previous_node = current_node;
      current_node = current_node->parent;
    }

    return nearest_node;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_tree::node* building_tree::find_leaf( node* current_node, double x, double y )
  {
    node* next_node = x < current_node->get_median() ? current_node->left : current_node->right;
    return NULL == next_node ? current_node : building_tree::find_leaf( next_node, y, x );
  }
}
