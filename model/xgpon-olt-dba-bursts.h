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

#ifndef XGPON_OLT_DBA_BURSTS_H
#define XGPON_OLT_DBA_BURSTS_H

#include <list>

#include "xgpon-olt-dba-per-burst-info.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The class used to maintain the bursts for producing BWMAP. Note that one onu may have multiple bursts.
 */
class XgponOltDbaBursts
{
  const static uint32_t MAX_TCONT_PER_ONU=64;            //at most, 64 T-CONTs of one ONU can be scheduled in the bwmap, i.e., 4 bursts.

public:

  /**
   * \brief Constructor
   */
  XgponOltDbaBursts ();
  virtual ~XgponOltDbaBursts ();


  /**
   * \brief clear the list of perBurstInfo for producing a new BWMAP.
   */
  void ClearBurstInfoList( );

  /**
    * \brief Check if a new burst is needed to serve a T-CONT. This is useful to check how much size is left to assigne in a
    * upstream phy frame.
    */
  bool IsNewBurstNecessary(const Ptr<XgponTcontOlt>& tcont);

  /**
   * \brief return the perOnuInfo in which the tcont will be scheduled. 
   *        0 means that this ONU cannot be scheduled in this bwmap anymore (too many bursts for one onu).
   */
  const Ptr<XgponOltDbaPerBurstInfo> GetBurstInfo4TcontOlt(const Ptr<XgponTcontOlt>& tcont);



  /**
   * \brief produce the final bwmap based on the info of the burst list. the bwmap is also stored for receiving the bursts in the future.
   * \return the generated BWmap
   * \param now the time that this BWmap is created
   * \param extraAllocationInLastBwmap the amount of over-allocation in the last bwmap.
   * \param usFrameSize the size of upstream frame.
   */
  const Ptr<XgponXgtcBwmap> ProduceBwmapFromBursts(uint64_t now, uint16_t extraAllocationInLastBwmap, uint16_t usFrameSize);


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
  std::list< Ptr<XgponOltDbaPerBurstInfo> > m_dbaPerBurstInfos;
  std::vector<bool> m_servedTconts;

  //used to return one null perburstinfo
  Ptr<XgponOltDbaPerBurstInfo> m_nullPerBurstInfo;
};


}; // namespace ns3

#endif // XGPON_OLT_DBA_BURSTS_H
