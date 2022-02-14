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

#ifndef XGPON_ONU_CONN_MANAGER_H
#define XGPON_ONU_CONN_MANAGER_H

#include "ns3/address.h"

#include "xgpon-onu-engine.h"

#include "xgpon-tcont-onu.h"
#include "xgpon-connection-receiver.h"
#include "xgpon-connection-sender.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The class used to manage the downstream and upstream connections (T-CONT) at ONU side.
 *        It's one abstract class and there are subclasses designed for flexibility or speed.
 */
class XgponOnuConnManager : public XgponOnuEngine
{
public:

  /**
   * \brief Constructor
   */
  XgponOnuConnManager ();
  virtual ~XgponOnuConnManager ();



  /**
   * \brief Add one XgponTcontOnu to this ONU
   */
  virtual void AddOneUsTcont (const Ptr<XgponTcontOnu>& tcont)=0;

  /**
   * \brief get the tcont-onu based on alloc-id. 0: no found
   */
  virtual const Ptr<XgponTcontOnu>& GetTcontById (uint16_t allocId)=0;



  /**
   * \brief Add one upstream connection to this ONU
   * \param alloId the id of the T-CONT that this connection belongs to
   */
  virtual void AddOneUsConn (const Ptr<XgponConnectionSender>& conn, uint16_t allocId)=0;

  /**
   * \brief find one upstream connection based on the source address of the packet to be transmitted.  0: not found
   */
  virtual const Ptr<XgponConnectionSender>& FindUsConnByAddress (const Address& addr)=0;

  /**
   * \brief find the upstream connection for OMCI traffic.  0: not found
   */
  const Ptr<XgponConnectionSender>& FindUsOmciConn ();

  /**
   * \brief jerome, X2, find one upstream connection based on the tcont Type.  0: not found
   */
	virtual const Ptr<XgponConnectionSender>& FindUsConnByTcontType (const uint16_t& type) = 0;





  /**
   * \brief Add one downstream connection belonging to this ONU
   */
  virtual void AddOneDsConn (const Ptr<XgponConnectionReceiver>& conn)=0;


  /**
   * \brief find one downstream connection based on XGEM port-id. 0: not found
   */
  virtual const Ptr<XgponConnectionReceiver>& FindDsConnByXgemPort (uint16_t port)=0;


  /**
   * \brief set the length of ip netmask of the sub-network controlled by this ONU
   */
  void SetOnuNetmaskLen (uint8_t len) { m_onuNetmaskLen = len; }


  ///////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;



protected:
  uint8_t m_onuNetmaskLen;                                 //Length of Netmask of the network controlled by this ONU.

  Ptr<XgponConnectionSender> m_usOmciConn;                 //the connection for upstream OMCI traffic.

  //to return one null conn or tcont
  Ptr<XgponConnectionSender> m_nullConnSender;
  Ptr<XgponConnectionReceiver> m_nullConnReceiver;
  Ptr<XgponTcontOnu> m_nullTcont;



};





///////////////////////////////////////////////INLINE Functions
inline const Ptr<XgponConnectionSender>& 
XgponOnuConnManager::FindUsOmciConn ()
{
  return m_usOmciConn;
}


}; // namespace ns3

#endif // XGPON_ONU_CONN_MANAGER_H
