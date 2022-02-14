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
#include <math.h>

#include "ns3/log.h"

#include "xgpon-xgem-frame.h"



NS_LOG_COMPONENT_DEFINE ("XgponXgemFrame");

namespace ns3 {

//uint64_t XgponXgemFrame::CREATED_XGEM_FRAME_NUM4DEBUG = 0;
//uint64_t XgponXgemFrame::DELETED_XGEM_FRAME_NUM4DEBUG = 0;

std::stack<void*> XgponXgemFrame::m_pool;   //initialize as one empty list;
bool XgponXgemFrame::m_poolEnabled = true;

XgponXgemFrame::XgponXgemFrame () 
  : m_type(XGPON_XGEM_FRAME_SHORT_IDLE), m_data(0)
{
  /*
  CREATED_XGEM_FRAME_NUM4DEBUG++;
  if((CREATED_XGEM_FRAME_NUM4DEBUG % 1000) == 0)
  {
    std::cout << CREATED_XGEM_FRAME_NUM4DEBUG << " xgem frames are created;  " << DELETED_XGEM_FRAME_NUM4DEBUG <<  " xgem frames are deleted." <<std::endl;
  }
  */
}
XgponXgemFrame::~XgponXgemFrame ()
{
  //DELETED_XGEM_FRAME_NUM4DEBUG++;
}








void* 
XgponXgemFrame::operator new(size_t size) noexcept(false) //throw(const char*)
{
  void *p;
  if(m_poolEnabled == true && (!m_pool.empty()))
  {
    p = m_pool.top();
    m_pool.pop();  
    NS_LOG_INFO("allocated XgponXgemFrame through the pool!!!");
  }
  else
  {
    p = malloc(size);
    if (!p) throw "cannot allocate more memory from the system!!!";
    NS_LOG_INFO("allocated XgponXgemFrame through malloc!!!");
  }
  return p;
}

void 
XgponXgemFrame::operator delete(void *p)
{
  if(m_poolEnabled == true)
  {
    m_pool.push(p);
  }
  else free(p);
}

void 
XgponXgemFrame::DisablePoolAllocation()
{
  void *p;

  m_poolEnabled = false;

  while (!m_pool.empty())
  {
    p = m_pool.top();
    free(p);
    m_pool.pop();
  }
}





uint32_t 
XgponXgemFrame::GetPaddedPayloadSize(uint32_t payloadSize)
{
  uint32_t pLen;
  if(payloadSize>=8) pLen = 4*ceil((float)payloadSize/4);
  else if(payloadSize>0) pLen = 8;
  else pLen = 0;
  
  return pLen;
}







void
XgponXgemFrame::Print (std::ostream &os)  const
{

  if(m_type == XGPON_XGEM_FRAME_SHORT_IDLE) os << " SHORT-IDLE-XGEM-FRAME ";
  else if (m_type == XGPON_XGEM_FRAME_LONG_IDLE)
  {
    os << " LONG-IDLE-XGEM-FRAME: " << GetSerializedSize() << " bytes ";
  } else
  {  
    os << " LONG-IDLE-XGEM-FRAME: " << GetSerializedSize() << " bytes " << std::endl;
    m_header.Print(os);
    m_data->Print(os);
  }

  os << std::endl;

  return;
}

uint32_t 
XgponXgemFrame::GetSerializedSize (void) const
{
  if(m_type == XGPON_XGEM_FRAME_SHORT_IDLE) return 4;
  else 
  {
    uint32_t len = m_header.GetPli ();
    len = GetPaddedPayloadSize(len);
    len += XgponXgemHeader::XGPON_XGEM_HEADER_LENGTH;
    return len;
  }
}

void 
XgponXgemFrame::Serialize (Buffer::Iterator start) const
{
  return;
}

uint32_t 
XgponXgemFrame::Deserialize (Buffer::Iterator start)
{
  return GetSerializedSize ();
}

}; // namespace ns3
