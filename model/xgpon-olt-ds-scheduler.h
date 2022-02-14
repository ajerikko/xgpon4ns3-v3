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

#ifndef XGPON_OLT_DS_SCHEDULER_H
#define XGPON_OLT_DS_SCHEDULER_H

#include "ns3/object.h"

#include "xgpon-olt-engine.h"
#include "xgpon-connection-sender.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The class used to schedule the downstream connections at OLT side. It is one abstract class.
 */
class XgponOltDsScheduler : public XgponOltEngine
{
public:

  /**
   * \brief Constructor
   */
  XgponOltDsScheduler ();
  virtual ~XgponOltDsScheduler ();




  /**
   * \brief  Get the connection whose packets will be sent in this downstream frame. It implements the scheduling for downstream connections.
   * \return the connection to be served.   0: all connections have no data to send.
   * \param  amountToServe used to return the amount of data to be transmitted for this connection. must be 4-bytes aligned.
   */  
  virtual const Ptr<XgponConnectionSender>  SelectConnToServe (uint32_t* amountToServe)=0;
  

  /**
   * \brief Adds a connection to the scheduler. The subclass may re-organize these connections in different ways for scheduling.
   * \param conn the downstream connection to be scheduled
   * \return void
   */  
  virtual void  AddConnToScheduler (const Ptr<XgponConnectionSender>& conn)=0;   


  /**
   * \brief prepare to start to generate one downstream frame
   */
  void Prepare2ProduceDsFrame ( );




  //////////////////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;


protected:
  Ptr<XgponConnectionSender> m_nullConn;  //used to return a null connection

  bool m_startFrame;  //whether it is the start of producing a downstream frame (to check the connection in segmentation mode)
 
  /* more variables may be needed */

};

}; // namespace ns3

#endif // XGPON_OLT_DS_SCHEDULER_H
