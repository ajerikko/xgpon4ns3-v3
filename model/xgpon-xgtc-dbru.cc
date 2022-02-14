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

#include "xgpon-xgtc-dbru.h"




NS_LOG_COMPONENT_DEFINE ("XgponXgtcDbru");

namespace ns3 {


std::stack<void*> XgponXgtcDbru::m_pool;   //initialize as one empty stack;
bool XgponXgtcDbru::m_poolEnabled = true;


XgponXgtcDbru::XgponXgtcDbru ()
  : m_bufOcc (0x00000000),
    m_crc (0x00)
{
  meta_createTime = 0;
  meta_receiveTime = 0;
}
XgponXgtcDbru::XgponXgtcDbru (uint32_t bufOcc)
{
  m_bufOcc = bufOcc & 0x00ffffff;  //24bits
  CalculateCrc ();

  meta_createTime = 0;
  meta_receiveTime = 0;
}
XgponXgtcDbru::~XgponXgtcDbru ()
{
}





XgponXgtcDbru& 
XgponXgtcDbru::operator=(const XgponXgtcDbru &rhs)
{
  this->m_bufOcc = rhs.m_bufOcc;
  this->m_crc = rhs.m_crc;

  this->meta_createTime = rhs.meta_createTime;
  this->meta_receiveTime = rhs.meta_receiveTime;

  return *this;
}









void* 
XgponXgtcDbru::operator new(size_t size) noexcept(false) //throw(const char*)
{
  void *p;
  if(m_poolEnabled == true && (!m_pool.empty()))
  {
    p = m_pool.top();
    m_pool.pop();  
    NS_LOG_INFO("allocated XgponXgtcDbru through the pool!!!");
  }
  else
  {
    p = malloc(size);
    if (!p) throw "cannot allocate more memory from the system!!!";
    NS_LOG_INFO("allocated XgponXgtcDbru through malloc!!!");
  }
  return p;
}

void 
XgponXgtcDbru::operator delete(void *p)
{
  if(m_poolEnabled == true)
  {
    m_pool.push(p);
  }
  else free(p);
}

void 
XgponXgtcDbru::DisablePoolAllocation()
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
XgponXgtcDbru::Print (std::ostream &os)  const
{
  os << " Create-Time=" << meta_createTime;
  os << " Receive-Time=" << meta_receiveTime << std::endl;  

  os << " BufOccupancy=" << (uint32_t)m_bufOcc;

  os.setf(std::ios::hex, std::ios::basefield);
  os << " CRC=" << (uint16_t)m_crc;
  os.unsetf(std::ios::hex);

  os << std::endl;

  return;
}

uint32_t XgponXgtcDbru::GetSerializedSize (void) const
{
  return XGPON_XGTC_DBRU_LENGTH;  //8 bytes;
}

void XgponXgtcDbru::Serialize (Buffer::Iterator start) const
{
  //leave alone in first phase
  return;
}

uint32_t XgponXgtcDbru::Deserialize (Buffer::Iterator start)
{
  //leave alone in first phase
  return GetSerializedSize ();
}

}; // namespace ns3
