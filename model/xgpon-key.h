/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 University College Cork (UCC), Ireland
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Xiuchao Wu <xw2@cs.ucc.ie>
 */

#ifndef XGPON_KEY_H
#define XGPON_KEY_H

#include "ns3/object.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The key used by XG-PON to encrypt downstream (or upstream) traffic.
 *        It's a stub class for designing the interactions/interfaces.
 *        Data encryption won't be carried for saving CPU.
 */

class XgponKey : public Object
{
public:

  /**
   * \brief Constructor
   */
  XgponKey ();
  virtual ~XgponKey ();

  void DeepCopy(const Ptr<XgponKey>& key);


  ///////////////////////////////////////////////required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
};


}; // namespace ns3

#endif // XGPON_KEY_H
