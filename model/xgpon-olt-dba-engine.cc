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
 * Authors: 
 * Xiuchao Wu <xw2@cs.ucc.ie>
 * Jerome A Arokkiam <jerom2005raj@gmail.com>
 */

#include "ns3/log.h"
#include "ns3/simulator.h"

#include "xgpon-olt-dba-engine.h"
#include "xgpon-olt-net-device.h"
#include "xgpon-channel.h"



NS_LOG_COMPONENT_DEFINE ("XgponOltDbaEngine");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (XgponOltDbaEngine);

TypeId 
XgponOltDbaEngine::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOltDbaEngine")
    .SetParent<XgponOltEngine> ()
  ;
  return tid;
}
TypeId
XgponOltDbaEngine::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}




XgponOltDbaEngine::XgponOltDbaEngine (): m_bursts(), 
  m_aggregateAllocatedSize(0),
  m_servedBwmaps(0), m_nullBwmap(0),
  m_extraInLastBwmap(0),
  m_dsFrameSlotSizeInNano (0), m_logicRtt (0), m_usRate(0)
{
  m_servedBwmaps.clear();
}
XgponOltDbaEngine::~XgponOltDbaEngine ()
{
}







void 
XgponOltDbaEngine::ReceiveStatusReport (const Ptr<XgponXgtcDbru>& report, uint16_t onuId, uint16_t allocId)
{
  NS_LOG_FUNCTION(this);

  const Ptr<XgponTcontOlt>& tcont = (m_device->GetConnManager( ))->GetTcontById (allocId);
  if(tcont != 0)
  {
    uint64_t nowNano = Simulator::Now().GetNanoSeconds();
    tcont->ReceiveStatusReport (report, nowNano);
  }
}



const Ptr<XgponXgtcBwmap> 
XgponOltDbaEngine::GenerateBwMap ()  //unit: word; we assume that multiple-thread dba is not used.
{
  NS_LOG_FUNCTION(this);

  uint64_t nowNano = Simulator::Now().GetNanoSeconds();
  //std::cout << "secondsNano: " << nowNano << std::endl; 
  const Ptr<XgponOltPloamEngine>& ploamEngine = m_device->GetPloamEngine();

  const Ptr<XgponPhy>& commonPhy = m_device->GetXgponPhy();
  uint32_t usPhyFrameSize = commonPhy->GetUsPhyFrameSizeInWord();
  //usPhyFrameSize= 9720 words, equivalent to 2488Mbps US

  uint16_t guardTime = commonPhy->GetUsMinimumGuardTime ();

  //carry out initialization before the loop
  m_bursts.ClearBurstInfoList( );
  uint32_t numScheduledTconts= 0;
  Prepare2ProduceBwmap ( );
  Ptr<XgponTcontOlt> tcontOlt = GetFirstTcontOlt ( );
  
  uint16_t allocatedSize = m_extraInLastBwmap;
  NS_ASSERT_MSG((m_extraInLastBwmap < (usPhyFrameSize - 10)), "the last bwmap over-allocated too much!!!");


  do 
  {
    uint32_t size2Assign = CalculateAmountData2Upload (tcontOlt, allocatedSize, nowNano);
    //enable the below output to see the details of the serving TCONT, TCONT Type, associated ONU and how much of Bytes requested by the TCONT
    //std::cout << "servingTcont-" << tcontOlt->GetTcontType() << "-" << tcontOlt->GetOnuId() << "-size2Assign-" << size2Assign*4 << "-Bytes"<< std::endl;
    //limit the over-allocation to one half of the upstream frame size.
    uint32_t largestAssign = (3 * usPhyFrameSize) / 2 - allocatedSize;
    if(size2Assign > largestAssign) size2Assign = largestAssign;  


    if(size2Assign > 0 && numScheduledTconts < MAX_TCONT_PER_BWMAP)
    {
			//std::cout << "oltDBA,atMicro," << nowNano/1000 << ",bytesToAssign," << size2Assign*4 << ",allocId," << tcontOlt->GetAllocId() << std::endl;
			      
			Ptr<XgponOltDbaPerBurstInfo> perBurstInfo = m_bursts.GetBurstInfo4TcontOlt(tcontOlt);      
      if(perBurstInfo!=0)
      {
        SetServedTcont(tcontOlt->GetAllocId());
        const Ptr<XgponLinkInfo>& linkInfo = ploamEngine->GetLinkInfo(tcontOlt->GetOnuId());

        if(perBurstInfo->GetBwAllocNumber() == 0) 
        {
          perBurstInfo->Initialize(tcontOlt->GetOnuId(), linkInfo->GetPloamExistAtOnu4OLT(), linkInfo->GetCurrentProfile(), guardTime, 
                              commonPhy->GetUsFecBlockDataSize(), commonPhy->GetUsFecBlockSize());

          //Create the first bwalloc; starttime will be set when producing bwmap from all bursts
          Ptr<XgponXgtcBwAllocation> bwAlloc = Create<XgponXgtcBwAllocation> (tcontOlt->GetAllocId(), true, linkInfo->GetPloamExistAtOnu4OLT(), 0, size2Assign, 0, linkInfo->GetCurrentProfileIndex());
          perBurstInfo->AddOneNewBwAlloc(bwAlloc, tcontOlt);
          allocatedSize += perBurstInfo->GetFinalBurstSize( );
          numScheduledTconts++;
        }
        else
        {
          if(perBurstInfo->FindBwAlloc(tcontOlt)==0)//bwalloc not already present
          {
            //Create the bwalloc
            Ptr<XgponXgtcBwAllocation> bwAlloc = Create<XgponXgtcBwAllocation> (tcontOlt->GetAllocId(), true, linkInfo->GetPloamExistAtOnu4OLT(), 0xFFFF, size2Assign, 0, 0);
            uint32_t orgBurstSize = perBurstInfo->GetFinalBurstSize( );
            perBurstInfo->AddOneNewBwAlloc(bwAlloc, tcontOlt);
            allocatedSize += perBurstInfo->GetFinalBurstSize( ) - orgBurstSize;
            numScheduledTconts++;
          }
          else
          {
            //BwAllocation already exists, add extra allocation to the existing bwAlloc
            Ptr<XgponXgtcBwAllocation> bwAlloc = perBurstInfo->FindBwAlloc(tcontOlt);
            uint32_t orgBurstSize = perBurstInfo->GetFinalBurstSize( );
            perBurstInfo->AddToExistingBwAlloc( bwAlloc, size2Assign);
            allocatedSize += perBurstInfo->GetFinalBurstSize( ) - orgBurstSize;
            //do not increment numScheduledTconts
          }
        }
      }
    }    

    if(CheckAllTcontsServed()) // all T-CONTs had been considered.
      break;

    tcontOlt = GetNextTcontOlt ( );
      
  } while((allocatedSize < (usPhyFrameSize - 10)) && numScheduledTconts<MAX_TCONT_PER_BWMAP);

  //TODO: assert m_minimumSI >= 1
  m_aggregateAllocatedSize += allocatedSize;
  //std::cout << "Total AllocatedSize: " << allocatedSize*4 << " Bytes" << std::endl;
	//std::cout << "nextT4threshold ALLOCATION CYCLE END" << std::endl;
      

  //To make sure that starttime in the bwalloc is less than usPhyFrameSize. 
  //Otherwise, OLT may not find the corresponding BWMAP for this burst.
  //10*4 = 40bytes > gap+preamble+delimiter


  Ptr<XgponXgtcBwmap> map = m_bursts.ProduceBwmapFromBursts(nowNano, m_extraInLastBwmap, usPhyFrameSize);
  //if(map->GetNumberOfBwAllocation() > 0) map->Print(std::cout);

  if(allocatedSize > usPhyFrameSize)  //update the over-allocation size
  {  
    m_extraInLastBwmap = allocatedSize - usPhyFrameSize;
  } else m_extraInLastBwmap = 0;

  map->SetCreationTime(nowNano);
  m_servedBwmaps.push_back(map);  //used for receiving the corresponding bursts

  FinalizeBwmapProduction();

  return map;
}









const Ptr<XgponXgtcBwmap>& 
XgponOltDbaEngine::GetBwMap4CurrentBurst (uint64_t time)
{
  NS_LOG_FUNCTION(this);

  uint64_t slotSize = GetFrameSlotSize ( );  //slotSize in nanosecond
  uint64_t rtt = GetRtt();

  while(m_servedBwmaps.size() > 0)
  {
    const Ptr<XgponXgtcBwmap>& first = m_servedBwmaps.front();
    uint64_t startTime = first->GetCreationTime() + rtt;
    uint64_t endTime = startTime + slotSize;

    NS_ASSERT_MSG((time>startTime), "The corresponding bwmap was deleted too early!!!"); 

    if(time < endTime) return m_servedBwmaps.front();
    else
    { //first bwmap should be deleted since all of the corresponding bursts have been received (based on time).
      m_servedBwmaps.pop_front();
    }
  }

  NS_ASSERT_MSG(false, "this line should never been run!!!"); 
  return m_nullBwmap;
}


const Ptr<XgponBurstProfile>& 
XgponOltDbaEngine::GetProfile4BurstFromChannel(uint64_t time)
{
  NS_LOG_FUNCTION(this);

  Ptr<XgponXgtcBwmap> map = GetBwMap4CurrentBurst (time);
  NS_ASSERT_MSG((map!=0), "There is no corresponding bwmap for this burst!!!");

  uint32_t first = GetIndexOfBurstFirstBwAllocation (map, time);
  NS_ASSERT_MSG((first < map->GetNumberOfBwAllocation()), "strange index of the corresponding bwallocation!!!");

  const Ptr<XgponXgtcBwAllocation>& bwAlloc = map->GetBwAllocationByIndex (first);

  uint16_t onuId = ((m_device->GetConnManager( ))->GetTcontById (bwAlloc->GetAllocId() ))->GetOnuId();

  return ((m_device->GetPloamEngine())->GetLinkInfo (onuId))->GetProfileByIndex(bwAlloc->GetBurstProfileIndex());  
}



uint32_t 
XgponOltDbaEngine::GetIndexOfBurstFirstBwAllocation (const Ptr<XgponXgtcBwmap>& bwmap, uint64_t time)
{
  NS_LOG_FUNCTION(this);


  //calculate STARTTIME based on receiving time of the burst
  // (offsetTime * usRate)/(4*1000000000)
  uint64_t offsetSize64 = (time - (bwmap->GetCreationTime ( ) + GetRtt())) * GetUsLinkRate();   
  uint64_t tmpInt64 = 1000000000L;
  offsetSize64 = offsetSize64 / (4*tmpInt64);
  uint32_t offsetSize = (uint32_t) offsetSize64;  //unit: word

  int num = bwmap->GetNumberOfBwAllocation();
  for(int i=0; i<num; i++)
  {
    uint32_t startTime = (bwmap->GetBwAllocationByIndex(i))->GetStartTime();

    //Note that the starttime in BwAlloc starts from the XGTC header (after the preamble and delimiter). Thus, the burst will arrive before the starttime.
    if((startTime!= 0xFFFF) && offsetSize < startTime ) { return i; }
  }
  
  NS_ASSERT_MSG(false, "this line should never been run!!!"); 
  return (num+1); //return one impossible value.
}





















void 
XgponOltDbaEngine::PrintAllActiveBwmaps (void) 
{
  std::list< Ptr<XgponXgtcBwmap> >::iterator it = m_servedBwmaps.begin(); 
  std::list< Ptr<XgponXgtcBwmap> >::iterator end = m_servedBwmaps.end();

  while(it!=end)
  {
    std::cout << std::endl << std::endl;
    (*it)->Print(std::cout);
    std::cout << std::endl << std::endl;
    it++;
  }
}




uint32_t
XgponOltDbaEngine::GetRtt ()
{
  if(m_logicRtt == 0)
  {
    const Ptr<XgponChannel>& ch =  DynamicCast<XgponChannel, Channel>(m_device->GetChannel());
    m_logicRtt = 2 * (ch->GetLogicOneWayDelay ());
  }
  return m_logicRtt;
}

uint32_t
XgponOltDbaEngine::GetFrameSlotSize ()
{
  if(m_dsFrameSlotSizeInNano == 0)
  {
    m_dsFrameSlotSizeInNano = (m_device->GetXgponPhy( ))->GetDsFrameSlotSize ( );  //slotSize in nanosecond
  }
  return m_dsFrameSlotSizeInNano;
}

uint32_t
XgponOltDbaEngine::GetUsLinkRate ()
{
  if(m_usRate==0)
  {
    m_usRate = (m_device->GetXgponPhy ( ))->GetUsLinkRate();
  }
  return m_usRate;
}


bool
XgponOltDbaEngine::CheckServedTcont(uint64_t allocId)
{
  return m_bursts.CheckServedTcont(allocId);
}

void
XgponOltDbaEngine::SetServedTcont(uint64_t allocId)
{
  m_bursts.SetServedTcont(allocId);
}



}//namespace ns3
