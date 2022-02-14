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
 * Author: Pedro Alvarez <pinheirp@tcd.ie>
 */
//#include <iostream>

#include "ns3/log.h"
#include "ns3/uinteger.h"

#include "xgpon-phy.h"



 
NS_LOG_COMPONENT_DEFINE ("XgponPhy");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (XgponPhy);

TypeId 
XgponPhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponPhy")
    .SetParent<Object> ()
    .AddAttribute ("DsLinkRate", 
                   "The link rate in downstream direction (Unit: Byte per second).",
                   UintegerValue (XGPON1_DS_LINE_RATE),
                   MakeUintegerAccessor (&XgponPhy::m_dsLinkRate),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("DsFrameSlotSize", 
                   "The duration of each downstream frame (Unit: nano-second).",
                   UintegerValue (XGPON1_DS_FRAME_SLOT_SIZE),
                   MakeUintegerAccessor (&XgponPhy::m_dsFrameSlotSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("DsPsbSize", 
                   "The size of downstream frame physical synchronization block (Unit: byte).",
                   UintegerValue (XGPON1_DS_PSB_SIZE),
                   MakeUintegerAccessor (&XgponPhy::m_dsPsbSize),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("UsLinkRate", 
                   "The link rate in upstream direction (Unit: Byte per second).",
                   UintegerValue (XGPON1_US_LINE_RATE),
                   MakeUintegerAccessor (&XgponPhy::m_usLinkRate),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("UsGuardTime", 
                   "The Guard Time between consecutive upstream bursts (Unit: word / 4bytes).",
                   UintegerValue (XGPON1_MINIMUM_GUARD_TIME),
                   MakeUintegerAccessor (&XgponPhy::m_usMinimalGuardTime),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("DsFecBlockSize", 
                   "The size of a FEC block for downstream direction (Unit: byte).",
                   UintegerValue (XGPON1_FEC_BLOCK_SIZE),
                   MakeUintegerAccessor (&XgponPhy::m_dsFecBlockSize),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("DsFecBlockDataSize", 
                   "The amount of data in a FEC block for downstream direction (Unit: byte).",
                   UintegerValue (XGPON1_DS_FEC_DATA_SIZE),
                   MakeUintegerAccessor (&XgponPhy::m_dsFecBlockDataSize),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("UsFecBlockSize", 
                   "The size of a FEC block for upstream direction (Unit: byte).",
                   UintegerValue (XGPON1_FEC_BLOCK_SIZE),
                   MakeUintegerAccessor (&XgponPhy::m_usFecBlockSize),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("UsFecBlockDataSize", 
                   "The amount of data in a FEC block for upstream direction (Unit: byte).",
                   UintegerValue (XGPON1_US_FEC_DATA_SIZE),
                   MakeUintegerAccessor (&XgponPhy::m_usFecBlockDataSize),
                   MakeUintegerChecker<uint16_t> ())

  ;
  return tid;
}

TypeId
XgponPhy::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}



XgponPhy::XgponPhy (): m_dsXgtcFrameSize(0), m_usPhyFrameSize(0)
{
}
XgponPhy::~XgponPhy ()
{
}






uint32_t 
XgponPhy::CalculateXgtcDsFrameSize ( )
{
  uint64_t tmp;
  uint32_t rst;

  tmp = ((uint64_t)m_dsLinkRate *  (uint64_t)m_dsFrameSlotSize)/ 1000000000;  //overall size in bytes
  tmp = tmp - m_dsPsbSize;
  tmp = (tmp * m_dsFecBlockDataSize) / m_dsFecBlockSize;
  
  rst = (uint32_t)tmp;

  return rst;
}







uint32_t 
XgponPhy::CalculateUsPhyFrameSize ( )
{
  uint64_t tmp;
  uint32_t rst;

  tmp = (((uint64_t)m_usLinkRate * (uint64_t)m_dsFrameSlotSize) / 1000000000)/4;  //overall size in word (4 bytes)
  
  rst = (uint32_t)tmp;

  return rst;
}



}//namespace ns3
