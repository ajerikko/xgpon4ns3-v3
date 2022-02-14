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

#ifndef XGPON_CONNECTION_H
#define XGPON_CONNECTION_H

#include "ns3/object.h"
#include "ns3/address.h"


namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The class used to represent a connection in XG-PON. 
 *        <XGEM-Port, direction> is the identifier since we assume that the same xgem-port number can be used in both directions. 
 *        Except the IDs, it also holds the corresponding QoS parameters.
 */
class XgponConnection : public Object
{
public:

  static const uint8_t UPSTREAM_CONN = 0;
  static const uint8_t DOWNSTREAM_CONN = 1;


  /**
   * \brief Constructor
   */
  XgponConnection ();
  virtual ~XgponConnection ();


  //////////////////////////////////////Member variable accessors (inline functions)
  void SetDirection (uint8_t direction);
  uint8_t GetDirection () const;

  void SetBroadcast (bool broadcast);
  bool IsBroadcast () const;

  void SetXgemPort (uint16_t port);
  uint16_t GetXgemPort () const;

  void SetAllocId (uint16_t allocId);
  uint16_t GetAllocId () const;

  void SetOnuId (uint16_t onuId);
  uint16_t GetOnuId () const;

  //below are not inline functions since we may consider multiple address types in the future.
  void SetUpperLayerAddr (const Address& addr);
  const Address& GetUpperLayerAddr () const;


  /////////////////////////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;


private:
  uint8_t    m_direction;       //the direction of data flow: upstream or downstream. 
  bool       m_broadcast;       //true: broadcast; false: unicast
  uint16_t   m_xgemPort;        //XGEM port.
  uint16_t   m_allocId;         //Alloc ID of the T-CONT. Used by only upstream connections;  Len: 14bits
  uint16_t   m_onuId;           //ONU ID; Len: 10bits

  Address    m_upperLayerAddr;  //upper-layer (IP) address of the host (XG-PON subscriber), for which this connection is set up.
                                //for instance, the IP address of the computer that connects to ONU.


  ////////////////////////////////////////QoS parameters of this connection.





  /* more variables may be needed */
  /* foreign onu-id, alloc-id, upper layer address may be necessary to support mobility among ONUs of the same XG-PON */

};









////////////////////////////////////////////////////////INLINE Functions
inline void
XgponConnection::SetDirection (uint8_t direction)
{
  m_direction = direction;
}
inline uint8_t 
XgponConnection::GetDirection () const
{
  return m_direction;
}

inline void 
XgponConnection::SetBroadcast (bool broadcast)
{
  m_broadcast = broadcast;
}
inline bool 
XgponConnection::IsBroadcast () const
{
  return m_broadcast;
}

inline void 
XgponConnection::SetXgemPort (uint16_t port)
{
  m_xgemPort = port;
}
inline uint16_t
XgponConnection::GetXgemPort () const
{
  return m_xgemPort;
}

inline void 
XgponConnection::SetAllocId (uint16_t allocId)
{
  m_allocId = allocId;
}
inline uint16_t 
XgponConnection::GetAllocId () const
{
  return m_allocId;
}

inline void 
XgponConnection::SetOnuId (uint16_t onuId)
{
  m_onuId = onuId;
}
inline uint16_t 
XgponConnection::GetOnuId () const
{
  return m_onuId;
}







} // namespace ns3

#endif // XGPON_CONNECTION_H

