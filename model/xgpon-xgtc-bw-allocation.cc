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

#include "ns3/xgpon-xgtc-bw-allocation.h"



NS_LOG_COMPONENT_DEFINE ("XgponXgtcBwAllocation");

namespace ns3 {

//uint64_t XgponXgtcBwAllocation::CREATED_BWALLOC_NUM4DEBUG = 0;
//uint64_t XgponXgtcBwAllocation::DELETED_BWALLOC_NUM4DEBUG = 0;

std::stack<void*> XgponXgtcBwAllocation::m_pool;   //initialize one empty stack;
bool XgponXgtcBwAllocation::m_poolEnabled = true;


XgponXgtcBwAllocation::XgponXgtcBwAllocation ()
  : m_allocId (0x0000),
    m_dbru (0x00),
    m_ploamu(0x00),
    m_startTime(0x0000),
    m_grantSize (0x0000),
    m_fwi (0x00),
    m_burstProfile (0x00),
    m_hec (0x0000)
{
  meta_createTime = 0;
  meta_receiveTime = 0;
  /*
  CREATED_BWALLOC_NUM4DEBUG++;
  if((CREATED_BWALLOC_NUM4DEBUG % 1000) == 0)
  {
    std::cout << CREATED_BWALLOC_NUM4DEBUG << " bw_alloc are created;  " << DELETED_BWALLOC_NUM4DEBUG <<  " bw_alloc are deleted." <<std::endl;
  }
  */
}


XgponXgtcBwAllocation::XgponXgtcBwAllocation (uint16_t allocId, bool dbru, bool ploamu, uint16_t startTime, uint16_t grantSize, uint8_t fwi, uint8_t burstProfile)
{
  m_allocId = allocId & 0x3fff;          //14bits
  m_dbru = dbru?1:0;                  //1bits
  m_ploamu = ploamu?1:0;              //1bits
  m_startTime = startTime;               //16bits
  m_grantSize = grantSize;               //16bits
  m_fwi = fwi * 0x01;                    //1bits
  m_burstProfile = burstProfile & 0x03;  //2bits

  CalculateHec ();


  meta_createTime = 0;
  meta_receiveTime = 0;

  /*
  CREATED_BWALLOC_NUM4DEBUG++;
  if((CREATED_BWALLOC_NUM4DEBUG % 1000) == 0)
  {
    std::cout << CREATED_BWALLOC_NUM4DEBUG << " bw_alloc are created;  " << DELETED_BWALLOC_NUM4DEBUG <<  " bw_alloc are deleted." <<std::endl;
  }
  */
}

XgponXgtcBwAllocation::~XgponXgtcBwAllocation ()
{
  //DELETED_BWALLOC_NUM4DEBUG++;
}






uint64_t
XgponXgtcBwAllocation::GetSerializedAllocation (void)
{
  uint64_t sAlloc=m_allocId;
  sAlloc=(sAlloc<<2)|(m_dbru<<1)|m_ploamu;
  sAlloc=(sAlloc<<16)|m_startTime;
  sAlloc=(sAlloc<<16)|m_grantSize;
  sAlloc=(sAlloc<<3)|(m_fwi<<2)|m_burstProfile;
  sAlloc=(sAlloc<<13)|m_hec;
  return sAlloc;  
}



void
XgponXgtcBwAllocation::DeserializeAllocation (uint64_t sAlloc)
{

  m_allocId=sAlloc>>50 & 0x3fff;
  m_dbru=sAlloc>>49 & 0x01;           
  m_ploamu= sAlloc>>48 & 0x01;         

  m_startTime=sAlloc>>32 & 0xffff;      
  m_grantSize=sAlloc>>16 & 0xffff;    

  m_fwi= sAlloc>>15 & 0x01 ;           
  m_burstProfile= sAlloc>>13 & 0x03;
  m_hec= sAlloc & 0x1fff; 

  return;
}











void* 
XgponXgtcBwAllocation::operator new(size_t size) noexcept(false) //throw(const char*)
{
  void *p;
  if(m_poolEnabled == true && (!m_pool.empty()))
  {
    p = m_pool.top();
    m_pool.pop();  
    NS_LOG_INFO("allocated XgponXgtcBwAllocation through the pool!!!");
  }
  else
  {
    p = malloc(size);
    if (!p) throw "cannot allocate more memory from the system!!!";
    NS_LOG_INFO("allocated XgponXgtcBwAllocation through malloc!!!");
  }
  return p;
}

void 
XgponXgtcBwAllocation::operator delete(void *p)
{
  if(m_poolEnabled == true)
  {
    m_pool.push(p);
  }
  else free(p);
}

void 
XgponXgtcBwAllocation::DisablePoolAllocation()
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
XgponXgtcBwAllocation::Print (std::ostream &os)  const
{
  os << " Create-Time=" << meta_createTime;
  os << " Receive-Time=" << meta_receiveTime << std::endl;  
  

  os << " ALLOC-ID=" << (int)m_allocId;
  os << " DBRU-FLAG=" << (int)m_dbru;
  os << " PLOAMU-FLAG=" << (int)m_ploamu;

  os << " START-TIME=" << (int)m_startTime;
  os << " GRANT-SIZE=" << (int)m_grantSize;

  os << " FWI-FLAG=" << (int)m_fwi;
  os << " BurstProfile-INDEX=" << (int)m_burstProfile;

  os.setf(std::ios::hex, std::ios::basefield);
  os << " HEC=" << (int)m_hec;
  os.unsetf(std::ios::hex);

  os << std::endl;

  return;
}

uint32_t
XgponXgtcBwAllocation::GetSerializedSize (void) const
{
  return XGPON_XGTC_BW_ALLOCATION_LENGTH;  //8 bytes;
}

void 
XgponXgtcBwAllocation::Serialize (Buffer::Iterator start) const
{
  uint64_t sAlloc=m_allocId;
  sAlloc=(sAlloc<<2)|(m_dbru<<1)|m_ploamu;
  sAlloc=(sAlloc<<16)|m_startTime;
  sAlloc=(sAlloc<<16)|m_grantSize;
  sAlloc=(sAlloc<<3)|(m_fwi<<2)|m_burstProfile;
  sAlloc=(sAlloc<<13)|m_hec;

  start.WriteHtonU64 (sAlloc);

  return;
}

uint32_t 
XgponXgtcBwAllocation::Deserialize (Buffer::Iterator start)
{
  uint64_t rst = start.ReadNtohU64();
  DeserializeAllocation(rst);

  return GetSerializedSize ();
}


}; // namespace ns3
