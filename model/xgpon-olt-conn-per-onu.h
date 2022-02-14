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

#ifndef XGPON_OLT_CONN_PER_ONU_H
#define XGPON_OLT_CONN_PER_ONU_H

#include <vector>

#include "ns3/object.h"
#include "ns3/address.h"

#include "xgpon-connection-sender.h"
#include "xgpon-tcont-olt.h"


namespace ns3 {


/**
 * \ingroup xgpon
 * \brief This class is used at OLT-side. It contains the list of XgponTcontOlt that belongs to the same ONU and the list of its downstream connections.
 *        Instead of using for (de)multiplexing, this class is used for future extention, some control plane functions for per-onu processing.
 *
 */
class XgponOltConnPerOnu : public Object
{
public:

  /**
   * \brief Constructor
   */
  XgponOltConnPerOnu ();
  virtual ~XgponOltConnPerOnu ();


  //////////////////////////////////////////////////////////////////////Main Interfaces
  //add connection and t-cont to this per-onu structure
  void AddOneUsTcont (const Ptr<XgponTcontOlt>& tcont);
  void AddOneDsConn (const Ptr<XgponConnectionSender>& conn);


  uint32_t GetNumberOfTconts ();
  uint32_t GetNumberOfDsConns ();


  /////////////////////////////////////////////////////////////////Member variable accessor
  void SetOnuId (uint16_t onuId);
  uint16_t GetOnuId ();


  ////////////////////////////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;


private:
  uint16_t  m_onuId;                                         //the corresponding ONU-ID;

  std::vector< Ptr<XgponTcontOlt> > m_tconts;                //tconts of the same onu that contains the list of upstream connections.
  std::vector< Ptr<XgponConnectionSender> > m_connections;   //the list of downstream connections that belongs to the same ONU.

  /* more variables may be needed */

};



///////////////////////////////////////////////////////////////INLINE Functions
inline void 
XgponOltConnPerOnu::AddOneUsTcont (const Ptr<XgponTcontOlt>& tcont)
{
  m_tconts.push_back (tcont);
}

inline void 
XgponOltConnPerOnu::AddOneDsConn (const Ptr<XgponConnectionSender>& conn)
{
  m_connections.push_back (conn);
}



inline void 
XgponOltConnPerOnu::SetOnuId (uint16_t onuId)
{
  m_onuId = onuId;
}
inline uint16_t 
XgponOltConnPerOnu::GetOnuId ()
{
  return m_onuId;
}





inline uint32_t
XgponOltConnPerOnu::GetNumberOfTconts ()
{
  return m_tconts.size();
}

inline uint32_t
XgponOltConnPerOnu::GetNumberOfDsConns ()
{
  return m_connections.size();
}



}; // namespace ns3

#endif // XGPON_OLT_CONN_PER_ONU_H
