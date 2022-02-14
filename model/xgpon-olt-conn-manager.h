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

#ifndef XGPON_OLT_CONN_MANAGER_H
#define XGPON_OLT_CONN_MANAGER_H

#include "ns3/address.h"

#include "xgpon-olt-engine.h"
#include "xgpon-olt-conn-per-onu.h"
#include "xgpon-connection-sender.h"
#include "xgpon-connection-receiver.h"
#include "xgpon-tcont-olt.h"
#include "xgpon-qos-parameters.h"


namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The class used to manage ONUs' downstream and upstream connections at OLT side. 
 *        It's one abstract class and several subclasses are implemented for flexibility or speed. 
 *
 */
class XgponOltConnManager : public XgponOltEngine
{
public:

  /**
   * \brief Constructor
   */
  XgponOltConnManager ();
  virtual ~XgponOltConnManager ();




  ///////////////////////////////////////////////////////////Main functions
  /**
   * \brief Add (find) one per-ONU structure; for management and extention
   */
  void AddOneOnu4Conns (const Ptr<XgponOltConnPerOnu>& onu4Conns);
  const Ptr<XgponOltConnPerOnu>& GetOneOnu4ConnsById (uint16_t onuId) const;




  /**
   * \brief Add one upstream TcontOlt for the ONU
   */
  void AddOneUsTcont (const Ptr<XgponTcontOlt>& tcont, uint16_t onuId);
  
  /**
   * \brief Find TcontOlt based on Alloc-ID (inline function). 0: no found
   */
  const Ptr<XgponTcontOlt>& GetTcontById (uint16_t allocId) const;

  /* jerome, Apr 9
   * \brief Find TcontOltType based on Alloc-ID (inline function). 0: unknown type
   */
  const XgponQosParameters::XgponTcontType GetTcontTypeById(uint16_t allocId) const;


  /**
   * \brief Add one upstream connection based on the related IDs
   *        It is used to enable XgponTcontOlt to calculate the aggregated QoS parameters. 
   *        It might also be used for management and further extension.
   */
  void AddOneUsConn (const Ptr<XgponConnectionReceiver>& conn, uint16_t allocId);




  /**
   * \brief Add one downstream connection
   */
  virtual void AddOneDsConn (const Ptr<XgponConnectionSender>& conn, bool isBroadcast, uint16_t onuId)=0;

  /**
   * \brief Find one downstream connection based on the destination address of the packet to be transmitted through xgpon. 
   * \return the corresponding XgponConnectionSender; 0: no found
   */
  virtual const Ptr<XgponConnectionSender>& FindDsConnByAddress (const Address& addr)=0; 

  /**
   * \brief Find one broadcast downstream connection based on the destination address of the packet to be broadcasted through xgpon. 
   * \return the corresponding XgponConnectionSender; 0: no found
   */
  virtual const Ptr<XgponConnectionSender>& FindBroadcastConnByAddress (const Address& addr) = 0;


  /**
   * \brief Find the downstream omci connection based on onu-id (inline function). 
   * \return the corresponding XgponConnectionSender; 0: no found
   */
  const Ptr<XgponConnectionSender>& FindDsOmciConnByOnuId (const uint16_t onuId) const ;

  /**
   * \brief Get the total number of ONUs
   */
  uint32_t GetNumberOfOnus ();

  /**
   * \brief Get the total number of T-CONTs
   */
  uint32_t GetNumberOfTconts ();


  /**
   * \brief Set the length of ip netmask for the network controlled by one onu.
   */
  void SetOnuNetmaskLen(uint8_t len);



  ////////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;




protected:
  //netmask length for the network controlled by ONUs
  uint8_t m_onuNetmaskLen;

  //used when mapping xgem-port of the low-layer xgem-frame to the corresponding connection (for per-onu connections).
  //In XgponOltConnManagerSpeed, it is also used to map ip address of the upper-layer packet to the corresponding connection (for per-onu connections).
  std::vector< Ptr<XgponConnectionSender> > m_dsConnsPortIndex;             //index == xgemPort;


  void AddOneBroadcastDsConnection (const Ptr<XgponConnectionSender>& conn);
  std::vector< Ptr<XgponConnectionSender> >& GetAllBroadcastDsConnections ();  

private:

  //////////////////////////////////Note that m_broadcastConns and m_onus are used for management purpose only.
  //used to maintain the connections for traffics broadcasted to all ONUs.
  std::vector< Ptr<XgponConnectionSender> > m_broadcastConns;  
  //ONU information and the downstream and upstream connections of the ONU are organized into XgponOltConnPerOnu.
  std::vector< Ptr<XgponOltConnPerOnu> > m_onus; 





  ///////used to find T-CONT, and their types quickly. CPU vs memory tradeoff. index == alloc-id
  std::vector< Ptr<XgponTcontOlt> > m_tconts;
  std::vector< XgponQosParameters::XgponTcontType > m_tcontsType; //jerome, Apr 9

  /* 
   * For scheduling the upstream connections, these Alloc-IDs should be organized according to their priorities.
   * For downstream connections, the similar state variables are also necessary.
   * With these list, it's much easier and faster to do scheduling.
   * Since we just put one pointer in the following lists, the memory overhead should be fine.
   * Hence, these Alloc-IDs and downstream connections are also maintained in XgponOltDbaEngine and XgponOltDsScheduler
   * according to their TCON type.
   */


  /* more variables may be needed */

};







///////////////////////////////////////////////INLINE Functions
inline void 
XgponOltConnManager::SetOnuNetmaskLen(uint8_t len)
{ 
  m_onuNetmaskLen = len; 
}


inline const Ptr<XgponTcontOlt>& 
XgponOltConnManager::GetTcontById (uint16_t allocId) const 
{
  NS_ASSERT_MSG((allocId<16384), "Alloc-ID is too large (unlawful)!!!");
  return m_tconts[allocId];
}

//jerome, Apr 9
inline const XgponQosParameters::XgponTcontType
XgponOltConnManager::GetTcontTypeById(uint16_t allocId) const
{
  NS_ASSERT_MSG((allocId<16384), "Alloc-ID is too large (unlawful)!!!");
  return m_tcontsType[allocId];
}


inline const Ptr<XgponConnectionSender>& 
XgponOltConnManager::FindDsOmciConnByOnuId (const uint16_t onuId) const 
{
  NS_ASSERT_MSG((onuId<1023), "ONU-ID is too large (unlawful)!!!");
  return m_dsConnsPortIndex[onuId];
}



inline void 
XgponOltConnManager::AddOneBroadcastDsConnection (const Ptr<XgponConnectionSender>& conn)
{
  m_broadcastConns.push_back(conn);
}
inline std::vector< Ptr<XgponConnectionSender> >& 
XgponOltConnManager::GetAllBroadcastDsConnections ()
{
  return m_broadcastConns;
}



}; // namespace ns3

#endif // XGPON_OLT_CONN_MANAGER_H
