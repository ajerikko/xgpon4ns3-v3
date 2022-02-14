/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
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
#include "ns3/assert.h"

#include "xgpon-id-allocator.h"


namespace ns3 {

XgponIdAllocator::XgponIdAllocator(uint8_t onuNetmaskLen)
{
  m_nextOnuId = 0;
  m_onuNetmaskLen = onuNetmaskLen;
}

XgponIdAllocator::~XgponIdAllocator()
{
}




uint16_t 
XgponIdAllocator::GetOneNewOnuId (void)
{
  NS_ASSERT_MSG((m_nextOnuId<1023), "Too many ONUs are added to the network (>1024).");
  //1023 is used when broadcasting ploam in downstream

  return m_nextOnuId++;
}

}//namespace ns3

