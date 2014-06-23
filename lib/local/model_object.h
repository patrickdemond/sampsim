/*=========================================================================

  Program:  sampsim
  Module:   model_object.h
  Language: C++

=========================================================================*/

#ifndef __sampsim_model_object_h
#define __sampsim_model_object_h

#include "base_object.h"

namespace Json { class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  /**
   * @class model_object
   * @author Patrick Emond <emondpd@mcmaster.ca>
   * @brief Abstract base class for all classes which describe the population
   * @details
   * Populations are organized into a tree such that all nodes are responsible for creating,
   * generating and deleting their children.  The structure is as follows:
   * - population
   *   + list of towns in population
   *     - list of n by m tiles
   *       + list of buildings in tile
   *         - list of households in building
   *           + list of individuals belonging to household
   * 
   * This class contains all functionality common to all model classes.
   */
  class model_object : public base_object {};
}

/** @} end of doxygen group */

#endif
