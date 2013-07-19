/*=========================================================================

  Program:  sampsim
  Module:   individual.h
  Language: C++

=========================================================================*/

/**
 * @class individual
 * @namespace sampsim
 * 
 * @author Patrick Emond <emondpd@mcmaster.ca>
 * 
 * @brief An individual which belongs to a household
 */

#ifndef __individual_h
#define __individual_h

#include "base_object.h"

namespace Json { class Value; }

/**
 * @addtogroup sampsim
 * @{
 */

namespace sampsim
{
  class household;

  class individual : public base_object
  {
  public:
    individual( household *parent );
    ~individual();
    household* get_parent() { return this->parent; }

    virtual void to_json( Json::Value& );

    bool is_male() const { return this->male; }
    void set_male( const bool male ) { this->male = male; }
    bool is_adult() const { return this->adult; }
    void set_adult( const bool adult ) { this->adult = adult; }

  protected:

  private:
    household *parent;
    bool male;
    bool adult;
  };
}

/** @} end of doxygen group */

#endif
