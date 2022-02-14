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

#ifndef XGPON_TCONT_ONU_H
#define XGPON_TCONT_ONU_H

#include "xgpon-tcont.h"

#include "xgpon-connection-sender.h"
#include "xgpon-xgtc-dbru.h"
#include "xgpon-xgtc-bwmap.h"


namespace ns3 {

class XgponOnuUsScheduler;


/**
 * \ingroup xgpon
 * \brief This class is used at ONU-side. It contains the list of (sender)connections that belongs to the Alloc-ID. 
 *        There is also one per-ALLOC-ID upstream scheduler.
 *
 */
class XgponTcontOnu : public XgponTcont
{

public:

  /**
   * \brief Constructor
   */
  XgponTcontOnu ();
  virtual ~XgponTcontOnu ();



  /////////////////////////////////////////////////////////MAIN Interfaces

  //called when buffer occupancy report need to be sent
  //the generated report is also put into the history list.
  Ptr<XgponXgtcDbru> PrepareBufOccupancyReport ( );


  //called when one bandwidth allocation is received from OLT. time: receiving time
  void ReceiveBwAllocation (const Ptr<XgponXgtcBwAllocation>& allocation, uint64_t time);
  
  /**
   * \brief  add one connection into the alloc-id. 
   * \param  conn the connection to be added.
   */
  void AddOneConnection (const Ptr<XgponConnectionSender>& conn);

  /////////////////////////////////INLINE Functions
  /**
   * \brief  get connection based on index
   * \return the connection whose index in the vector equals to the parameter
   * \param  index the index of the connection to be returned
   */
  const Ptr<XgponConnectionSender>& GetConnByIndex (uint32_t index) const;   

  //return the number of connections in this alloc
  uint32_t GetConnNumber ( ) const;  

  ////////////////////////////////////////////////////////Scheduler accessor
  const Ptr<XgponOnuUsScheduler>& GetOnuUsScheduler ( ) const; 
  void SetOnuUsScheduler (const Ptr<XgponOnuUsScheduler>& ussScheduler);

  ///////////////////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;



private:
  //Connections of the same alloc-id. They should have the same T-CON type
  //We assume that there are a very few number connections in each ALLOC-ID.
  //Thus, a vector is used here. and the connection will be searched sequentially.
  std::vector< Ptr<XgponConnectionSender> > m_connections;   


  Ptr<XgponOnuUsScheduler> m_usScheduler;   //scheduler for connections belong to the same ALLOC-ID.


  //////////////////////////////////////////////////////Clear history
  //remove based on create_time
  virtual void ClearOldBufOccupancyReports (uint64_t time);

  //remove based on receive_time
  virtual void ClearOldBandwidthAllocations (uint64_t time);

};




//////////////////////////////////////////////////////////INLINE Functions
inline const Ptr<XgponConnectionSender>& 
XgponTcontOnu::GetConnByIndex (uint32_t index) const
{
  NS_ASSERT_MSG((index < m_connections.size()), "Index is too large!!!");
  return m_connections[index];
}

inline uint32_t 
XgponTcontOnu::GetConnNumber ( ) const
{
  return m_connections.size();
}



}; // namespace ns3



#endif // XGPON_TCONT_ONU_H
