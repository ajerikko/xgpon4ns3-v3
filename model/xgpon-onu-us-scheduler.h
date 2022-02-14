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

#ifndef XGPON_ONU_US_SCHEDULER_H
#define XGPON_ONU_US_SCHEDULER_H

#include "xgpon-tcont-onu.h"

#include "xgpon-connection-sender.h"
#include "xgpon-xgtc-dbru.h"
#include "xgpon-xgtc-bw-allocation.h"
#include "xgpon-qos-parameters.h" //jerome, C1

namespace ns3 {



/**
 * \ingroup xgpon
 * \brief This class is responsible to carry out upstream scheduling (among connections of the same T-CONT).
 *        Each T-CONT at ONU side should have one object of this class. 
 *        It's one abstract class and sub-classes are implemented for different scheduling schemes.
 *
 */
class XgponOnuUsScheduler : public Object
{

public:

  /**
   * \brief Constructor
   */
  XgponOnuUsScheduler ();
  virtual ~XgponOnuUsScheduler ();


  //////////////////////////////////////////Main Interfaces 

  /**
   * \brief  Get the connection whose packets will be sent in this upstream burst. 
   *         It implements the scheduling for upstream connections that belong to the same T-CONT.
   * \return the connection to be served. 0: all connections have no data to send. (unit: byte)
   * \param  amountToServe used to return the amount of data to be transmitted for this connection. must be 4-bytes aligned.
   */  
  virtual const Ptr<XgponConnectionSender>  SelectConnToServe (uint32_t* amountToServe)=0;





  ///////////////////////////////////////////////////////AllocOnu accessor
  const Ptr<XgponTcontOnu>& GetTcontOnu ( ) const; 
  void SetTcontOnu (const Ptr<XgponTcontOnu>& tcont);
  



  ///////////////////////////////////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;


protected:
  Ptr<XgponTcontOnu> m_tcontOnu;

  Ptr<XgponConnectionSender> m_nullConn;  //return a null conn


};




}; // namespace ns3



#endif // XGPON_ONU_US_SCHEDULER_H
