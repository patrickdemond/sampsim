/*=========================================================================

  Program:  sampsim
  Module:   building_tree.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_building_tree_h
#define __sampsim_building_tree_h

#include "base_object.h"

#include "building.h"
#include "coordinate.h"
#include "utilities.h"

#include <stdexcept>

namespace Json { class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class building;

  /**
   * @class building_tree
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief An implementation of a kd-tree
   * @details
   * A 2D tree of buildings.  Otherwise known as a kd-tree, or k-Dimension tree, this class is
   * a data structure that is used for storing coordinates so nearest-neighbour searches or
   * range-searches are quick.
   * It is a binary-search tree that is specialised for coordinate searching, and is useful for
   * answering questions such as: which pub is closest to my current location?
   */
  class building_tree
  {
  private:
    /**
     * TODO: document
     */
    struct node
    {
      node( node* parent = NULL )
      {
        this->parent = parent;
        this->depth = NULL == parent ? 0 : parent->depth + 1;
        this->building = NULL;
        this->left = NULL;
        this->right = NULL;
      }

      double get_median()
      {
        if( NULL == this->building )
          throw std::runtime_error( "Tried to get median of node that has no building" );

        sampsim::coordinate c = this->building->get_position();
        return 0 == depth % 2 ? c.x : c.y;
      }

      node* parent;
      long long int depth;
      sampsim::building* building;
      node *left;
      node *right;
    };

  public:
    building_tree( building_list_type );
    
    ~building_tree();

    building* find_nearest( coordinate );

    std::string to_string() { return building_tree::to_string( this->root_node ); }

  private:
    static node* build( building_list_type, node* parent_node = NULL );

    static void destroy( node* );
    
    static node* find_nearest_node( node*, coordinate, double& );

    static node* find_leaf( node*, double x, double y );

    static std::string to_string( node* );

    node *root_node;
  };
}

/** @} end of doxygen group */

#endif
