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

#ifndef XGPON_OLT_DBA_ENGINE_EBU_H_
#define XGPON_OLT_DBA_ENGINE_EBU_H_

#include "ns3/object.h"
#include "xgpon-olt-dba-engine.h"
#include "xgpon-olt-dba-per-burst-info.h"

namespace ns3 {

class XgponOltDbaEngineEbu : public XgponOltDbaEngine
{
public:
  const static uint32_t ALLOC_PER_SERVICE_MAX_SIZE=1000;    //1K words (4Kbytes). TODO: replace with one attribute
  const static uint32_t MAX_POLLING_INTERVAL=10000000;      //10ms. Unit: nanosecond
  const static uint32_t TIMER_EXPIRE_VALUE=0;               //Value at which the timer expires. Unit: frames.


  /**
   * \brief Constructor
   */
  XgponOltDbaEngineEbu ();
  virtual ~XgponOltDbaEngineEbu ();

  /**
    * \brief sets the initial value of the serviceIntervalTimer of a specific alloc
    * \param takes 3 parameters: AllocId, bandwidth type and timer start value.
    *
    */
  //void SetTimerStartValue(uint16_t allocId, XgponEbuBandwidthType type, uint32_t initialValue);


  /**jerome, C1, function not required.
    * \brief function is called in AllTConts served
    * \param
    
  void UpdateAllTimers( );
    */

  /**
      * \brief sets serviceInterval of a specific alloc
      * \param takes 3 parameters: AllocId, bandwidth type and serviceInterval.
      *
      */
  //void SetServiceInterval(uint16_t allocId, XgponEbuBandwidthType type, uint32_t serviceInterval);

  /**
   * \brief Add Alloc-Id info into the DBA Engine
   * \param the Alloc-Id to be added to the engine
   */
  virtual void  AddTcontToDbaEngine (ns3::Ptr<ns3::XgponTcontOlt>&);

  //returns m_vectorpair iterator to the beginning
  virtual void Prepare2ProduceBwmap();

  /**
   *  \breif Decrements or resets the SImin and SImax timers in the vector pairs
   */
  virtual void FinalizeBwmapProduction();

  //checks if all tconts have been served
  virtual bool CheckAllTcontsServed ();

  //Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  void DoInitialize(void);

private:

  //Get the next tcont to be served, from all the exixting tcont list
  virtual const Ptr<XgponTcontOlt>& GetNextTcontOlt ( );

  void UpdateTcontOltForNextCycle(); //jerome, C1
  virtual const Ptr<XgponTcontOlt>& GetCurrentTcontOlt ( );

  //Get the first tcont to be served, from all the existing tcont list
  virtual const Ptr<XgponTcontOlt>& GetFirstTcontOlt ( );

  //Calculate the amount of data to be sent for the AllocOlt
  virtual uint32_t CalculateAmountData2Upload (const Ptr<XgponTcontOlt>& allocOlt,uint32_t allocatedSize, uint64_t nowNano);
  
  //Get the allocation bytes using service rate and SI. return in unit:words
  uint32_t GetAllocationBytesFromRateAndServiceInterval(uint32_t rate, uint16_t si);

  /*
   * \jerome, C1, To set the minium service Interval in the entire XG-PON 
   * so that aggregated allocation can be controlled to be 
   * less than m_minimumSI*usPhyFrameSize
   */
  void SetMinimumServiceInterval(uint16_t si);

private:
  uint16_t m_lastScheduledAllocIndex;  	//the index in this alloc-type list that has been scheduled most recently
  std::vector< Ptr<XgponTcontOlt> > m_usAllTcons;
  Ptr<XgponTcontOlt> m_nullTcont;      	// Pointer used to return a null T-CONT

  //jerome, C1
  //temporary iterator which points to the next tcont to be served
  std::vector<Ptr<XgponTcontOlt> >::iterator  m_tcontIterator;
  //conditions to check if T3/T4 are already served
  bool  m_t1Served, m_t2Served, m_t3Served, m_t4Served; 
  //states to keep track of first and last served tcont indices in T3 and T4
  uint16_t  m_firstServedT1Index, m_lastServedT1Index;
  uint16_t  m_firstServedT2Index, m_lastServedT2Index;
  uint16_t  m_firstServedT3Index, m_lastServedT3Index;
  uint16_t  m_firstServedT4Index, m_lastServedT4Index;
  uint16_t  m_nextCycleTcontIndex;
  // used to check if all TCONTs are served, to break the loop in GenerateBwMap(), And to distinguish allocation of GIR/PIR in T3
  bool m_stop, m_t3FirstRound;
  //to store the total requested allocation in words, from the view of a single frame
  uint32_t m_nonBestEffortAllocationInWords, m_totalAllocationInWords;
  uint16_t m_minimumSI;
  uint16_t m_allocCycleCount; //used to keep a record of no of allocation cycles served, to update m_aggregateAllocatedSize

  int32_t m_aggregatedVW_t2, m_aggregatedVW_t3, m_aggregatedVW_t4;
};


inline void
XgponOltDbaEngineEbu::SetMinimumServiceInterval(uint16_t si)
{
  m_minimumSI = si;
}

#endif /* XGPON_OLT_DBA_ENGINE_EBU_H_ */

} // namespace ns3
