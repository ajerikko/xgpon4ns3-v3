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

#include "xgpon-id-allocator-flexible.h"


namespace ns3 {

XgponIdAllocatorFlexible::XgponIdAllocatorFlexible(uint8_t onuNetmaskLen) : XgponIdAllocator(onuNetmaskLen)
{
  m_nextAllocId = 1024;
  m_nextUpstreamPortId = 1024;
  m_nextDownstreamPortId = 1024;
}

XgponIdAllocatorFlexible::~XgponIdAllocatorFlexible()
{
}



uint16_t 
XgponIdAllocatorFlexible::GetOneNewAllocId (uint16_t onuId)
{
  NS_ASSERT_MSG((m_nextAllocId<16384), "Too many Alloc-IDs are added to the network.");
  return m_nextAllocId++;
}



uint16_t 
XgponIdAllocatorFlexible::GetOneNewUpstreamPortId (uint16_t onuId, const Address& addr)
{
  NS_ASSERT_MSG((m_nextUpstreamPortId<65536), "Too many upstream XGEM-PORTs are added to the network.");
  return m_nextUpstreamPortId++;
}



uint16_t 
XgponIdAllocatorFlexible::GetOneNewDownstreamPortId (uint16_t onuId, const Address& addr)
{
  NS_ASSERT_MSG((m_nextDownstreamPortId<65536), "Too many downstream XGEM-PORTs are added to the network.");
  return m_nextDownstreamPortId++;
}

uint16_t 
XgponIdAllocatorFlexible::GetOneNewBroadcastDownstreamPortId (const Address& addr)
{
  NS_ASSERT_MSG((m_nextDownstreamPortId<65536), "Too many downstream XGEM-PORTs are added to the network.");
  return m_nextDownstreamPortId++;
}


}//namespace ns3

