/*
 * Copyright (c)  2013 The Provost, Fellows and Scholars of the
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin.
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
 * Authors: Jerome A Arokkiam <jerom2005raj@gmail.com> 
 * Xgiant here is Giant + intraTcontFairness originally implemented for the LTE + XGPON network architecture as in the below reference:
 * J. A. Arokkiam, K. N. Brown and C. J. Sreenan, "Optimised QoS-Aware DBA Mechanisms in XG-PON for Upstream Traffic in LTE Backhaul," 2016 IEEE 4th International Conference on Future Internet of Things and Cloud Workshops (FiCloudW), Vienna, 2016, pp. 361-368.
 *
 */

#include "ns3/log.h"
#include "ns3/simulator.h"

#include "xgpon-olt-dba-engine-xgiant.h"
#include "xgpon-olt-net-device.h"
#include "xgpon-phy.h"
#include "xgpon-olt-ploam-engine.h"
#include "xgpon-link-info.h"
#include "xgpon-burst-profile.h"
#include "xgpon-qos-parameters.h" 
#include "xgpon-tcont-olt.h"
#include "ns3/enum.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include <vector>


NS_LOG_COMPONENT_DEFINE ("XgponOltDbaEngineXgiant");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponOltDbaEngineXgiant);

TypeId
XgponOltDbaEngineXgiant::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOltDbaEngineXgiant")
    .SetParent<XgponOltDbaEngine> ()
    .AddConstructor<XgponOltDbaEngineXgiant> ();
  return tid;
}

TypeId
XgponOltDbaEngineXgiant::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}



XgponOltDbaEngineXgiant::XgponOltDbaEngineXgiant () : XgponOltDbaEngine(),
  m_lastScheduledAllocIndex(0),
  m_nullTcont(0),
  m_t1Served(false), m_t2Served(false), m_t3Served(false), m_t4Served(false),
  m_firstServedT1Index(0), m_lastServedT1Index(0), 
  m_firstServedT2Index(1), m_lastServedT2Index(1), 
  m_firstServedT3Index(2), m_lastServedT3Index(2), 
  m_firstServedT4Index(3), m_lastServedT4Index(3),
  m_nextCycleTcontIndex(0), 
  m_stop(false),
  m_t3FirstRound(true), 
  m_nonBestEffortAllocationInWords(0), 
  m_totalAllocationInWords(0)
{
  m_usAllTcons.clear();
}



XgponOltDbaEngineXgiant::~XgponOltDbaEngineXgiant ()
{
}

void
XgponOltDbaEngineXgiant::DoInitialize()
{
}


void
XgponOltDbaEngineXgiant::AddTcontToDbaEngine (ns3::Ptr<ns3::XgponTcontOlt>& alloc)
{
  NS_LOG_FUNCTION(this);
  /* Each TCONT is updated with its respective service rates and service intervals (only min and max SI for XGIANT)
   * Additionally, in OLT, tcont should have the aggregation for per TCONT type service rate for highly complex DBAs (this is not yet implemented)
   * In order to store these values, instead of using 4 pairs of <XgponOltTcont,XgponOltDbaParametersXgiant> for 4 TCONT types, a single list of <XgponOltTcont> will be used. 
   * Features of XGPON-OLT-DBA-PARAMETERS-XGIANT has to be ported to xgpon-olt-tcont for now.
   * XgponOltDbaParametersXgiant has to completely implemented in XgponOltTcont
   */ 

  //initially, allocatedRate, sI are calculated
  XgponQosParameters::XgponTcontType type = alloc->GetTcontType();
  alloc->CalculateTcontQosParameters(type);
  //then allocationWords is calculated.
  alloc->SetAllocationWords (GetAllocationBytesFromRateAndServiceInterval(alloc->GetAllocatedRate(), alloc->GetServiceInterval()));
  //sI timer is implemented as a parameter of tcont olt. One TCONT can have only one type...!!!

  //TODO: these will be used to prevent BE starvation
  //by reserving a portion of total US transmission opportunity.
  if ( type != XgponQosParameters::XGPON_TCONT_TYPE_4 )
    m_nonBestEffortAllocationInWords += alloc->GetAllocationWords(); 	//total BW requirement without BE
  m_totalAllocationInWords += alloc->GetAllocationWords();	//total BW requirement including BE
  m_usAllTcons.push_back(alloc);
  //std::cout << "type: " << type << ", maxAB: " << 4*(alloc->GetAllocationWords()) << std::endl;
  return;
}

const Ptr<XgponTcontOlt>&
XgponOltDbaEngineXgiant::GetFirstTcontOlt ( )
{
//return the first tcont in the list of olt tconts, if the list is not empty
//m_stop is reset to false at the beginning of every dba cycle

  NS_LOG_FUNCTION(this);
  m_stop=false;
  NS_ASSERT_MSG(!m_usAllTcons.empty(), "No tconts available to be served");

  // T1 is always served first every alloc cycle
	//FirstServedT1Index is always updated here as this is the first served T1 index
	//since T1 is served, its condition should be true
  m_tcontIterator = m_usAllTcons.begin() + m_lastServedT1Index; //start with T1 last served index
	NS_ASSERT_MSG( ((*m_tcontIterator)->GetTcontType()) == XgponQosParameters::XGPON_TCONT_TYPE_1, "T1 should always be treated first every alloc cycle");
  m_firstServedT1Index = m_lastServedT1Index;
  //std::cout << "nextTcontType (First): " << (*m_tcontIterator)->GetTcontType() << std::endl;
	return (*m_tcontIterator);
}

const Ptr<XgponTcontOlt>&
XgponOltDbaEngineXgiant::GetCurrentTcontOlt ( )
{
  NS_LOG_FUNCTION(this);
  return (*m_tcontIterator);
}



const Ptr<XgponTcontOlt>&
XgponOltDbaEngineXgiant::GetNextTcontOlt ( )
{

  NS_LOG_FUNCTION(this);
  return (*m_tcontIterator);
	
}

//10th May 2016. Finalised XGIANT.
//All Tconts are visited at least once before the cycle of served tconts repeated. This is valid even when the allocation cycle goes to more than 1. But when all the tconts are served once, the allocation cycle is broken in the middle. So at the beginning of next allocation cycle, tconts are served from the beginning of tcont loop. 
//By practice, 2-3 allocation cycle is required to complete one tcont cycle, given 0.2, 1, 1.5 and 1 for fixed, Assured, Non-Assured and BE. So as long as the SI >= 3, there will be no conflict.

bool
XgponOltDbaEngineXgiant::CheckAllTcontsServed ()
{
	

  XgponQosParameters::XgponTcontType type = (*m_tcontIterator)->GetTcontType();

  if (type == XgponQosParameters::XGPON_TCONT_TYPE_1)
  {
		m_lastServedT1Index += 4;
		if (m_lastServedT1Index >= m_usAllTcons.size())
			m_lastServedT1Index = 0;
		m_tcontIterator = m_usAllTcons.begin() + m_lastServedT1Index;
	}
  else if (type == XgponQosParameters::XGPON_TCONT_TYPE_2)
  {
		m_lastServedT2Index += 4;
		if (m_lastServedT2Index >= m_usAllTcons.size())
			m_lastServedT2Index = 1;
		m_tcontIterator = m_usAllTcons.begin() + m_lastServedT2Index;
	}
  else if (type == XgponQosParameters::XGPON_TCONT_TYPE_3)
  {
		m_lastServedT3Index += 4;
		if (m_lastServedT3Index >= m_usAllTcons.size())
			m_lastServedT3Index = 2;
		m_tcontIterator = m_usAllTcons.begin() + m_lastServedT3Index;
	}
  else 
  {
		NS_ASSERT (type == XgponQosParameters::XGPON_TCONT_TYPE_4);
		m_lastServedT4Index += 4;
		if (m_lastServedT4Index >= m_usAllTcons.size())
			m_lastServedT4Index = 3;
		m_tcontIterator = m_usAllTcons.begin() + m_lastServedT4Index;
	}

	//CHECK FOR BOUNDARY CONDITIONS
	//************************************************************************************************************
	if ( (type == XgponQosParameters::XGPON_TCONT_TYPE_1) && (m_firstServedT1Index == m_lastServedT1Index) )
  {
    m_tcontIterator = m_usAllTcons.begin() + m_lastServedT2Index; 
    m_firstServedT2Index = m_lastServedT2Index;
    //std::cout << "tcontType: " << (*m_tcontIterator)->GetTcontType() << ", distance: " << std::distance(m_usAllTcons.begin(), m_tcontIterator) << std::endl;
    NS_ASSERT_MSG( ((*m_tcontIterator)->GetTcontType() == XgponQosParameters::XGPON_TCONT_TYPE_2), "T2 should be served after T1");
    return m_stop;
	}
      
  if ((type == XgponQosParameters::XGPON_TCONT_TYPE_2) && (m_firstServedT2Index == m_lastServedT2Index) )
  {
    m_tcontIterator = m_usAllTcons.begin() + m_lastServedT3Index;
    m_firstServedT3Index = m_lastServedT3Index;
    NS_ASSERT_MSG( ((*m_tcontIterator)->GetTcontType() == XgponQosParameters::XGPON_TCONT_TYPE_3), "T3 should be served after T2");
		m_t3FirstRound = true;
    return m_stop;
  }

  //for T3 and T4, keep serving the next tcont untill the firstServedT3/T4Index is reached
  //but only for T3, GIR is iniitially granted and if more space available, PIR-GIR is given as well, after polling all the t4. Once all T3 is granted upto PIR, then the t4 requests are considered. TODO: T4 should have a minimum reservation (eg:10% of XGPON US capacity) to avoid BE starvation
  if ( (type == XgponQosParameters::XGPON_TCONT_TYPE_3) && (m_firstServedT3Index == m_lastServedT3Index) )
  {
		m_tcontIterator = m_usAllTcons.begin() + m_lastServedT4Index; 
    m_firstServedT4Index = m_lastServedT4Index;
    NS_ASSERT_MSG( ((*m_tcontIterator)->GetTcontType() == XgponQosParameters::XGPON_TCONT_TYPE_4), "T4 should be served after T3");
		//m_t3FirstRound = true;
    return m_stop;
  }


  if ((type == XgponQosParameters::XGPON_TCONT_TYPE_4) && (m_firstServedT4Index == m_lastServedT4Index))
  {
    if (m_t3FirstRound == true)
    {
      m_t3FirstRound = false;
			m_tcontIterator = m_usAllTcons.begin() + m_lastServedT3Index;
			m_firstServedT3Index = m_lastServedT3Index;
			NS_ASSERT_MSG( ((*m_tcontIterator)->GetTcontType() == XgponQosParameters::XGPON_TCONT_TYPE_3), "T3 should be served after T4 at end of 1st round");
    }
    else
    {
			m_stop = true;
      m_t3FirstRound = true;
      m_lastServedT1Index = 0;
      m_lastServedT2Index = 1;
      m_lastServedT3Index = 2;
      m_lastServedT4Index = 3;
    }
		
		return m_stop;
  }
  
	return m_stop;
}


void
XgponOltDbaEngineXgiant::UpdateTcontOltForNextCycle()
{
  m_nextCycleTcontIndex =  std::distance(m_usAllTcons.begin(), m_tcontIterator);
}

void
XgponOltDbaEngineXgiant::Prepare2ProduceBwmap ( )
{
}

void
XgponOltDbaEngineXgiant::FinalizeBwmapProduction (){

  std::vector<Ptr<XgponTcontOlt> >::iterator it;
	for (it = m_usAllTcons.begin(); it !=m_usAllTcons.end(); it++)
  {
		if( (*it)->GetPIRtimerValue() > XgponOltDbaEngineXgiant::TIMER_EXPIRE_VALUE)
		{
		  //std::cout << "tcontType:timer: " << (*it)->GetPIRtimerValue() << std::endl;
      (*it)->UpdatePIRtimer();
    }
    if( ( (*it)->GetTcontType() == XgponQosParameters::XGPON_TCONT_TYPE_3) || ( (*it)->GetTcontType() == XgponQosParameters::XGPON_TCONT_TYPE_4) )
    {
			if(((*it)->GetGIRtimerValue()) > XgponOltDbaEngineXgiant::TIMER_EXPIRE_VALUE)
				(*it)->UpdateGIRtimer();
    }
  }   
}

uint32_t
XgponOltDbaEngineXgiant::GetAllocationBytesFromRateAndServiceInterval(uint32_t rate, uint16_t si)
{
  //uint128_t tmp128;
  uint64_t tmp64;
  uint32_t tmp32;


  tmp64=(uint64_t)rate*(uint64_t)GetFrameSlotSize(); 	//GetFrameSlotSize()=125us=125000ns
  tmp64=tmp64*(uint64_t)si;  	//rate is in bps and frame slot size is in nanoseconds
  tmp64=tmp64/1000000000;     	//Get value in bits
  if ((tmp64%(32))!=0)
	tmp64 = (tmp64/32)*32;  //Make the value a multiple of 32 (for unit:word)
  NS_ASSERT_MSG(tmp64%(32)==0, "Cannot assign that rate to the connection since it will not be a multiple of 4 bytes (one word).");
  tmp32=tmp64/32;        	//Convert bits to words

  return tmp32;			//unit:word

}


uint32_t
XgponOltDbaEngineXgiant::CalculateAmountData2Upload (const Ptr<XgponTcontOlt>& tcontOlt, uint32_t allocatedSize, uint64_t nowNano)
{
  NS_LOG_FUNCTION(this);
  uint32_t size2Assign = 0;
  uint32_t sizeRemaining = 0;

	Ptr<XgponPhy> phy = m_device->GetXgponPhy();
  uint32_t usPhyFrameSize = phy->GetUsPhyFrameSizeInWord();
  Ptr<XgponBurstProfile> burstProfile = m_device->GetPloamEngine()->GetLinkInfo(tcontOlt->GetOnuId())->GetCurrentProfile();

	size2Assign = tcontOlt->CalculateRemainingDataToServe(GetRtt(), GetFrameSlotSize());
	//std::cout << "size2Assign " << size2Assign << ", tcontType: " << tcontOlt->GetTcontType() << std::endl;
	
	if((tcontOlt->GetTcontType()) == XgponQosParameters::XGPON_TCONT_TYPE_1)
  {
    if((tcontOlt->GetPIRtimerValue()) == XgponOltDbaEngineXgiant::TIMER_EXPIRE_VALUE)
    {
      size2Assign = tcontOlt->GetAllocationWords();
      tcontOlt->ResetPIRtimer(); 
    }
  }
  else if ((tcontOlt->GetTcontType()) == XgponQosParameters::XGPON_TCONT_TYPE_2)
  {
    if((tcontOlt->GetPIRtimerValue()) == XgponOltDbaEngineXgiant::TIMER_EXPIRE_VALUE)
    {
      size2Assign = tcontOlt->CalculateRemainingDataToServe(GetRtt(), GetFrameSlotSize()); //requested bandwidth?
      //std::cout << "at," << nowNano << ",alloc," << tcontOlt->GetAllocId() << ",PIR,requested," << 4*size2Assign << ",Bytes";
      if(size2Assign>0)
      {
        if (size2Assign<4)
          size2Assign =4; //smallest allocation for receiving data from ONU
        if(size2Assign>(tcontOlt->GetAllocationWords()))
          size2Assign = tcontOlt->GetAllocationWords(); //assigned US data for T2 is the min (requeste, allocationWords)
        if(!CheckServedTcont(tcontOlt->GetAllocId()))
          size2Assign+=1; //This T-CONT was not served before in this bwMap, add one word for queue status report
      }
      else //size2assign = 0, ensure the allocOlt is polled
				size2Assign = 1;

      //std::cout << ",given," << 4*size2Assign << ",Bytes" << std::endl;
			tcontOlt->ResetPIRtimer();
    }
    
    //~ else
      //~ std::cout << "type 2 request has no expiry" << std::endl;
  }

  //for T3, if in first round (GIR), then only the min(request, allocationWordsBasedOnABmin) is given. When in second round, then min(request, allocationWordsBasedOnABsur) is given. GIR:PIR = 0.4
  //TODO: T3 should have two SI for GIR and PIR respectively, triggering allocation two times than that for T1/T2. However, since only one SI is used for now, reset is only done at second round. However, this has to be considered for ideal XGIANT implementation
  else if ((tcontOlt->GetTcontType()) == XgponQosParameters::XGPON_TCONT_TYPE_3)
  {
    if (m_t3FirstRound == true)
    {
      if((tcontOlt->GetGIRtimerValue()) == XgponOltDbaEngineXgiant::TIMER_EXPIRE_VALUE)
      {
        size2Assign = tcontOlt->CalculateRemainingDataToServe(GetRtt(), GetFrameSlotSize()); //requested bandwidth?
        //std::cout << "at," << nowNano << ",alloc," << tcontOlt->GetAllocId() << ",GIR,requested," << 4*size2Assign << ",Bytes";
        if(size2Assign>0)
				{
					if (size2Assign<4)
						size2Assign =4; //smallest allocation for receiving data from ONU
          if(size2Assign>0.2*(tcontOlt->GetAllocationWords()))
            size2Assign = 0.2*(tcontOlt->GetAllocationWords()); //assigned US data for T3 is the min (request,05*allcationWords), where, GIR = 0.4*allocationWords. TODO: need to make 0.4 accessible at user level, to be able to change the ratio of GIR:PIR
					if(!CheckServedTcont(tcontOlt->GetAllocId()))
						size2Assign+=1; //This T-CONT was not served before in this bwMap, add one word for queue status report
				}
        else //no need to poll t3 in first round as there is a possible second round of allocation. If it has no allocation in 2nd round, then poll
					size2Assign =0;

				//std::cout << ",given," << 4*size2Assign << ",Bytes" << std::endl;
				tcontOlt->ResetGIRtimer(); //GIR timer update in first round
      }

    //~ else
      //~ std::cout << "type 3 GIR request has no expiry" << std::endl;
    }
    else
    {
      if((tcontOlt->GetPIRtimerValue()) == XgponOltDbaEngineXgiant::TIMER_EXPIRE_VALUE)
      {
        size2Assign = tcontOlt->CalculateRemainingDataToServe(GetRtt(), GetFrameSlotSize()); //requested bandwidth
        //std::cout << "at," << nowNano << ",alloc," << tcontOlt->GetAllocId() << ",PIR,requested," << 4*size2Assign << ",Bytes";
        if(size2Assign>0)
				{
					if (size2Assign<4)
						size2Assign =4; //smallest allocation for receiving data from ONU
          if(size2Assign>0.6*(tcontOlt->GetAllocationWords()))
            size2Assign = 0.6*(tcontOlt->GetAllocationWords()); //assigned US data for T3 is the min (request,05*allcationWords), where, GIR = 0.5*allocationWords. TODO: need to make 0.4 accessible at user level, to be able to change the ratio of GIR:PIR
					if(!CheckServedTcont(tcontOlt->GetAllocId()))
						size2Assign+=1; //This T-CONT was not served before in this bwMap, add one word for queue status report
				}
				else
				size2Assign = 1; //poll t3 only in the second round for less overhead

				//std::cout << ",given," << 4*size2Assign << ",Bytes" << std::endl;
				tcontOlt->ResetPIRtimer(); //for T3, In the second round the PIR timer is updated 
      }

    //~ else
      //~ std::cout << "type 3 PIR request has no expiry" << std::endl;
    }
  }


  else //(*m_tcontIterator)->GetTcontType() == XgponQosParameters::XGPON_TCONT_TYPE_4
  {
    //verify this is in fact a t4
    NS_ASSERT_MSG( (tcontOlt->GetTcontType() == XgponQosParameters::XGPON_TCONT_TYPE_4), "Invalid Tcont Type detected at allocating grants");
      //for firstT3round t4 is only polled.
		if (m_t3FirstRound == true)
		{
			if((tcontOlt->GetGIRtimerValue()) == XgponOltDbaEngineXgiant::TIMER_EXPIRE_VALUE)
			{
				size2Assign = 0;
				//std::cout << "type-4 round 1 given: " << 4*size2Assign << " Bytes " << std::endl;
				tcontOlt->ResetGIRtimer();
			}
			//~ else
				//~ std::cout << "type 4 R1 request has no expiry" << std::endl;
		}
    else
    {
			if((tcontOlt->GetPIRtimerValue()) == XgponOltDbaEngineXgiant::TIMER_EXPIRE_VALUE)
			{
        size2Assign = tcontOlt->CalculateRemainingDataToServe(GetRtt(), GetFrameSlotSize()); //requested bandwidth
				//std::cout << "at," << nowNano << ",alloc," << tcontOlt->GetAllocId() << ",R2,requested," << 4*size2Assign << ",Bytes";
        if(size2Assign>0)
				{
					if (size2Assign<4)
						size2Assign =4; //smallest allocation for receiving data from ONU
					if(!CheckServedTcont(tcontOlt->GetAllocId()))
						size2Assign+=1; 
					if(size2Assign>(tcontOlt->GetAllocationWords()))
						size2Assign = tcontOlt->GetAllocationWords(); //assigned US data for T4 is the min (request,allcationWords)
				}
				else
					size2Assign = 1; //no need to poll t4 in the second round

				//std::cout << ",given," << 4*size2Assign << ",Bytes" << std::endl;
      	tcontOlt->ResetPIRtimer();
      }
			//~ else
				//~ std::cout << "type 4 R2 request has no expiry" << std::endl;
    }

  }

  bool isNewBurstNecessary=m_bursts.IsNewBurstNecessary(tcontOlt);


  /**************************************
   *
   * Get the remaining size in a frame
   *
   **************************************/

  if(isNewBurstNecessary)
  {

    if(burstProfile->GetFec())
    {
      //Get the remaining size available for XGTC data when a new burst is needed and FEC is enabled.
      uint32_t fecBlocks=(2+size2Assign)/(phy->GetUsFecBlockDataSize()/4); //Divide by four to convert to words; Add two to take XGTC header and trailer into account
      if((2+size2Assign)%(phy->GetUsFecBlockDataSize()/4)!=0)
        fecBlocks++;
      uint32_t codeWords=fecBlocks*(phy->GetUsFecBlockSize()-phy->GetUsFecBlockDataSize())/4;
      sizeRemaining = usPhyFrameSize-allocatedSize-phy->GetUsMinimumGuardTime()-(burstProfile->GetPreambleLen()+burstProfile->GetDelimiterLen())/4-codeWords-2; //Two words for XGTC headers.

    }
    else
    {
      //Get the remaining size available for XGTC data when a new burst is needed and FEC is disabled.
      sizeRemaining = usPhyFrameSize-allocatedSize-phy->GetUsMinimumGuardTime()-(burstProfile->GetPreambleLen()+burstProfile->GetDelimiterLen())/4-2;
    }
  }
  else
  {
    if(burstProfile->GetFec())
    {
      //Get the remaining size available for XGTC data when the ONU has been served before and FEC is enabled.
      Ptr<XgponOltDbaPerBurstInfo> burstInfo=m_bursts.GetBurstInfo4TcontOlt(tcontOlt);
      uint32_t oldDataWords=burstInfo->GetHeaderTrailerDataSize()/phy->GetUsFecBlockDataSize()/4;
      uint32_t oldFecBlocks=oldDataWords/(phy->GetUsFecBlockDataSize()/4); //Divide by four to convert to words
      if(oldDataWords%(phy->GetUsFecBlockDataSize()/4)!=0)
        oldFecBlocks++;
      uint32_t oldCodeWords=oldFecBlocks*(phy->GetUsFecBlockSize()-phy->GetUsFecBlockDataSize())/4;


      uint32_t newFecBlocks=(2+size2Assign+oldDataWords)/(phy->GetUsFecBlockDataSize()/4); //Divide by four to convert to words; Add two to take XGTC header and trailer into account
      if((2+size2Assign+oldDataWords)%(phy->GetUsFecBlockDataSize()/4)!=0)
        newFecBlocks++;
      uint32_t newCodeWords=newFecBlocks*(phy->GetUsFecBlockSize()-phy->GetUsFecBlockDataSize())/4;
      sizeRemaining = usPhyFrameSize-(allocatedSize-oldCodeWords)-phy->GetUsMinimumGuardTime()-(burstProfile->GetPreambleLen()+burstProfile->GetDelimiterLen())/4-newCodeWords-2; //Two words for XGTC headers.

      /*
      Ptr<XgponOltDbaPerBurstInfo> burstInfo=m_bursts.GetBurstInfo4TcontOlt(tcontOlt);
      uint32_t oldDataWords=burstInfo->GetHeaderTrailerDataSize()/phy->GetUsFecBlockDataSize()/4;
      uint32_t oldFecBlocks=oldDataWords/(phy->GetUsFecBlockDataSize()/4); //Divide by four to convert to words
      if(oldDataWords%(phy->GetUsFecBlockDataSize()/4)!=0)
        oldFecBlocks++;
      uint32_t oldCodeWords=oldFecBlocks*(phy->GetUsFecBlockSize()-phy->GetUsFecBlockDataSize())/4;

      uint32_t newDataWords=oldDataWords+size2Assign; //The new burst should fill up the frame completely
      uint32_t newFecBlocks=newDataWords/(phy->GetUsFecBlockDataSize()/4); //Divide by four to convert to words
      uint32_t newCodeWords=newFecBlocks*(phy->GetUsFecBlockSize()-phy->GetUsFecBlockDataSize())/4;
      if(newDataWords%(phy->GetUsFecBlockDataSize()/4)!=0)
      {
        newFecBlocks++;
        uint32_t shortenedDataBlockSize=newDataWords%(phy->GetUsFecBlockDataSize()/4);
        newCodeWords=newCodeWords+shortenedDataBlockSize+(phy->GetUsFecBlockSize()-phy->GetUsFecBlockDataSize())/4;
      }
      */
      sizeRemaining = usPhyFrameSize-(allocatedSize-oldCodeWords);
    }
    else
    {
      //Get the remaining size available for XGTC data when the ONU has been served before and FEC is disabled.
      sizeRemaining = usPhyFrameSize-allocatedSize;
    }
  }


  /**************************************
   *
   * Prevent the bwMap from granting more than
   * what is allowed by the US PHY frame.
   *
   **************************************/
  if(size2Assign>sizeRemaining)
  {
    //There is not enough space to send all that the user requested
    if(burstProfile->GetFec())
    {
      //FEC is enabled. We need to compute what is the maximum amount of words we can transmit,
      //considering the FEC overhead will be smaller if the transmitted data is smaller.
      if(isNewBurstNecessary)
      {
        //Computing the maximum data transmission possible, bearing in mind that a new burst is necessary to serve the T-CONT.
        uint32_t sizeRemainingNoFec=usPhyFrameSize-allocatedSize-phy->GetUsMinimumGuardTime()-(burstProfile->GetPreambleLen()+burstProfile->GetDelimiterLen())/4;

        uint32_t fecBlocs=sizeRemainingNoFec/(phy->GetUsFecBlockSize()/4);
        uint32_t dataRemainder=0;
        if(sizeRemainingNoFec%(phy->GetUsFecBlockSize()/4)!=0)
        {
          dataRemainder=sizeRemainingNoFec-fecBlocs*(phy->GetUsFecBlockSize()/4)-(phy->GetUsFecBlockSize()-phy->GetUsFecBlockDataSize())/4;
        }
        sizeRemaining=fecBlocs*phy->GetUsFecBlockDataSize()/4+dataRemainder-2;
      }
      else
      {
        //Computing the maximum possible data transmission, bearing in mind that the ONU was served before.
        Ptr<XgponOltDbaPerBurstInfo> burstInfo=m_bursts.GetBurstInfo4TcontOlt(tcontOlt);
        uint32_t oldDataWords=burstInfo->GetHeaderTrailerDataSize()/phy->GetUsFecBlockDataSize()/4;
        uint32_t oldFecBlocks=oldDataWords/(phy->GetUsFecBlockDataSize()/4); //Divide by four to convert to words
        if(oldDataWords%(phy->GetUsFecBlockDataSize()/4)!=0)
          oldFecBlocks++;
        uint32_t oldCodeWords=oldFecBlocks*(phy->GetUsFecBlockSize()-phy->GetUsFecBlockDataSize())/4;

        uint32_t newCodedBurstSize=usPhyFrameSize-(allocatedSize-oldCodeWords-oldDataWords);
        uint32_t newFecBlocks = newCodedBurstSize/(phy->GetUsFecBlockSize()/4);
        uint32_t newDataWords = newFecBlocks*(phy->GetUsFecBlockDataSize()/4);
        if(newCodedBurstSize%(phy->GetUsFecBlockSize()/4)!=0)
        {
          newFecBlocks++;
          uint32_t shortenedCodedBlockSize=newCodedBurstSize%(phy->GetUsFecBlockSize()/4);
          newDataWords=newDataWords+shortenedCodedBlockSize-(phy->GetUsFecBlockSize()-phy->GetUsFecBlockDataSize())/4;
        }
        sizeRemaining=newDataWords-oldDataWords;
      }
    }
    return sizeRemaining;
  }
  else
  {
    return size2Assign;
  }

}




}//namespace ns3
