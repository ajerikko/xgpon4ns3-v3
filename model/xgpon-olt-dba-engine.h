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

#ifndef XGPON_OLT_DBA_ENGINE_H
#define XGPON_OLT_DBA_ENGINE_H

#include "ns3/object.h"

#include "xgpon-olt-engine.h"
#include "xgpon-olt-dba-bursts.h"

#include "xgpon-xgtc-dbru.h"




/* Limitations on the BwMap. Note that frame size and maximal allocation/burst size depend on PHY parameters
 *
 * Size of Upstream PHY frame:  2.48832e9*125e-6= 311040 bits = 38880 bytes = 9720 words
 *
 * minimum Start Time = 0.
 * maximum Start Time = 9719. This means that a ONU burst can cross the PHY frame boundry
 * maximum number of allocations per BwMap = 512 
 * maximum number of burst allocation series per given ONU in a BwMap = 4
 * maximum number of allocation structures per burst = 16
 * maximum number of burst allocation structures per given ONU = 64
 * maximum allocation size = 9718 words
 * maximum XGTC burst size = 9720 words 
 * minimum guard time recomended between bursts = 2 words
 *
 */


namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The class used to instantiate DBA functions of XG-PON at OLT side. 
 *        It is responsible to interpret queue-status-reports from ONUs and generate BWmap. 
 *        In another word, it carries out the scheduling of alloc-ids at OLT-side.
 *        It is an abstract class and subclasses are implemented for different DBA schemes.
 *
 */
class XgponOltDbaEngine : public XgponOltEngine
{
public:

  const static uint32_t MAX_TCONT_PER_BWMAP=512;         //at most, 512 T-CONTs can be scheduled in one bwmap.

  /**
   * \brief Constructor
   */
  XgponOltDbaEngine ();
  virtual ~XgponOltDbaEngine ();



  /**
   * \brief Add T-CONT into the DBA Engine that may organize T-CONTs in different ways for upstream scheduling purpose (DBA algorithm).
   * \param tcont the T-CONT to be added to the engine
   */
  virtual void  AddTcontToDbaEngine (Ptr<XgponTcontOlt>& tcont)=0; 

  /**
   * \brief generate BWmap. Effectively, it instantiates the scheduling of upstream connections (more specifically alloc-id) at OLT-side.
   */
  const Ptr<XgponXgtcBwmap> GenerateBwMap ();



  /**
   * \brief process the buffer occupancy report from ONU.
   * \param report the report to be processed
   * \param onuId the ID of the source ONU
   * \param allocId the T-CONT that this report belongs to
   */
  void ReceiveStatusReport (const Ptr<XgponXgtcDbru>& report, uint16_t onuId, uint16_t allocId);  






  //////////////////////////////Maintaining the BWmap list (whose bursts have not been received yet) at OLT
  /**
   * \brief Get the burst profile used by a upstream burst through looking up the BW_maps based on the receiving time. 
   * \param time the time that the upstream burst is received.
   * \return the profile used by the upstream burst.
   */
  const Ptr<XgponBurstProfile>& GetProfile4BurstFromChannel(uint64_t time);


  /**
   * \brief Get the corresponding BW_MAP in which the just received burst is allocated.
   * \return the corresponding BW_MAP
   * \param time the time that the upstream burst is received.
   */
  const Ptr<XgponXgtcBwmap>& GetBwMap4CurrentBurst (uint64_t time);


  /**
   * \brief Get the index of the first bandwidth allocation that belongs to the current upstream burst.
   * \return the index of the first bandwidth allocation
   * \param bwmap the BW_MAP in which the upstream burst is scheduled.
   * \param time the time that the upstream burst is received.
   */  
  uint32_t GetIndexOfBurstFirstBwAllocation (const Ptr<XgponXgtcBwmap>& bwmap, uint64_t time);




  //for debugging
  void PrintAllActiveBwmaps (void);




  //////////////////////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;



protected:
  /**
   * \brief return the sum of rtt and downstream frame slot size.  Unit: nanosecond
   */
  uint32_t GetRtt ();

  /**
   * \brief return downstream frame slot size. Unit: nanosecond
   */
  uint32_t GetFrameSlotSize ();

  /**
   * \brief return the data rate in upstream direction. Unit: bytes per second
   */
  uint32_t GetUsLinkRate ();


  /**
   * \brief return the bytes allocated in the current frame from the last BwMap. Used mainly for error checking. Unit: words
   */
  uint16_t GetExtraInLastBwmap () const;

  /**
   * \brief Check if a particular T-CONT has been served in this BwMap cycle.
   * \return True if the T-CONT has been served in this BwMap cycle, false if not.
   * \param AllocId of the T-CONT to check
   */
  bool CheckServedTcont(uint64_t allocId);

  /**
   * \brief Check if a particular T-CONT has been served in this BwMap cycle.
   * \return True if the T-CONT has been served in this BwMap cycle, false if not.
   * \param AllocId of the T-CONT to check
   */
  void SetServedTcont(uint64_t allocId);


private:

  /**
   * \brief return the next T-CONT to be considered. Note that the scheduled TcontType and TcontIndex are updated.
   * \return the T-CONT to be considered. 0 means that there is no T-CONT in this XG-PON network (unlikely!!!).
   */
  virtual const Ptr<XgponTcontOlt>& GetNextTcontOlt ( ) = 0; 


  /**
   * \brief return the T-CONT pointed by the current cursor
   */
  virtual const Ptr<XgponTcontOlt>& GetCurrentTcontOlt ( ) = 0;

  /**
   * \brief return the first T-CONT in a DBA cycle
   */
  virtual const Ptr<XgponTcontOlt>& GetFirstTcontOlt ( ) = 0;


  /**
   * \brief Calculate the amount of data to be sent for the T-CONT. unit: word
   */
  virtual uint32_t CalculateAmountData2Upload (const Ptr<XgponTcontOlt>& tcontOlt, uint32_t allocatedSize,uint64_t nowNano) = 0;

  /*
   * \brief This functions is used to check if all T-CONTs in the DBA have been served in this cycle.
   * The DBA engine can break the cycle this way, setting up any state variable it desires.
   */
  virtual bool CheckAllTcontsServed ( ) = 0;

  /*
   * \brief Called before producing one Bwmap with aim of adjusting the next alloc-olt to be served.
   *  For round robin, nothing to be done. In other dba algorithms, olt should first check the T-CONT with the highest priority.
   */
  virtual void Prepare2ProduceBwmap ( ) = 0;

  /*
   * \brief Called after producing the BwMap so that the scheduler can do some clean up and update state variables.
   * E.g. the scheduler might want to override this function to update some timers.
   */
  virtual void FinalizeBwmapProduction () = 0;

protected:
  XgponOltDbaBursts m_bursts;      //bursts used to produce BWMAP
  //jerome, C1
  uint32_t m_aggregateAllocatedSize;    //Used to maintain the total allocation for the minimum no of cycles. Tcont cycle is reset once this exceeded.

private:
  //the list of BW-MAPs that have been sent out, but the corresponding bursts have not been received yet.
  std::list< Ptr<XgponXgtcBwmap> > m_servedBwmaps;  
  Ptr<XgponXgtcBwmap> m_nullBwmap;  //used to return a null bwmap.

  uint16_t m_extraInLastBwmap;     //BWMAP may cross the boundary of frame and this variable is used to maintail the over-allocation. unit: word;

  //calculate once to save CPU.
  uint32_t m_dsFrameSlotSizeInNano; //unit: nanosecond
  uint32_t m_logicRtt;  //unit: nanosecond
  uint32_t m_usRate; //unit: Byte per second


  /* more variables may be needed */  
};

inline uint16_t
XgponOltDbaEngine::GetExtraInLastBwmap ( ) const
{
  return m_extraInLastBwmap;
}


}; // namespace ns3

#endif // XGPON_OLT_DBA_ENGINE_H
