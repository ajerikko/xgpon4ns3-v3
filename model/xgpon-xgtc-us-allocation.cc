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

#include "ns3/log.h"

#include "xgpon-xgtc-us-allocation.h"



NS_LOG_COMPONENT_DEFINE ("XgponXgtcUsAllocation");

namespace ns3 {

//uint64_t XgponXgtcUsAllocation::CREATED_USALLOC_NUM4DEBUG = 0;
//uint64_t XgponXgtcUsAllocation::DELETED_USALLOC_NUM4DEBUG = 0;

std::stack<void*> XgponXgtcUsAllocation::m_pool;   //initialize as one empty stack;
bool XgponXgtcUsAllocation::m_poolEnabled = true;


XgponXgtcUsAllocation::XgponXgtcUsAllocation ()
  : m_burst(0), m_dbru(0), meta_dbruExist (false)
{
  /*
  CREATED_USALLOC_NUM4DEBUG++;
  if((CREATED_USALLOC_NUM4DEBUG % 1000) == 0)
  {
    std::cout << CREATED_USALLOC_NUM4DEBUG << " us_allocations are created;  " << DELETED_USALLOC_NUM4DEBUG <<  " us_alllocations are deleted." <<std::endl;
  }
  */
  m_burst.reserve(XGPON1_MAX_XGEM_FRAMES_PER_US_ALLOCATION);  
}
XgponXgtcUsAllocation::~XgponXgtcUsAllocation ()
{
  m_dbru = 0;

  //DELETED_USALLOC_NUM4DEBUG++;
}








void* 
XgponXgtcUsAllocation::operator new(size_t size) noexcept(false) //throw(const char*)
{
  void *p;
  if(m_poolEnabled == true && (!m_pool.empty()))
  {
    p = m_pool.top();
    m_pool.pop();  
    NS_LOG_INFO("allocated XgponXgtcUsAllocation through the pool!!!");
  }
  else
  {
    p = malloc(size);
    if (!p) throw "cannot allocate more memory from the system!!!";
    NS_LOG_INFO("allocated XgponXgtcUsAllocation through malloc!!!");
  }
  return p;
}

void 
XgponXgtcUsAllocation::operator delete(void *p)
{
  if(m_poolEnabled == true)
  {
    m_pool.push(p);
  }
  else free(p);
}

void 
XgponXgtcUsAllocation::DisablePoolAllocation()
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















void
XgponXgtcUsAllocation::Print (std::ostream &os)  const
{
  if(meta_dbruExist==true)
  {
    os << " DBRU: ";
    m_dbru->Print (os);
  }

  int frameNum = GetNXgemFrames ();
  os << " XGEM-FRAME-NUM= " << frameNum;

  std::vector<Ptr<XgponXgemFrame> >::const_iterator iterator4Print, end;
  iterator4Print = m_burst.begin();
  end = m_burst.end();
  int i = 1;
  while (iterator4Print != end)
  {
    os << " XGEM-FRAME "<<i<<": ";
    (*iterator4Print)->Print(os);
    iterator4Print++;
    i++;
  }

  os << std::endl;

  return;
}

uint32_t XgponXgtcUsAllocation::GetSerializedSize (void) const
{
  uint32_t len=0;
  
  std::vector<Ptr<XgponXgemFrame> >::const_iterator it, end;
  it = m_burst.begin();
  end = m_burst.end();
  while(it!=end) { len += (*it)->GetSerializedSize(); }

  if(meta_dbruExist == true) len = len + m_dbru->GetSerializedSize();

  return len;
}

void XgponXgtcUsAllocation::Serialize (Buffer::Iterator start) const
{
  //TODO: serialization;
  //if(meta_dbruExist == true) m_dbru->Serialize(start);
  //packet burst serialization;
  return;
}

uint32_t XgponXgtcUsAllocation::Deserialize (Buffer::Iterator start)
{
  //TODO: deserialization;
  //if(meta_dbruExist == true) m_dbru->Deserialize(start);
  //packet burst deserialization;
  return GetSerializedSize ();
}

}; // namespace ns3
