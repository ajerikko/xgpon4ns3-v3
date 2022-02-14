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
 * Authors: Jerome A Arokkiam <jerome.arokkiam@insight-centre.org> 
 */

 //brought from xgpon-olt-dba-engine-ebu.cc from 4c server, copied into ebu and modified as per EBU algorithm

#include "ns3/log.h"
#include "ns3/simulator.h"

#include "xgpon-olt-dba-engine-ebu.h"
#include "xgpon-olt-net-device.h"
#include "xgpon-phy.h"
#include "xgpon-olt-ploam-engine.h"
#include "xgpon-link-info.h"
#include "xgpon-burst-profile.h"
#include "xgpon-tcont-olt.h"
#include "xgpon-qos-parameters.h" 

#include "ns3/enum.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include <vector>


NS_LOG_COMPONENT_DEFINE ("XgponOltDbaEngineEbu");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (XgponOltDbaEngineEbu);

TypeId
XgponOltDbaEngineEbu::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::XgponOltDbaEngineEbu")
    .SetParent<XgponOltDbaEngine> ()
    .AddConstructor<XgponOltDbaEngineEbu> ();
  return tid;
}

TypeId
XgponOltDbaEngineEbu::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}



XgponOltDbaEngineEbu::XgponOltDbaEngineEbu () : XgponOltDbaEngine(),
  m_lastScheduledAllocIndex(0),
  m_nullTcont(0),
  m_t1Served(false), m_t2Served(false), m_t3Served(false), m_t4Served(false),
  m_firstServedT1Index(0), m_lastServedT1Index(65535),
  m_firstServedT2Index(0), m_lastServedT2Index(65535), 
  m_firstServedT3Index(0), m_lastServedT3Index(65535),
  m_firstServedT4Index(0), m_lastServedT4Index(65535),
  m_nextCycleTcontIndex(0), 
  m_stop(false),
  m_t3FirstRound(true),
  m_nonBestEffortAllocationInWords(0),
  m_totalAllocationInWords(0), 
  m_minimumSI(5), 
  m_aggregatedVW_t2(0), 
  m_aggregatedVW_t3(0), 
  m_aggregatedVW_t4(0)


{
  m_allocCycleCount = m_minimumSI; 
  m_usAllTcons.clear();
}



XgponOltDbaEngineEbu::~XgponOltDbaEngineEbu ()
{
}

void
XgponOltDbaEngineEbu::DoInitialize()
{
}

void
XgponOltDbaEngineEbu::AddTcontToDbaEngine (ns3::Ptr<ns3::XgponTcontOlt>& alloc)
{
  NS_LOG_FUNCTION(this);
  /* Each TCONT is updated with its respective service rates and service intervals (only min and max SI for EBU)
   * Additionally, in OLT, tcont should have the aggregation for per TCONT type service rate for highly complex DBAs (this is not yet implemented)
   * In order to store these values, instead of using 4 pairs of <XgponOltTcont,XgponOltDbaParametersGiant> for 4 TCONT types, a single list of <XgponOltTcont> will be used. 
   * Features of XGPON-OLT-DBA-PARAMETERS-EBU has to be ported to xgpon-olt-tcont for now.
   * XgponOltDbaParametersGiant has to completely implemented in XgponOltTcont
   */ 

  //initially, allocatedRate, sI are calculated
  XgponQosParameters::XgponTcontType type = alloc->GetTcontType();
  alloc->CalculateTcontQosParameters(type);
  //then allocationWords is calculated.
  alloc->SetAllocationWords (GetAllocationBytesFromRateAndServiceInterval(alloc->GetAllocatedRate(), alloc->GetServiceInterval()));
  //then variable_byte is reset to ABmin
	alloc->SetVariableWord(alloc->GetAllocationWords());


  //SI timer is implemented as a parameter of tcont olt. One TCONT can have only one type...!!!

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
XgponOltDbaEngineEbu::GetFirstTcontOlt ( )
{
//return the first tcont in the list of olt tconts, if the list is not empty
//m_stop is reset to false at the beginning of every dba cycle

  NS_LOG_FUNCTION(this);

  m_stop=false;

  NS_ASSERT_MSG(!m_usAllTcons.empty(), "No tconts available to be served");

  m_tcontIterator = m_usAllTcons.begin() + m_nextCycleTcontIndex;

  XgponQosParameters::XgponTcontType type = (*m_tcontIterator)->GetTcontType();
  //std::cout << "firstType-" << type << ", index-" << m_nextCycleTcontIndex << std::endl;

  //if no T1/T2 available, serve t3/t4 and update indexes accordingly
  if (type == XgponQosParameters::XGPON_TCONT_TYPE_1)
  {
    if (m_t1Served)
      m_lastServedT1Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
    else
    {
      m_firstServedT1Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
      m_t1Served = true;
    }
  }
  else if (type == XgponQosParameters::XGPON_TCONT_TYPE_2)
  {
    if (m_t2Served)
      m_lastServedT2Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
    else
    {
      m_firstServedT2Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
      m_t2Served = true;
    }
  }
  else if (type == XgponQosParameters::XGPON_TCONT_TYPE_3)
  {
    if (m_t3Served)
      m_lastServedT3Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
    else
    {
      m_firstServedT3Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
      m_t3Served = true;
    }
  }
  else if (type == XgponQosParameters::XGPON_TCONT_TYPE_4)
  {
    if (m_t4Served)
      m_lastServedT4Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
    else
    {
      m_firstServedT4Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
      m_t4Served = true;
    }
  }

  return (*m_tcontIterator);
}

const Ptr<XgponTcontOlt>&
XgponOltDbaEngineEbu::GetCurrentTcontOlt ( )
{
  NS_LOG_FUNCTION(this);
  return (*m_tcontIterator);
}



const Ptr<XgponTcontOlt>&
XgponOltDbaEngineEbu::GetNextTcontOlt ( )
{

  NS_LOG_FUNCTION(this);
  return (*m_tcontIterator);
}

//10th May 2016. Finalised EBU.
//All Tconts are visited at least once before the cycle of served tconts repeated. This is valid even when the allocation cycle goes to more than 1. But when all the tconts are served once, the allocation cycle is broken in the middle. So at the beginning of next allocation cycle, tconts are served from the beginning of tcont loop. 
//By practice, 2-3 allocation cycle is required to complete one tcont cycle, given 0.2, 1, 1.5 and 1 for fixed, Assured, Non-Assured and BE. So as long as the SI >= 3, there will be no conflict.

bool
XgponOltDbaEngineEbu::CheckAllTcontsServed ()
{

  XgponQosParameters::XgponTcontType type = (*m_tcontIterator)->GetTcontType();
  //std::cout << "checked type-" << type << std::endl;

  do 
  {
    m_tcontIterator++;
        if (m_tcontIterator == m_usAllTcons.end())
	  m_tcontIterator = m_usAllTcons.begin();
  } while ((*m_tcontIterator)->GetTcontType() != type);
  //while: to make sure only the same tcont types are served first

  if (type == XgponQosParameters::XGPON_TCONT_TYPE_1)
  {
    if (m_t1Served)
      m_lastServedT1Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
    else
    {
      m_firstServedT1Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
      m_t1Served = true;
    }
  }

  //if t2/t3/t4 is already served first, then m_t3/m_t3/t4Served will be true. so update lastServedIndex
  //else update only the firstServedIndex
  if (type == XgponQosParameters::XGPON_TCONT_TYPE_2)
  {
    if (m_t2Served)
      m_lastServedT2Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
    else
    {
      m_firstServedT2Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
      m_t2Served = true;
    }
  }
  if (type == XgponQosParameters::XGPON_TCONT_TYPE_3)
  {
    if (m_t3Served)
      m_lastServedT3Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
    else
    {
      m_firstServedT3Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
      m_t3Served = true;
    }
  }
  if (type == XgponQosParameters::XGPON_TCONT_TYPE_4)
  {
    if (m_t4Served)
      m_lastServedT4Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
    else
    {
      m_firstServedT4Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
      m_t4Served = true;
    }
  }

  //t1 and t2 are always served. However, t3 and t4 are served based on availability of bw in xgpon.
  //so m_startFromBeginningT3/T4 and lastServedT3Index/T4Index are used to remember last served tcont

  if ((type == XgponQosParameters::XGPON_TCONT_TYPE_1) && (m_firstServedT1Index == m_lastServedT1Index))
  {
    //bring the iterator to the begninning of the list, when all T1 is served,and look for the next tcont type available.
    m_t1Served = false;
    m_tcontIterator = m_usAllTcons.begin();
    m_tcontIterator = m_usAllTcons.begin() + m_nextCycleTcontIndex + 1;
    //std::cout << "tcontType: " << (*m_tcontIterator)->GetTcontType() << ", distanct: " << std::distance(m_usAllTcons.begin(), m_tcontIterator) << std::endl;
    
    NS_ASSERT_MSG( ((*m_tcontIterator)->GetTcontType() == XgponQosParameters::XGPON_TCONT_TYPE_2), "T2 should be served after T1");
		m_firstServedT2Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
    m_t2Served = true;
  }
  
      
  if ((type == XgponQosParameters::XGPON_TCONT_TYPE_2) && (m_firstServedT2Index == m_lastServedT2Index))
  {
    m_t2Served = false;
    m_tcontIterator = m_usAllTcons.begin() + m_nextCycleTcontIndex + 2;
    //std::cout << "tcontType: " << (*m_tcontIterator)->GetTcontType() << ", distance: " << std::distance(m_usAllTcons.begin(), m_tcontIterator) << std::endl;
		
    NS_ASSERT_MSG( ((*m_tcontIterator)->GetTcontType() == XgponQosParameters::XGPON_TCONT_TYPE_3), "T3 should be served after T2");
		m_firstServedT3Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
    m_t3Served = true;
    m_t3FirstRound = true;
  }

  //for T3 and T4, keep serving the next tcont untill the firstServedT3/T4Index is reached
  //but only for T3, GIR is iniitially granted and if more space available, PIR-GIR is given as well, after polling all the t4. Once all T3 is granted upto PIR, then the t4 requests are considered. TODO: T4 should have a minimum reservation (eg:10% of XGPON US capacity) to avoid BE starvation
  if ( (type == XgponQosParameters::XGPON_TCONT_TYPE_3) && (m_firstServedT3Index == m_lastServedT3Index) )
  {
    m_t3Served = false;
    m_tcontIterator = m_usAllTcons.begin() + m_nextCycleTcontIndex + 3;
    NS_ASSERT_MSG( ((*m_tcontIterator)->GetTcontType() == XgponQosParameters::XGPON_TCONT_TYPE_4), "T2 should be served after T1");
		m_firstServedT4Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
    m_t4Served = true;
	}

  if ((type == XgponQosParameters::XGPON_TCONT_TYPE_4) && (m_firstServedT4Index == m_lastServedT4Index))
  {
    m_t4Served = false;
    if (m_t3FirstRound == true)
    {
      m_t3FirstRound = false;
      m_tcontIterator = m_usAllTcons.begin() + m_nextCycleTcontIndex + 2;
			NS_ASSERT_MSG( ((*m_tcontIterator)->GetTcontType() == XgponQosParameters::XGPON_TCONT_TYPE_3), "T3 should be served after T4 first round");
			m_firstServedT3Index = std::distance(m_usAllTcons.begin(), m_tcontIterator);
			m_t3Served = true;
    }
    else
    {
			m_stop = true;
      m_t3FirstRound = true;
      m_tcontIterator = m_usAllTcons.begin();
			m_nextCycleTcontIndex = 0;
    }
  }

  //to prevent every allocation cycle to have infinite no of tconts cycle, when no allocation bytes are available.
  //if (m_nextCycleTcontIndex == std::distance(m_usAllTcons.begin(), m_tcontIterator))
  //  m_stop = true;
  //UpdateTcontOltForNextCycle();
  return m_stop;
}

void
XgponOltDbaEngineEbu::UpdateTcontOltForNextCycle()
{
  m_nextCycleTcontIndex =  std::distance(m_usAllTcons.begin(), m_tcontIterator);
}

void
XgponOltDbaEngineEbu::Prepare2ProduceBwmap ( )
{
}

void
XgponOltDbaEngineEbu::FinalizeBwmapProduction ()
{ 
  m_allocCycleCount--;

  std::vector<Ptr<XgponTcontOlt> >::iterator it;
  int32_t tempAggregatedVariableWord = 0;
  uint16_t tempTcontType;

  for (it = m_usAllTcons.begin(); it !=m_usAllTcons.end(); it++)
  {
		tempTcontType = (uint16_t)(*it)->GetTcontType(); 
    //std::cout << "type: " << tempTcontType <<  ", allocationBytes: " << 4*(it->GetAllocationWords()) << std::endl;
    if (tempTcontType == 1)
      continue;
    else if (tempTcontType == 2)
      tempAggregatedVariableWord = m_aggregatedVW_t2;
    else if (tempTcontType == 3)
      tempAggregatedVariableWord = m_aggregatedVW_t3;
    else
      tempAggregatedVariableWord = m_aggregatedVW_t4;

    if ( ((*it)->GetVariableWord() < 0) && (tempAggregatedVariableWord > 0) )
    {
      if ((tempAggregatedVariableWord + (*it)->GetVariableWord()) >= 0)
	      (*it)->SetVariableWord(0);
      else
	      (*it)->SetVariableWord(tempAggregatedVariableWord + (*it)->GetVariableWord());
    }

    //seperated out T2 and T3 aggregation patterns.
    if (tempTcontType == 2)
    {
      if(((*it)->GetPIRtimerValue()) == XgponOltDbaEngineEbu::TIMER_EXPIRE_VALUE)
      {
        if( (*it)->GetVariableWord() >= 0 )
	      {
	        tempAggregatedVariableWord += (*it)->GetVariableWord();
	        (*it)->SetVariableWord((*it)->GetAllocationWords());
	      }
	      else
	        (*it)->SetVariableWord((*it)->GetAllocationWords() + (*it)->GetVariableWord());
       
	      (*it)->ResetPIRtimer();
      }
      else
	      (*it)->UpdatePIRtimer();
    }

    //this block is specialised for tcont type 3, as per compliance with EBU
    else if (tempTcontType == 3)
    { 
      if(((*it)->GetGIRtimerValue()) == XgponOltDbaEngineEbu::TIMER_EXPIRE_VALUE)
      {  // VB(j) = min { VB(j)+AB(j) , AB(j) }
	      if( (*it)->GetVariableWord() >= 0 )
	      {
	        tempAggregatedVariableWord += 0.2*((*it)->GetVariableWord());
	        (*it)->SetVariableWord(0.2*((*it)->GetAllocationWords()));
	      }
	      else
	        (*it)->SetVariableWord(0.2*((*it)->GetAllocationWords() + (*it)->GetVariableWord()));
	      
	      (*it)->ResetGIRtimer();
      }
      else 
        (*it)->UpdateGIRtimer();     

      if((*it)->GetPIRtimerValue() == XgponOltDbaEngineEbu::TIMER_EXPIRE_VALUE)
      {  
        if( (*it)->GetVariableWord() >= 0 )
	      {
	      tempAggregatedVariableWord += 0.6*((*it)->GetVariableWord());
	      (*it)->SetVariableWord(0.6*((*it)->GetAllocationWords()));
	      }
	      else
	        (*it)->SetVariableWord(0.6*((*it)->GetAllocationWords() + (*it)->GetVariableWord()));
	    (*it)->ResetPIRtimer();
      }
      else 
	      (*it)->UpdatePIRtimer();     
      }

    //this block is for T4
    else
    {
      if(((*it)->GetPIRtimerValue()) == XgponOltDbaEngineEbu::TIMER_EXPIRE_VALUE)
      {  
	      if( (*it)->GetVariableWord() >= 0 )
	      {
	        tempAggregatedVariableWord += 0.5*((*it)->GetVariableWord());
	        (*it)->SetVariableWord(0.5*((*it)->GetAllocationWords()));
      	}
	      else
	        (*it)->SetVariableWord(0.5*((*it)->GetAllocationWords() + (*it)->GetVariableWord()));
	
	      (*it)->ResetPIRtimer();
      }
      else 
	      (*it)->UpdatePIRtimer();
    }

    if (tempTcontType == 2)
      m_aggregatedVW_t2 = tempAggregatedVariableWord;
    else if (tempTcontType == 3)
      m_aggregatedVW_t3 = tempAggregatedVariableWord;
    else
      m_aggregatedVW_t4 = tempAggregatedVariableWord;

  }


  if (m_allocCycleCount == 0) 
  {
    //std::cout << "expired all timers and reset m_allocCycleCount" << std::endl;
    //to make sure no more than the m_minimumSI*usPhyFrameSize is allocated in each multiple of m_minimumSI allocation cycles, to control the given policy for T1, T2, T3 and T4 in the given priority order.
    m_allocCycleCount = m_minimumSI;
    m_aggregatedVW_t2 = m_aggregatedVW_t3 = m_aggregatedVW_t4 = 0;
    
    m_nextCycleTcontIndex = std::distance(m_usAllTcons.begin(), m_tcontIterator);
		m_nextCycleTcontIndex = (m_nextCycleTcontIndex/4)*4;
  }

}

uint32_t
XgponOltDbaEngineEbu::GetAllocationBytesFromRateAndServiceInterval(uint32_t rate, uint16_t si)
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
XgponOltDbaEngineEbu::CalculateAmountData2Upload (const Ptr<XgponTcontOlt>& tcontOlt, uint32_t allocatedSize, uint64_t nowNano)
{
  NS_LOG_FUNCTION(this);
  uint32_t size2Assign = 0;
  uint32_t sizeRemaining = 0;
  int64_t tempVariableWord = tcontOlt->GetVariableWord();
  
  Ptr<XgponPhy> phy = m_device->GetXgponPhy();
  uint32_t usPhyFrameSize = phy->GetUsPhyFrameSizeInWord();
  Ptr<XgponBurstProfile> burstProfile = m_device->GetPloamEngine()->GetLinkInfo(tcontOlt->GetOnuId())->GetCurrentProfile();

	if((tcontOlt->GetTcontType()) == XgponQosParameters::XGPON_TCONT_TYPE_1)
  {
    if((tcontOlt->GetPIRtimerValue()) == XgponOltDbaEngineEbu::TIMER_EXPIRE_VALUE)
    {
      size2Assign = tcontOlt->GetAllocationWords();
      tcontOlt->ResetPIRtimer();
    }
    else 
      tcontOlt->UpdatePIRtimer();

  }

  else if ((tcontOlt->GetTcontType()) == XgponQosParameters::XGPON_TCONT_TYPE_2)
  {
    if (tempVariableWord >= 0)
    {
      size2Assign = tcontOlt->CalculateRemainingDataToServe(GetRtt(), GetFrameSlotSize()); //req. bw
			
			if(size2Assign>0)
      {
        if (size2Assign<4)
          size2Assign =4; //smallest allocation for receiving data from ONU
				if (size2Assign > tcontOlt->GetAllocationWords()) 
					size2Assign = tcontOlt->GetAllocationWords();
				if(!CheckServedTcont(tcontOlt->GetAllocId()))
					size2Assign+=1; //This T-CONT was not served before in this bwMap, add one word for queue status report
      }
    //If this is when the timer expired and there is no grant given, its time to give a dbru opportunity
      else // size2Assign == 0
				size2Assign = 1;
      tcontOlt->SetVariableWord(tempVariableWord - size2Assign); //this can be negative, if size2Assign > tempVariableWord
	//std::cout << "@" << nowNano << " type 2: \t" << 4*size2Assign << " Bytes" << std::endl;
    }
  }

  //for T3, if in first round (GIR), then only the min(request, allocationWordsBasedOnABmin) is given. When in second round, then min(request, allocationWordsBasedOnABsur) is given. GIR:PIR = 0.4. This ratio converts to 0.2 as below due to SI_PIR being twice that of SI_GIR
  //TODO: T3 should have two SI for GIR and PIR respectively, triggering allocation two times than that for T1/T2. However, since only one SI is used for now, reset is only done at second round. However, this has to be considered for ideal EBU implementation
  else if ((tcontOlt->GetTcontType()) == XgponQosParameters::XGPON_TCONT_TYPE_3)
  {
    if (m_t3FirstRound == true)
    {
      if (tempVariableWord >= 0)
      {
				size2Assign = tcontOlt->CalculateRemainingDataToServe(GetRtt(), GetFrameSlotSize());
				if(size2Assign>0)
				{
					if (size2Assign<4)
						size2Assign =4;
					if (size2Assign > 0.2*(tcontOlt->GetAllocationWords())) 
						size2Assign = 0.2*(tcontOlt->GetAllocationWords());
					if(!CheckServedTcont(tcontOlt->GetAllocId()))
						size2Assign+=1;
				}
				else
					size2Assign = 0;
				tcontOlt->SetVariableWord(tempVariableWord - size2Assign);
				//std::cout << "@" << nowNano << " type 3 GIR: \t" << 4*size2Assign << " Bytes" << std::endl;
      }
      //no need to give a dbru opportunity to t3 since its given at PIR expiry
    }

    else
    {
      if (tempVariableWord >= 0)
      {
				size2Assign = tcontOlt->CalculateRemainingDataToServe(GetRtt(), GetFrameSlotSize());
				if(size2Assign>0)
				{
					if (size2Assign<4)
						size2Assign =4;
					if (size2Assign > 0.6*(tcontOlt->GetAllocationWords())) 
						size2Assign = 0.6*(tcontOlt->GetAllocationWords());
					if(!CheckServedTcont(tcontOlt->GetAllocId()))
						size2Assign+=1;
				}
				else // size2Assign == 0
					size2Assign = 1;
				tcontOlt->SetVariableWord(tempVariableWord - size2Assign);
				//std::cout << "@" << nowNano << " type 3 PIR: \t" << 4*size2Assign << " Bytes" << std::endl;
      }
    }
  }

  else //(*m_tcontIterator)->GetTcontType() == XgponQosParameters::XGPON_TCONT_TYPE_4
  {
    NS_ASSERT_MSG( ((*m_tcontIterator)->GetTcontType() == XgponQosParameters::XGPON_TCONT_TYPE_4), "Invalid Tcont Type detected at allocating grants");

    if (m_t3FirstRound == true)
    {
      size2Assign = 0;    
      tcontOlt->SetVariableWord(tempVariableWord - size2Assign);
    }
    else // m_t3FirstRound == false
    {
      if (tempVariableWord >= 0)
      {
				size2Assign = tcontOlt->CalculateRemainingDataToServe(GetRtt(), GetFrameSlotSize());
				if(size2Assign>0)
				{
					if (size2Assign<4)
						size2Assign =4;
					if (size2Assign > tcontOlt->GetAllocationWords()) 
						size2Assign = tcontOlt->GetAllocationWords();
					if(!CheckServedTcont(tcontOlt->GetAllocId()))
						size2Assign+=1; 
				}
				else // size2Assign == 0
					size2Assign = 1;
				tcontOlt->SetVariableWord(tempVariableWord - size2Assign);
				//std::cout << "@" << nowNano << " type 4: \t" << 4*size2Assign << " Bytes" << std::endl ;
      }
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
