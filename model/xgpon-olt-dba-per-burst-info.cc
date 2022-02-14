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

#include "xgpon-olt-dba-per-burst-info.h"
#include "xgpon-xgtc-ploam.h"


NS_LOG_COMPONENT_DEFINE ("XgponOltDbaPerBurstInfo");

namespace ns3{

std::stack<void*> XgponOltDbaPerBurstInfo::m_pool;   //initialize one empty list;
bool XgponOltDbaPerBurstInfo::m_poolEnabled = true;


XgponOltDbaPerBurstInfo::XgponOltDbaPerBurstInfo () 
  : m_onuId(0), m_gapPhyOverhead(0), 
  m_fec(false), m_ploamExist(false),
  m_dataBlockSize(0), m_fecBlockSize(0),
  m_headerTrailerDataSize(0), m_finalBurstSize(0)
{
}
XgponOltDbaPerBurstInfo::~XgponOltDbaPerBurstInfo ()
{
}







void 
XgponOltDbaPerBurstInfo::Initialize(uint16_t onuId, bool ploam, const Ptr<XgponBurstProfile>& profile, uint16_t guardTime, uint16_t dataBlockSize, uint16_t fecBlockSize)
{
  NS_LOG_FUNCTION(this);

  m_onuId = onuId;

  m_gapPhyOverhead = guardTime + (profile->GetPreambleLen () + profile->GetDelimiterLen ()) / 4;  //unit: word;

  m_fec = profile->GetFec();
  m_ploamExist = ploam;
  m_dataBlockSize = dataBlockSize / 4;
  m_fecBlockSize = fecBlockSize / 4;  

  //XgtcUsBurstHeader: one word; trailer: one word
  m_headerTrailerDataSize = XGTC_USBURST_HEADERTRAILER_INWORD; 
  if(ploam) m_headerTrailerDataSize = m_headerTrailerDataSize + XgponXgtcPloam::XGPON_XGTC_PLOAM_LENGTH / 4;

  UpdateFinalBurstSize();

  m_bwAllocs.clear();
  m_tcontOlts.clear();
}




void 
XgponOltDbaPerBurstInfo::AddOneNewBwAlloc(const Ptr<XgponXgtcBwAllocation>& bwAlloc, const Ptr<XgponTcontOlt>& tcontOlt)
{
  NS_LOG_FUNCTION(this);

  m_bwAllocs.push_back(bwAlloc);
  m_tcontOlts.push_back(tcontOlt);

  uint32_t grantSize = bwAlloc->GetGrantSize ();
  m_headerTrailerDataSize += grantSize;
  
  UpdateFinalBurstSize();
}



void 
XgponOltDbaPerBurstInfo::UpdateFinalBurstSize()
{
  NS_LOG_FUNCTION(this);

  if(m_fec)
  {
    uint32_t burstSize, tmp;

    tmp = m_headerTrailerDataSize % m_dataBlockSize;
  
    /**OLT should try to make the xgtc burst size according to FEC block size. 
     * In case the length of allocated burst is not a multiple of m_dataBlockSize, 
     * ONU can carry out FEC in which the last FEC block is a shortened one (data + parity bits).  */
    if(tmp==0) burstSize = (m_headerTrailerDataSize / m_dataBlockSize) * m_fecBlockSize;
    else burstSize = (m_headerTrailerDataSize / m_dataBlockSize) * m_fecBlockSize + tmp + (m_fecBlockSize - m_dataBlockSize);
    
    m_finalBurstSize = burstSize + m_gapPhyOverhead;
  }
  else
  {
    m_finalBurstSize = m_headerTrailerDataSize + m_gapPhyOverhead;
  }
}






void 
XgponOltDbaPerBurstInfo::PutAllBwAllocIntoBwmap(const Ptr<XgponXgtcBwmap>& map, uint16_t startTime, uint64_t now)
{
  NS_LOG_FUNCTION(this);

  int num = m_bwAllocs.size();
  for(int i=0; i<num; i++)
  {
    const Ptr<XgponXgtcBwAllocation>& bwAlloc = m_bwAllocs[i];
    if(i==0) bwAlloc->SetStartTime(startTime);
    map->AddOneBwAllocation (bwAlloc);
    m_tcontOlts[i]->AddNewBwAllocation2ServiceHistory(bwAlloc, now);
  }
}


void
XgponOltDbaPerBurstInfo::AddToExistingBwAlloc(Ptr<XgponXgtcBwAllocation> bwAlloc, uint32_t extraGrantSize)
{
  uint32_t origGrantSize = bwAlloc->GetGrantSize ();
  bwAlloc->SetGrantSize(origGrantSize + extraGrantSize);
  m_headerTrailerDataSize += extraGrantSize;

  UpdateFinalBurstSize();
}

Ptr<XgponXgtcBwAllocation>
XgponOltDbaPerBurstInfo::FindBwAlloc(Ptr<XgponTcontOlt> tcontOlt)
{
 std::deque<Ptr<XgponXgtcBwAllocation> >::iterator  it;

 for (it=m_bwAllocs.begin(); it!=m_bwAllocs.end(); it++)
 {
    if((*it)->GetAllocId() == tcontOlt->GetAllocId())
      return *it;
  }
 return 0;

}













void* 
XgponOltDbaPerBurstInfo::operator new(size_t size) noexcept(false) //throw(const char*)
{
  void *p;
  if(m_poolEnabled == true && (!m_pool.empty()))
  {
    p = m_pool.top();
    m_pool.pop();  
    NS_LOG_INFO("allocated XgponOltDbaPerBurstInfo through the pool!!!");
  }
  else
  {
    p = malloc(size);
    if (!p) throw "cannot allocate more memory from the system!!!";
    NS_LOG_INFO("allocated XgponOltDbaPerBurstInfo through malloc!!!");
  }
  return p;
}

void 
XgponOltDbaPerBurstInfo::operator delete(void *p)
{
  if(m_poolEnabled == true)
  {
    m_pool.push(p);
  }
  else free(p);
}

void 
XgponOltDbaPerBurstInfo::DisablePoolAllocation()
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





}//namespace ns3
