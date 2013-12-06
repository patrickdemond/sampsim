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
     * @struct node
     * @brief An internal struct for handling tree nodes
     */
    struct node
    {
      /**
       * Constructor
       */
      node( node* parent = NULL )
      {
        this->parent = parent;
        this->depth = NULL == parent ? 0 : parent->depth + 1;
        this->building = NULL;
        this->left = NULL;
        this->right = NULL;
      }

      /**
       * Returns the node's median value
       * 
       * This will depend on the depth of the node since the axis the median is calculated
       * from alternates between the x and y axes.
       */
      double get_median()
      {
        sampsim::coordinate c = this->get_position();
        return 0 == depth % 2 ? c.x : c.y;
      }

      /**
       * Convenience method for getting a node's position from its building
       */
      coordinate get_position()
      {
        if( NULL == this->building )
          throw std::runtime_error( "Tried to get position of node that has no building" );

        return this->building->get_position();
      }

      /**
       * The node's parent node (null for the root node)
       */
      node* parent;
      
      /**
       * The node's depth
       */
      long long int depth;
      
      /**
       * The building found at the node's position
       */
      sampsim::building* building;
      
      /**
       * The node's left (negative axis direction) child node
       */
      node *left;

      /**
       * The node's right (positive axis direction) child node
       */
      node *right;
    };

  public:
    /**
     * Constructor
     */
    building_tree( building_list_type );
    
    /**
     * Destructor
     */
    ~building_tree();

    /**
     * Returns the building which is located nearest to the given coordinate
     */
    building* find_nearest( coordinate );

    /**
     * Provides a string representation of the tree
     */
    std::string to_string() { return building_tree::to_string( this->root_node ); }

  private:
    /**
     * A recursive function used to create a 2d-tree of buildings
     */
    static node* build( building_list_type, node* parent_node = NULL );

    /**
     * A recursive function that deletes the node an all of its children.
     */
    static void destroy( node* );
    
    /**
     * A recursive function that returns the nearest node in the tree
     * 
     * This is used by the find_nearest() method.
     */
    static node* find_nearest_node( node*, coordinate );

    /**
     * A recursive function that searches for the nearest leaf-node
     * 
     * This is used by the find_nearest_node() method.
     */
    static node* find_leaf( node*, double x, double y );

    /**
     * A recursive function used to create a string representation of a tree
     */
    static std::string to_string( node* );

    /**
     * The tree's root node
     */
    node *root_node;
  };
}

/** @} end of doxygen group */

#endif
