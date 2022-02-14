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

#ifndef XGPON_OLT_DS_SCHEDULER_ROUND_ROBIN_H
#define XGPON_OLT_DS_SCHEDULER_ROUND_ROBIN_H

#include <vector>

#include "xgpon-olt-ds-scheduler.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The class used to schedule the downstream connections at OLT side in a round-robin manner. 
 */
class XgponOltDsSchedulerRoundRobin : public XgponOltDsScheduler
{
public:

  const static uint32_t XGPON1_DS_PER_SERVICE_MAX_SIZE = 155520;  //larger than the whole downstream frame so that 
                                                                  //one xgem-port can fully utilize the whole network.

  /**
   * \brief Constructor
   */
  XgponOltDsSchedulerRoundRobin ();
  virtual ~XgponOltDsSchedulerRoundRobin ();



  /**
   * \brief  Get the connection whose packets will be sent in this downstream frame. It implements the scheduling for downstream connections.
   * \return the connection to be served. 0: all connections have no data to send.
   * \param  amountToServe used to return the amount of data to be transmitted for this connection. 
   */  
  virtual const Ptr<XgponConnectionSender>  SelectConnToServe (uint32_t* amountToServe);
  

  /**
   * \brief Adds a connection to the scheduler. The scheduler may re-organize these connections to facilitate downstream scheduling.
   * \param conn the downstream connection to be scheduled
   * \return void
   */  
  virtual void AddConnToScheduler (const Ptr<XgponConnectionSender>& conn);   




  //////////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;


private:

  //Get the last connection served in the last downstream connection.
  Ptr<XgponConnectionSender> GetTheLastServedConnection () const;  

  //Get the next connection to be considered for serving in this downstream frame.
  Ptr<XgponConnectionSender> GetNextConnection2Serve ();  

private:
  uint32_t m_maxServiceSize;       //maximal served size, configured through attribute 



  //used when selecting the next connection
  uint16_t m_lastServedConnIndex;   //the index in this list for the connection that are served most recently.
  std::vector< Ptr<XgponConnectionSender> > m_dsAllConns;

};






///////////////////////////////////////////////////////INLINE Functions
inline Ptr<XgponConnectionSender> 
XgponOltDsSchedulerRoundRobin::GetTheLastServedConnection () const
{
  return m_dsAllConns[m_lastServedConnIndex];
}


inline Ptr<XgponConnectionSender> 
XgponOltDsSchedulerRoundRobin::GetNextConnection2Serve ()
{
  m_lastServedConnIndex++;
  if(m_lastServedConnIndex >= m_dsAllConns.size()) m_lastServedConnIndex = 0;

  return m_dsAllConns[m_lastServedConnIndex];
}


}; // namespace ns3

#endif // XGPON_OLT_DS_SCHEDULER_ROUND_ROBIN_H
