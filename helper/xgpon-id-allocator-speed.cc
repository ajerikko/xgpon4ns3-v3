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
 * Author: Jerome Arokkiam
 */
#include "ns3/assert.h"

#include "ns3/ipv4-address.h"

#include "xgpon-id-allocator-speed.h"


namespace ns3 {

XgponIdAllocatorSpeed::XgponIdAllocatorSpeed(uint8_t onuNetmaskLen) : XgponIdAllocator(onuNetmaskLen), m_nextAllocIds(1024)
{
  for(int i=0; i < 1024; i++) m_nextAllocIds[i] = 1;  //to jump alloc-id for omci.
  m_nextBroadcastPortIndex = 1;
}

XgponIdAllocatorSpeed::~XgponIdAllocatorSpeed()
{
}



uint16_t 
XgponIdAllocatorSpeed::GetOneNewAllocId (uint16_t onuId)
{
  NS_ASSERT_MSG((onuId<1023), "ONU-ID is too large!!!");

  uint16_t next2Assign = m_nextAllocIds[onuId];
  NS_ASSERT_MSG((next2Assign<16), "Too many Alloc-IDs are added per ONU.");

  return (next2Assign * 1024 + onuId);
}



uint16_t 
XgponIdAllocatorSpeed::GetOneNewUpstreamPortId (uint16_t onuId, const Address& addr)
{
  NS_ASSERT_MSG((onuId<1023), "ONU-ID is too large!!!");

  Ipv4Address ipv4Addr = Ipv4Address::ConvertFrom (addr);
  uint32_t temp = ipv4Addr.Get();
  uint16_t onuIdInAddr = temp >> ((32 - m_onuNetmaskLen)) % 1024;
  NS_ASSERT_MSG((onuId == onuIdInAddr), "Strange Address!!!!");

  uint16_t next2Assign = m_nextAllocIds[onuId];
  NS_ASSERT_MSG((next2Assign<16), "Too many Alloc-IDs are added per ONU.");
  m_nextAllocIds[onuId] = m_nextAllocIds[onuId] + 1;
  //std::cout << "UPSTREAM addr: " << addr << ", temp: " << temp << ", onuIdInAddr: " << onuIdInAddr << ", next2Assign: " << next2Assign << ", m_onuNetmaskLen: " << m_onuNetmaskLen << std::endl;
  return onuId + (next2Assign)*1024; //ja:update:ns-3.35 redefining the portIds and allocIds

}



uint16_t 
XgponIdAllocatorSpeed::GetOneNewDownstreamPortId (uint16_t onuId, const Address& addr)
{
  NS_ASSERT_MSG((onuId<1024), "ONU-ID is too large!!!");   //1023 will be used for broadcast connections.
  
  if(onuId == 1023)
  {
    uint16_t tmpIndex = m_nextBroadcastPortIndex;
    uint16_t port = onuId + tmpIndex * 1024;
    m_nextBroadcastPortIndex++;
    return port;
  }
  else
  {
    Ipv4Address ipv4Addr = Ipv4Address::ConvertFrom (addr);
    uint32_t temp = ipv4Addr.Get();
    uint16_t onuIdInAddr = temp >> ((32 - m_onuNetmaskLen)) % 1024;
    NS_ASSERT_MSG((onuId == onuIdInAddr), "Strange Address!!!!");

    uint16_t next2Assign = m_nextAllocIds[onuId];
    //ja:update:ns-3.35 removing the change to the next allocId at downstream, rather moving to that in the upstream
//    NS_ASSERT_MSG((next2Assign<16), "Too many Alloc-IDs are added per ONU.");
//    m_nextAllocIds[onuId] = m_nextAllocIds[onuId] + 1;
    //std::cout << "DOWNSTREAM addr: " << addr << ", temp: " << temp << ", onuIdInAddr: " << onuIdInAddr << ", next2Assign: " << next2Assign << ", m_onuNetmaskLen: " << m_onuNetmaskLen << std::endl;
    return onuId + next2Assign*1024;
  }
}

uint16_t 
XgponIdAllocatorSpeed::GetOneNewBroadcastDownstreamPortId (const Address& addr)
{
  return GetOneNewDownstreamPortId (1023, addr);
}


}//namespace ns3

