/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c)  2012 The Provost, Fellows and Scholars of the 
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
 * Author: Pedro Alvarez <pinheirp@tcd.i>
 */

#ifndef XGPON_OLT_DBA_ENGINE_ROUND_ROBIN_H
#define XGPON_OLT_DBA_ENGINE_ROUND_ROBIN_H

#include "xgpon-olt-dba-engine.h"




namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The class used to instantiate DBA functions of XG-PON at OLT side. 
 *        It is responsible to interpret queue-status-reports from ONUs and generate BWmap. 
 *        In another word, it carries out the scheduling of T-CONTs at OLT-side.
 *        A simple round-robin scheme is implemented in this sub-class.
 *
 */
class XgponOltDbaEngineRoundRobin : public XgponOltDbaEngine
{
public:
  const static uint32_t XGPON1_TCONT_PER_SERVICE_MAX_SIZE=9718;    //9718K words (around 40Kbytes to let one flow to fuly utilize the whole network). 
  const static uint32_t XGPON1_MAX_POLLING_INTERVAL=2000000;       //2ms. Unit: nanosecond

  /**
   * \brief Constructor
   */
  XgponOltDbaEngineRoundRobin ();
  virtual ~XgponOltDbaEngineRoundRobin ();




  /**
   * \brief Add Alloc-Id info into the DBA Engine
   * \param the Alloc-Id to be added to the engine
   */
  virtual void  AddTcontToDbaEngine (Ptr<XgponTcontOlt>& tcont); 



  ///////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

private:

  /**
   * \brief return the next T-CONT to be considered. Note that the scheduled TcontType and TcontIndex are updated.
   * \return the T-CONT to be considered. 0 means that there is no T-CONT in this XG-PON network (unlikely!!!).
   */
  virtual const Ptr<XgponTcontOlt>& GetNextTcontOlt ( ); 

  /**
    * \brief return the next T-CONT to be considered. Note that the scheduled TcontType and TcontIndex are updated.
    * \return the T-CONT to be considered. 0 means that there is no T-CONT in this XG-PON network (unlikely!!!).
    */
  virtual const Ptr<XgponTcontOlt>& GetFirstTcontOlt ( );

  /**
   * \brief return the T-CONT pointed by the current cursor
   */
  virtual const Ptr<XgponTcontOlt>& GetCurrentTcontOlt ( );


  /**
   * \brief Calculate the amount of data to be sent for the T-CONT.  unit: word
   */
  virtual uint32_t CalculateAmountData2Upload (const Ptr<XgponTcontOlt>& tcontOlt, uint32_t allocatedSize,uint64_t nowNano);

  /**
    * \brief Compare the first T-CONt .
    * \return true if all T-CONTs were served, false otherwise
    */
  virtual bool CheckAllTcontsServed();


  //Called before producing one Bwmap with aim of adjusting the next alloc-olt to be served.
  //For round robin, nothing to be done. In other dba algorithms, olt should first check the T-CONT with the highest priority.
  virtual void Prepare2ProduceBwmap ( );

  //Called before producing one Bwmap with aim of adjusting the next alloc-olt to be served.
  //For round robin, nothing to be done. In other dba algorithms, olt should first check the T-CONT with the highest priority.
  virtual void FinalizeBwmapProduction ();




private:
  uint32_t m_maxPollingInterval;
  uint32_t m_maxServiceSize;


  uint16_t m_lastScheduledTcontIndex;               //the index in this tcont-type list that are scheduled most recently.
  std::vector< Ptr<XgponTcontOlt> > m_usAllTconts;
  
  Ptr<XgponTcontOlt> m_firstTcontOlt;
  bool m_getNextTcontAtBeginning;  //whether to get the next T-CONT at the beginning of bwmap generation

};

}; // namespace ns3

#endif // XGPON_OLT_DBA_ENGINE_ROUND_ROBIN_H
