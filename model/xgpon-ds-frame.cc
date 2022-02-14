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

#include "ns3/xgpon-ds-frame.h"



NS_LOG_COMPONENT_DEFINE ("XgponDsFrame");

namespace ns3 {

//uint64_t XgponDsFrame::CREATED_DS_FRAME_NUM4DEBUG = 0;
//uint64_t XgponDsFrame::DELETED_DS_FRAME_NUM4DEBUG = 0;

std::stack<void*> XgponDsFrame::m_pool;   //initialize as one empty stack;
bool XgponDsFrame::m_poolEnabled = true;

XgponDsFrame::XgponDsFrame () : PonFrame()
{
  /*
  CREATED_DS_FRAME_NUM4DEBUG++;
  if((CREATED_DS_FRAME_NUM4DEBUG % 1000) == 0)
  {
    std::cout << CREATED_DS_FRAME_NUM4DEBUG << " downstream frames are created;  " << DELETED_DS_FRAME_NUM4DEBUG <<  " downstream frames are deleted." <<std::endl;
  }
  */
}
XgponDsFrame::~XgponDsFrame ()
{
  //DELETED_DS_FRAME_NUM4DEBUG++;
}










void* 
XgponDsFrame::operator new(size_t size) noexcept(false) //throw(const char*)
{
  void *p;
  if(m_poolEnabled == true && (!m_pool.empty()))
  {
    p = m_pool.top();
    m_pool.pop();  
    NS_LOG_INFO("allocated XgponDsFrame through the pool!!!");
  }
  else
  {
    p = malloc(size);
    if (!p) throw "cannot allocate more memory from the system!!!";
    NS_LOG_INFO("allocated XgponDsFrame through malloc!!!");
  }
  return p;
}

void 
XgponDsFrame::operator delete(void *p)
{
  if(m_poolEnabled == true)
  {
    m_pool.push(p);
  }
  else free(p);
}

void 
XgponDsFrame::DisablePoolAllocation()
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
XgponDsFrame::Print (std::ostream &os)  const
{
  m_psbd.Print(os);
  m_xgtcDsFrame.Print(os);
  os<<std::endl;

  return;
}

uint32_t XgponDsFrame::GetSerializedSize (void) const
{
  uint32_t len;

  len = m_psbd.GetSerializedSize();
  len = len + m_xgtcDsFrame.GetSerializedSize();

  return len;
}

void XgponDsFrame::Serialize (Buffer::Iterator start) const
{
  //TODO: serialization;
  return;
}

uint32_t XgponDsFrame::Deserialize (Buffer::Iterator start)
{
  //TODO: deserialization;
  return GetSerializedSize ();
}

}; // namespace ns3
