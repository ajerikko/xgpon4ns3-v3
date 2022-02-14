/*
 * Copyright (c)  2012 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
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

#ifndef XGPON_ID_ALLOCATOR_FLEXIBLE_H
#define XGPON_ID_ALLOCATOR_FLEXIBLE_H

#include "xgpon-id-allocator.h"

namespace ns3 {

/**
 * \brief a database used to hold the configuration of the xgpon network
 *
 */
class XgponIdAllocatorFlexible : public XgponIdAllocator
{

public:
  /**
   * \brief constructor
   */
  XgponIdAllocatorFlexible (uint8_t onuNetmaskLen);
  virtual ~XgponIdAllocatorFlexible ();


  virtual uint16_t GetOneNewAllocId (uint16_t onuId);

  virtual uint16_t GetOneNewUpstreamPortId (uint16_t onuId, const Address& addr);

  virtual uint16_t GetOneNewDownstreamPortId (uint16_t onuId, const Address& addr);

  virtual uint16_t GetOneNewBroadcastDownstreamPortId (const Address& addr);


private:
  uint16_t m_nextAllocId;
  uint16_t m_nextUpstreamPortId;
  uint16_t m_nextDownstreamPortId;
};



}//namespace ns3

#endif /* XGPON_ID_ALLOCATOR_FLEXIBLE_H */
