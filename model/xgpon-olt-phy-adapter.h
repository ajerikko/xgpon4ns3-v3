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
#ifndef XGPON_OLT_PHY_ADAPTER_H
#define XGPON_OLT_PHY_ADAPTER_H

#include "xgpon-olt-engine.h"

#include "xgpon-burst-profile.h"
#include "xgpon-ds-frame.h"
#include "xgpon-us-burst.h"


namespace ns3 {

/**
 * \ingroup xgpon
 * \brief PHY and PHY-adaptation layers at OLT-side.
 *
 */

class XgponOltPhyAdapter : public XgponOltEngine
{
public:

  /**
   * \brief Constructor
   */
  XgponOltPhyAdapter ();
  virtual ~XgponOltPhyAdapter ();


  /////////////////////Inherited from XgponPhy
  void ProcessXgponUsBurstFromChannel (const Ptr<XgponUsBurst>& burst, const Ptr<XgponBurstProfile>& profile);
  void ProcessXgtcDsFrameFromUpperLayer (const Ptr<XgponDsFrame>& frame);


  ////////////////////////Member variables accessors
  void SetPonid (uint64_t id);
  uint64_t GetPonid ( ) const;

  void SetSfc (uint64_t sfc);
  uint64_t GetSfc ( ) const;


  //////////////////////////////////////Required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;



private:
  //PON ID and Frame Counter used in downstream frame header
  uint64_t  m_sfc;                //51bits
  uint64_t  m_ponid;              //51bits

};




inline void 
XgponOltPhyAdapter::SetPonid (uint64_t id)
{
  m_ponid = id;
}
inline uint64_t 
XgponOltPhyAdapter::GetPonid ( ) const
{
  return m_ponid;
}

inline void 
XgponOltPhyAdapter::SetSfc (uint64_t sfc)
{
  m_sfc = sfc;
}
inline uint64_t 
XgponOltPhyAdapter::GetSfc ( ) const
{
  return m_sfc;
}

}// namespace ns3

#endif // XGPON_OLT_PHY_ADAPTER_H
