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

#include "xgpon-xgtc-bwmap.h"




NS_LOG_COMPONENT_DEFINE ("XgponXgtcBwmap");

namespace ns3 {

//uint64_t XgponXgtcBwmap::CREATED_BWMAP_NUM4DEBUG = 0;
//uint64_t XgponXgtcBwmap::DELETED_BWMAP_NUM4DEBUG = 0;

std::stack<void*> XgponXgtcBwmap::m_pool;   //initialize as one empty stack;
bool XgponXgtcBwmap::m_poolEnabled = true;


XgponXgtcBwmap::XgponXgtcBwmap ()
  : meta_allocationNumber (0), meta_creationTime (0)
{
  //m_bwAllocations.reserve(XgponOltDbaEngine::MAX_ALLOCID_PER_BWMAP+1);  
  /*
  CREATED_BWMAP_NUM4DEBUG++;
  if((CREATED_BWMAP_NUM4DEBUG % 1000) == 0)
  {
    std::cout << CREATED_BWMAP_NUM4DEBUG << " bw_map are created;  " << DELETED_BWMAP_NUM4DEBUG <<  " bw_map are deleted." <<std::endl;
  }
  */
}
XgponXgtcBwmap::~XgponXgtcBwmap ()
{
  //DELETED_BWMAP_NUM4DEBUG++;
}





void 
XgponXgtcBwmap::AddOneBwAllocation (const Ptr<XgponXgtcBwAllocation>& bwAlloc)
{
  m_bwAllocations.push_back(bwAlloc);
}

void 
XgponXgtcBwmap::AddOneSerializedBwAllocation (uint64_t sBwAlloc)
{
  Ptr<XgponXgtcBwAllocation> bwAlloc = Create<XgponXgtcBwAllocation>(); 
  bwAlloc->DeserializeAllocation(sBwAlloc);

  m_bwAllocations.push_back(bwAlloc);
  return;
}

uint16_t 
XgponXgtcBwmap::GetNumberOfBwAllocation ( ) const
{
  return m_bwAllocations.size();
}

const Ptr<XgponXgtcBwAllocation>& 
XgponXgtcBwmap::GetBwAllocationByIndex (uint16_t index) const
{
  NS_ASSERT_MSG( (index < m_bwAllocations.size()), "The index is too large for BWmap!!!");

  return m_bwAllocations[index];
}








void* 
XgponXgtcBwmap::operator new(size_t size) noexcept(false) //throw(const char*)
{
  void *p;
  if(m_poolEnabled == true && (!m_pool.empty()))
  {
    p = m_pool.top();
    m_pool.pop();  
    NS_LOG_INFO("allocated XgponXgtcBwmap through the pool!!!");
  }
  else
  {
    p = malloc(size);
    if (!p) throw "cannot allocate more memory from the system!!!";
    NS_LOG_INFO("allocated XgponXgtcBwmap through malloc!!!");
  }
  return p;
}

void 
XgponXgtcBwmap::operator delete(void *p)
{
  if(m_poolEnabled == true)
  {
    m_pool.push(p);
  }
  else free(p);
}

void 
XgponXgtcBwmap::DisablePoolAllocation()
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
XgponXgtcBwmap::Print (std::ostream &os)  const
{
  uint32_t i;
  std::deque< Ptr<XgponXgtcBwAllocation> >::const_iterator iterator4Print, end;

  os << std::endl <<std::endl <<"Creation Time = " << meta_creationTime <<std::endl;
  os << " BW-ALLOC-NUM=" << m_bwAllocations.size();
  os << std::endl;

  iterator4Print = m_bwAllocations.begin();
  end = m_bwAllocations.end();
  i = 1;
  while (iterator4Print != end)
  {
    os << " BW-ALLOCATION "<<i<<": ";
    (*iterator4Print)->Print(os);
    //os << std::endl;
    iterator4Print++;
    i++;
  }

  os << std::endl;

  return;
}

uint32_t XgponXgtcBwmap::GetSerializedSize (void) const
{
  return XGPON_XGTC_BW_ALLOCATION_LENGTH * m_bwAllocations.size(); 
}

void XgponXgtcBwmap::Serialize (Buffer::Iterator start) const
{
  int i, num;
  Ptr<XgponXgtcBwAllocation> bwAlloc;

  num = m_bwAllocations.size(); 

  for(i=0; i<num; i++)
  {
    bwAlloc = GetBwAllocationByIndex(i);
    bwAlloc->Serialize(start);
  }
  return;
}

uint32_t XgponXgtcBwmap::Deserialize (Buffer::Iterator start)
{
  int i;
  Ptr<XgponXgtcBwAllocation> bwAlloc;

  for(i=0; i<meta_allocationNumber; i++)
  {
    bwAlloc = Create<XgponXgtcBwAllocation>();
    bwAlloc->Deserialize(start);
    m_bwAllocations.push_back(bwAlloc);
  }
  
  return GetSerializedSize ();
}


} // namespace ns3
