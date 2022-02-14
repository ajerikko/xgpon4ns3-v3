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

#ifndef XGPON_ONU_CONN_MANAGER_SPEED_H
#define XGPON_ONU_CONN_MANAGER_SPEED_H

#include "xgpon-onu-conn-manager.h"



namespace ns3 {


/**
 * \ingroup xgpon
 * \brief The class instantiates XgponOnuConnManager. It does impose several relationship between alloc-id, xgem-port, onu-id, ipaddress, etc.
 *        1. alloc-id values: (onu_id) + (0...15) * 1024. T-CONTs are organized by a vector whose length is 16.
 *        2. ds/us xgem-port values: onu_id + (0...63) * 1024. xgem-ports are organized by a vector whose length is 64.
 *        3. ipaddress: onuid + other bits + 6bits for identify each computer (xgem port) that belongs to this ONU.
 * onu-id: 10bits; xgem-port: 16bits; alloc-id: 14bits
 * Hence, for each ONU, the number of supported xgem-ports / t-conts must be less than 64 / 16 (should be enough for many scenarios).
 */
class XgponOnuConnManagerSpeed : public XgponOnuConnManager
{
public:

  /**
   * \brief Constructor
   */
  XgponOnuConnManagerSpeed ();
  virtual ~XgponOnuConnManagerSpeed ();



  /**
   * \brief Add one XgponTcontOnu to this ONU
   */
  virtual void AddOneUsTcont (const Ptr<XgponTcontOnu>& tcont);

  /**
   * \brief get the tcont-onu based on alloc-id. 0: no found
   */
  virtual const Ptr<XgponTcontOnu>& GetTcontById (uint16_t allocId);



  /**
   * \brief Add one upstream connection to this ONU
   * \param alloId the id of the T-CONT that this connection belongs to
   */
  virtual void AddOneUsConn (const Ptr<XgponConnectionSender>& conn, uint16_t allocId);

  /**
   * \brief find one upstream connection based on the source address of the packet to be transmitted.  0: not found
   */
  virtual const Ptr<XgponConnectionSender>& FindUsConnByAddress (const Address& addr);

  /**
   * \brief jerome, X2, find one upstream connection based on the tcont Type.  0: not found
   */
	virtual const Ptr<XgponConnectionSender>& FindUsConnByTcontType (const uint16_t& type);




  /**
   * \brief Add one downstream connection belonging to this ONU
   */
  virtual void AddOneDsConn (const Ptr<XgponConnectionReceiver>& conn);


  /**
   * \brief find one downstream connection based on XGEM port-id. 0: not found
   */
  virtual const Ptr<XgponConnectionReceiver>& FindDsConnByXgemPort (uint16_t port);








  ///////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;


private:
  std::vector< Ptr<XgponTcontOnu> > m_tconts;                               //T-CONTs
  std::vector< Ptr<XgponConnectionSender> > m_usConns;                      //the downstream connections that belongs to this ONU

  std::vector< Ptr<XgponConnectionReceiver> > m_dsConns;                    //the downstream connections that belongs to this ONU
  std::vector< Ptr<XgponConnectionReceiver> > m_broadcastConns;             //broadcast downstream connections


  /* more variables may be needed */

};



}; // namespace ns3

#endif // XGPON_ONU_CONN_MANAGER_SPEED_H
