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
  building_tree::building_tree( const building_tree& tree )
  {
    this->root_node = new node;
    this->root_node->copy( tree.root_node );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_tree::~building_tree()
  {
    building_tree::destroy( this->root_node );
    this->root_node = NULL;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_list_type building_tree::get_building_list()
  {
    building_list_type building_list;
    if( !this->is_empty() )
    {
      building_list.push_back( this->root_node->building );
      building_tree::get_building_list( this->root_node, building_list );
    }
    return building_list;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building* building_tree::find_nearest( coordinate search_coord )
  {
    node* nearest_node = building_tree::find_nearest_node( this->root_node, search_coord );
    return NULL == nearest_node ? NULL : nearest_node->building;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building_tree::remove( building* building )
  {
    // gather all ancestors to the building's node
    node* remove_node = this->get_node( building );
    if( NULL == remove_node )
      throw std::runtime_error( "Tried to remove building which doesn't exist in the building_tree" );
    node* parent_node = remove_node->parent;
    building_list_type building_list;
    building_tree::get_building_list( remove_node, building_list );
    
    // special case: if the parent node is the root node and the building list is empty then
    // rebuild the entire tree (for balancing purposes)
    if( parent_node == this->root_node && 0 == building_list.size() )
    {
      building_tree::get_building_list( this->root_node, building_list );
      building_list.push_back( this->root_node->building );
      this->root_node = building_tree::build( building_list );
    }
    else
    {
      // replace the node with a tree built from the node's child buildings
      node* replacement_node = building_tree::build( building_list );

      if( replacement_node ) replacement_node->parent = parent_node;
      if( NULL == parent_node ) this->root_node = replacement_node;
      else if( parent_node->left == remove_node ) parent_node->left = replacement_node;
      else parent_node->right = replacement_node;
    }

    // now we can delete the node
    utilities::safe_delete( remove_node );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  std::string building_tree::to_string( node* current_node )
  {
    std::string spacer = std::string( current_node->depth * 2, ' ' );
    std::stringstream stream;
    coordinate p = current_node->get_position();
    stream << "node: " << current_node << " building: " << current_node->building
           << " ( " << p.x << ", " << p.y << " )" << std::endl;

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
    if( current_node )
    {
      if( NULL != current_node->left ) building_tree::destroy( current_node->left );
      if( NULL != current_node->right ) building_tree::destroy( current_node->right );
      utilities::safe_delete( current_node );
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  void building_tree::get_building_list( node* root_node, building_list_type& building_list )
  {
    if( root_node->left )
    {
      node* node = root_node->left;
      building_list.push_back( node->building );
      get_building_list( node, building_list );
    }
    if( root_node->right )
    {
      node* node = root_node->right;
      building_list.push_back( node->building );
      get_building_list( node, building_list );
    }
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_tree::node* building_tree::find_node( node* root_node, building* building )
  {
    node* found_node = NULL;
    if( building == root_node->building )
      found_node = root_node;
    if( NULL == found_node && root_node->left )
      found_node = building_tree::find_node( root_node->left, building );
    if( NULL == found_node && root_node->right )
      found_node = building_tree::find_node( root_node->right, building );

    return found_node;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_tree::node* building_tree::get_node( building* building )
  {
    return building_tree::find_node( this->root_node, building );
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_tree::node* building_tree::find_nearest_node( node* root_node, coordinate search_coord )
  {
    if( NULL == root_node ) return NULL;

    // start by finding the nearest leaf
    node* nearest_node = building_tree::find_leaf(
      root_node,
      0 == root_node->depth % 2 ? search_coord.x : search_coord.y,
      0 == root_node->depth % 2 ? search_coord.y : search_coord.x );

    // if the leaf is the root then we're done, otherwise we need to unwind the tree back to the root
    if( nearest_node != root_node )
    {
      double nearest_sqdist = nearest_node->get_position().squared_distance( search_coord );
      node* current_node = nearest_node->parent;
      node* previous_node = nearest_node;

      // now travel up the tree back to the root searching for a node which is closer
      while( true )
      {
        coordinate current_coord = current_node->get_position();
        double current_sqdist = current_coord.squared_distance( search_coord );
        if( current_sqdist < nearest_sqdist )
        { // found a new nearest neighbour
          nearest_node = current_node;
          nearest_sqdist = current_sqdist;
        }

        // if the distance > the splitting hyperplane then we need to go down the other branch of the tree
        node* opposite_node = current_node->left == previous_node ? current_node->right : current_node->left;
        if( opposite_node )
        {
          // if the search coordinate is closer to the current node's splitting plane than it is to the
          // nearest node then we need to perform the nearest node search down the other side
          double plane_sqdist = 0 == current_node->depth % 2
                              ? current_coord.x - search_coord.x
                              : current_coord.y - search_coord.y;
          plane_sqdist *= plane_sqdist;
          if( plane_sqdist < nearest_sqdist )
          {
            node* branch_node = 
              building_tree::find_nearest_node( opposite_node, search_coord );

            double branch_sqdist = branch_node->get_position().squared_distance( search_coord );
            if( branch_sqdist < nearest_sqdist )
            {
              nearest_node = branch_node;
              nearest_sqdist = branch_sqdist;
            }
          }
        }

        if( current_node == root_node ) break;

        previous_node = current_node;
        current_node = current_node->parent;
      }
    }

    return nearest_node;
  }

  //-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-+#+-
  building_tree::node* building_tree::find_leaf( node* current_node, double x, double y )
  {
    node* next_node = x < current_node->get_median() ? current_node->left : current_node->right;
    node* opposite_node = x < current_node->get_median() ? current_node->right : current_node->left;

    // if the next node is null we've reached the end of the branch, but we may still be able to go
    // down the other side from this node to reach the true leaf
    if( NULL == next_node && NULL != opposite_node ) next_node = opposite_node;

    return NULL == next_node ? current_node : building_tree::find_leaf( next_node, y, x );
  }
}
