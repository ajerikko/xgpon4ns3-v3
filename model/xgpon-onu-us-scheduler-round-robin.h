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

#ifndef XGPON_ONU_US_SCHEDULER_ROUND_ROBIN_H
#define XGPON_ONU_US_SCHEDULER_ROUND_ROBIN_H

#include "xgpon-onu-us-scheduler.h"



namespace ns3 {



/**
 * \ingroup xgpon
 * \brief This class is a simple sub-class of XgponOnuUsScheduler. 
 *        For status report, it just returns the sum of the amount of data in each connection's queue.
 *        For upstream scheduling, a simple round-robin scheme is implemented. 
 *
 */
class XgponOnuUsSchedulerRoundRobin : public XgponOnuUsScheduler
{

public:

  const static uint32_t XGPON1_US_PER_SERVICE_MAX_SIZE = 40000;  //40KBytes; 
  //Note that it is larger than the whole upstream frame to allow one upstream connection to fully utilize the whole network.

  /**
   * \brief Constructor
   */
  XgponOnuUsSchedulerRoundRobin ();
  virtual ~XgponOnuUsSchedulerRoundRobin ();


  //////////////////////////////////////////////////////////////////Main Interfaces

  /**
   * \brief  Get the connection whose packets will be sent in this upstream burst. 
   *         It implements the scheduling for upstream connections that belong to the same T-CONT.
   *         Round-Robin is implemented in this class.
   * \return the connection to be served. 0: all connections have no data to send.  (unit: byte)
   * \param  amountToServe used to return the amount of data to be transmitted for this connection. 
   */  
  virtual const Ptr<XgponConnectionSender>  SelectConnToServe (uint32_t* amountToServe);


  

  ///////////////////////////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;



private:

  uint32_t m_maxServiceSize;  //configured through attribute

  //These XgponConnectionSender of the same T-CONT should have the same T-con type and be served according to round robin.
  //Other subclass may consider their QoS parameters.
  uint16_t m_lastServedConnIndex;                            //The index of the last served connection. 
                                                             

};




}; // namespace ns3



#endif // XGPON_ONU_US_SCHEDULER_ROUND_ROBIN_H
