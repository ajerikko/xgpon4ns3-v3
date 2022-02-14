/*
 * Copyright (c)  2012 The Provost, Fellows and Scholars of the 
 * College of the Holy and Undivided Trinity of Queen Elizabeth near Dublin. 
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
 * Author: Pedro Alvarez <pinheirp@tcd.ie>
 */

#ifndef XGPON_ONU_PHY_ADAPTER_H
#define XGPON_ONU_PHY_ADAPTER_H


#include "xgpon-onu-engine.h"

#include "xgpon-ds-frame.h"
#include "xgpon-us-burst.h"
#include "xgpon-burst-profile.h"



namespace ns3 {


/**
 * \ingroup xgpon
 * \brief PHY and PHY-adaptation layers at ONU-side.
 */
class XgponOnuPhyAdapter : public XgponOnuEngine
{
public:

  /**
   * \brief Constructor
   */
  XgponOnuPhyAdapter ();
  virtual ~XgponOnuPhyAdapter ();


  /////////////////////Main functions
  void ProcessXgponDsFrameFromChannel (const Ptr<XgponDsFrame>& frame); 
  void ProcessXgtcBurstFromUpperLayer (const Ptr<XgponUsBurst>& burst, const Ptr<XgponBurstProfile>& profile);


  void SetPonid (uint64_t id);
  uint64_t GetPonid ( ) const;

  void SetSfc (uint64_t sfc);
  uint64_t GetSfc ( ) const;



  //////////////////////////////////////////Required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;


private:

  //PON ID and Frame Counter used in downstream frame header
  uint64_t  m_sfc;                //51bits
  uint64_t  m_ponid;              //51bits


/** Suggested values for preamble and delimiter; may be used in the future.

Preamble            32-bit delimiter                    64-bit delimiter
0x BB52 1E26        0x A376 70C9                        0x B9D4 3E68 462B C197 
                    0x 4BDE 1B90 (FEC on)               0x B9D4 3E68 462B C197 (FEC on)
                    0x A376 70C9 (FEC off)              0x B752 1F06 48AD E879 (FEC off)
0x AAAA AAAA        0x AD4C C30F                        0x B3BD D310 B2C5 0FA1
                    0x A566 79E0 (FEC on)               0x B3BD D310 B2C5 0FA1 (FEC on)
                    0x AD4C C30F (FEC off)              0x CE99 CE5E 5028 B41F (FEC off) 
**********/


};


inline void 
XgponOnuPhyAdapter::SetPonid (uint64_t id)
{
  m_ponid = id;
}
inline uint64_t 
XgponOnuPhyAdapter::GetPonid ( ) const
{
  return m_ponid;
}

inline void 
XgponOnuPhyAdapter::SetSfc (uint64_t sfc)
{
  m_sfc = sfc;
}
inline uint64_t 
XgponOnuPhyAdapter::GetSfc ( ) const
{
  return m_sfc;
}


}// namespace ns3

#endif // XGPON_ONU_PHY_ADAPTER_H
