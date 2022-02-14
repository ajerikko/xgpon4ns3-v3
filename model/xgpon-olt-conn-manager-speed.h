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

#ifndef XGPON_OLT_CONN_MANAGER_SPEED_H
#define XGPON_OLT_CONN_MANAGER_SPEED_H

#include "xgpon-olt-conn-manager.h"


namespace ns3 {


/**
 * \ingroup xgpon
 * \brief The class instantiates XgponOltConnManager. It imposees some constraint between address and onuId/XgemPort so that,
 *        the corresponding xgem-port can be calculated from address, and get the connection in O(1) time. 
 *        It is optimized for uni-cast traffics.
 *        More specifically, DS XgemPort values: onuId + (0...63) * 1024; IP address:  onuId + other bits + 6bits
 */
class XgponOltConnManagerSpeed : public XgponOltConnManager
{
public:

  /**
   * \brief Constructor
   */
  XgponOltConnManagerSpeed ();
  virtual ~XgponOltConnManagerSpeed ();



  /**
   * \brief Add one downstream connection
   */
  virtual void AddOneDsConn (const Ptr<XgponConnectionSender>& conn, bool isBroadcast, uint16_t onuId);

  /**
   * \brief Find one downstream connection based on the destination address of the packet to be transmitted through xgpon. 
   * \return the corresponding XgponConnectionSender; 0: no found
   */
  virtual const Ptr<XgponConnectionSender>& FindDsConnByAddress (const Address& addr);

  /**
   * \brief Find one broadcast downstream connection based on the destination address of the packet to be broadcasted through xgpon. 
   * \return the corresponding XgponConnectionSender; 0: no found
   */
  virtual const Ptr<XgponConnectionSender>& FindBroadcastConnByAddress (const Address& addr);




  ////////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

private:

  //used when mapping ip address of the upper-layer packet to the corresponding connection (for broadcast connections only).
  //As for uni-cast connection, we calculate XgemPort from IP address directly.
  //key = address, value = XgponConnectionSender; Performance: O(log n);
  std::map< Address, Ptr<XgponConnectionSender> > m_broadcastConnsAddressIndex;    
  
  uint16_t CalculateXgemPortFromAddress (const Address& addr);

};

}; // namespace ns3

#endif // XGPON_OLT_CONN_MANAGER_SPEED_H
