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

#include "xgpon-xgtc-ploam.h"

NS_LOG_COMPONENT_DEFINE ("XgponXgtcPloam");

namespace ns3 {


std::stack<void*> XgponXgtcPloam::m_pool;   //initialize as one empty stack;
bool XgponXgtcPloam::m_poolEnabled = true;


XgponXgtcPloam::XgponXgtcPloam ()
  : m_onuId (0x0000),
    m_type (0x00),
    m_seqNo (0x00)
{
}
XgponXgtcPloam::~XgponXgtcPloam ()
{
}



XgponXgtcPloam& 
XgponXgtcPloam::operator=(const XgponXgtcPloam &rhs)
{
  this->m_onuId = rhs.m_onuId;
  this->m_type = rhs.m_type;
  this->m_seqNo = rhs.m_seqNo;

  for(int i=0; i<36; i++) { this->m_content[i] = rhs.m_content[i]; }
  for(int i=0; i<8; i++) { this->m_mic[i] = rhs.m_mic[i]; }
  
  return *this;
}










void* 
XgponXgtcPloam::operator new(size_t size) noexcept(false) //throw(const char*)
{
  void *p;
  if(m_poolEnabled == true && (!m_pool.empty()))
  {
    p = m_pool.top();
    m_pool.pop();  
    NS_LOG_INFO("allocated XgponXgtcPloam through the pool!!!");
  }
  else
  {
    p = malloc(size);
    if (!p) throw "cannot allocate more memory from the system!!!";
    NS_LOG_INFO("allocated XgponXgtcPloam through malloc!!!");
  }
  return p;
}

void 
XgponXgtcPloam::operator delete(void *p)
{
  if(m_poolEnabled == true)
  {
    m_pool.push(p);
  }
  else free(p);
}

void 
XgponXgtcPloam::DisablePoolAllocation()
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
XgponXgtcPloam::Print (std::ostream &os)  const
{
  os << " ONU-ID=" << (int)m_onuId;
  os << " TYPE=" << (int)m_type;
  os << " SEQ-NO=" << (int)m_seqNo;

  os.setf(std::ios::hex, std::ios::basefield);
  os << " CONTENT=";
  for(int i=0; i<36; i++) { os << (int)m_content[i] << " "; }
  os << " MIC=";
  for(int i=0; i<8; i++) { os << (int)m_mic[i] << " "; }
  os.unsetf(std::ios::hex);

  os << std::endl;

  return;
}

uint32_t XgponXgtcPloam::GetSerializedSize (void) const
{
  return XGPON_XGTC_PLOAM_LENGTH;  //48 bytes;
}

void XgponXgtcPloam::Serialize (Buffer::Iterator start) const
{
  start.WriteHtonU16(m_onuId);
  start.WriteU8(m_type);
  start.WriteU8(m_seqNo);
    
  for(int j=0;j<36;j++) start.WriteU8(m_content[j]);
  for(int j=0;j<8;j++) start.WriteU8(m_mic[j]);

  return;
}

uint32_t XgponXgtcPloam::Deserialize (Buffer::Iterator start)
{

  m_onuId=start.ReadNtohU16();
  m_type=start.ReadU8();
  m_seqNo=start.ReadU8();
    
  for(int j=0;j<36;j++) m_content[j]=start.ReadU8();
  for(int j=0;j<8;j++) m_mic[j]=start.ReadU8();

  //VerifyMic( );
  return GetSerializedSize ();
}

}; // namespace ns3
