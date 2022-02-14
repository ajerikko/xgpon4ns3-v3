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

#include "xgpon-us-burst.h"



NS_LOG_COMPONENT_DEFINE ("XgponUsBurst");

namespace ns3 {

//uint64_t XgponUsBurst::CREATED_US_BURST_NUM4DEBUG = 0;
//uint64_t XgponUsBurst::DELETED_US_BURST_NUM4DEBUG = 0;

std::stack<void*> XgponUsBurst::m_pool;   //initialize as one empty list;
bool XgponUsBurst::m_poolEnabled = true;

XgponUsBurst::XgponUsBurst () : PonFrame ()
{
  /*
  CREATED_US_BURST_NUM4DEBUG++;
  if((CREATED_US_BURST_NUM4DEBUG % 1000) == 0)
  {
    std::cout << CREATED_US_BURST_NUM4DEBUG << " upstream bursts are created;  " << DELETED_US_BURST_NUM4DEBUG <<  " upstream bursts are deleted." <<std::endl;
  }
  */
}
XgponUsBurst::~XgponUsBurst ()
{
  //DELETED_US_BURST_NUM4DEBUG++;
}








void* 
XgponUsBurst::operator new(size_t size) noexcept(false) //throw(const char*)
{
  void *p;
  if(m_poolEnabled == true && (!m_pool.empty()))
  {
    p = m_pool.top();
    m_pool.pop();  
    NS_LOG_INFO("allocated XgponUsBurst through the pool!!!");
  }
  else
  {
    p = malloc(size);
    if (!p) throw "cannot allocate more memory from the system!!!";
    NS_LOG_INFO("allocated XgponUsBurst through malloc!!!");
  }
  return p;
}

void 
XgponUsBurst::operator delete(void *p)
{
  if(m_poolEnabled == true)
  {
    m_pool.push(p);
  }
  else free(p);
}

void 
XgponUsBurst::DisablePoolAllocation()
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
XgponUsBurst::Print (std::ostream &os)  const
{
  m_psbu.Print(os);
  m_xgtcUsBurst.Print(os);

  os << std::endl;

  return;
}

uint32_t 
XgponUsBurst::GetSerializedSize (void) const
{
  uint32_t len;

  len = m_psbu.GetSerializedSize();  
  len = len +  m_xgtcUsBurst.GetSerializedSize(); 

  return len;
}

void 
XgponUsBurst::Serialize (Buffer::Iterator start) const
{
  m_psbu.Serialize(start);
  m_xgtcUsBurst.Serialize(start);

  return;
}

uint32_t 
XgponUsBurst::Deserialize (Buffer::Iterator start)
{
  m_psbu.Deserialize(start);
  m_xgtcUsBurst.Deserialize(start);

  return GetSerializedSize ();
}

}; // namespace ns3
