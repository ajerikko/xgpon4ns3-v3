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

#ifndef XGPON_ID_ALLOCATOR_H
#define XGPON_ID_ALLOCATOR_H

#include <stdint.h>

#include "ns3/address.h"

namespace ns3 {



/**
 * \brief a database used to hold the configuration of the xgpon network
 *
 */
class XgponIdAllocator
{

public:
  /**
   * \brief constructor
   */
  XgponIdAllocator (uint8_t onuNetmaskLen);
  virtual ~XgponIdAllocator ();

  uint16_t GetOneNewOnuId (void);


  virtual uint16_t GetOneNewAllocId (uint16_t onuId)=0;

  virtual uint16_t GetOneNewUpstreamPortId (uint16_t onuId, const Address& addr) = 0;

  virtual uint16_t GetOneNewDownstreamPortId (uint16_t onuId, const Address& addr) = 0;

  virtual uint16_t GetOneNewBroadcastDownstreamPortId (const Address& addr) = 0;

protected:
  uint8_t m_onuNetmaskLen;

private:
  uint16_t m_nextOnuId;
};





}//namespace ns3

#endif /* XGPON_ID_ALLOCATOR_H */
