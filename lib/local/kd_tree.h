/*=========================================================================

  Program:  sampsim
  Module:   kd_tree.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_kd_tree_h
#define __sampsim_kd_tree_h

#include "base_object.h"

#include "coordinate.h"
#include "utilities.h"

namespace Json { class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class building;

  /**
   * @class kd_tree
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief An implementation of a kd-tree
   * @details
   * A kD tree (otherwise known as a kd-tree, or k-Dimension tree) is a data structure that is used
   * for storing coordinates so nearest-neighbour searches or range-searches are quick.
   * It is a binary-search tree that is specialised for coordinate searching, and is useful for
   * answering questions such as which pub is closest to my current location?
   */
  class kd_tree
  {
  private:
    /**
     * TODO: document
     */
    struct leaf
    {
      leaf( long long int d )
      {
        this->depth = d;
        this->median = 0;
        this->building = NULL;
        this->left = NULL;
        this->right = NULL;
      }
      long long int depth;
      double median;
      sampsim::building* building;
      leaf *left;
      leaf *right;
    };

  public:
    kd_tree( building_list_type );
    
    ~kd_tree();

    building* find_nearest( coordinate );

    std::string to_string() { return kd_tree::to_string( this->root ); }

  private:
    static leaf* build( long long int depth, building_list_type );

    static void destroy( leaf* );
    
    static leaf* find( leaf*, double x, double y );

    static std::string to_string( leaf* );

    leaf *root;
  };
}

/** @} end of doxygen group */

#endif
