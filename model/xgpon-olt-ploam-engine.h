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

#ifndef XGPON_OLT_PLOAM_ENGINE_H
#define XGPON_OLT_PLOAM_ENGINE_H

#include "xgpon-olt-engine.h"
#include "xgpon-xgtc-ploam.h"
#include "xgpon-link-info.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The class used to instantiate PLOAM messages related functions of XG-PON at OLT side. It maintains the ONUs' related information.
 *        Currently, we just implement the interface for PLOAM messages.
 */
class XgponOltPloamEngine : public XgponOltEngine
{
public:

  /**
   * \brief Constructor
   */
  XgponOltPloamEngine ();
  virtual ~XgponOltPloamEngine ();


  ////////////////////////////////////////////////Main Functions
  /**
   * \brief receive and process one PLOAM message
   * \param msg the message to be processed
   * \param onuId the source ONU of this PLOAM message
   */
  void ReceivePloamMessage (const Ptr<XgponXgtcPloam>& msg, uint16_t onuId);  


  /**
   * \brief generate PLOAM message. 
   * \return the PLOAM message to be transmitted
   */  
  void GeneratePloamMessage ( );  


  /**
   * \brief get all ploam messages to be transmitted. called by framing engine to fill PLOAM messages into the header
   */
  std::list< Ptr<XgponXgtcPloam> >& GetPloamsForTransmit(void);




  ///////////////////////////////////////////////////////maintain linkinfo for ONUs (inline functions)
  void AddLinkInfo (const Ptr<XgponLinkInfo>& linkInfo, uint16_t onuId);
  const Ptr<XgponLinkInfo>& GetLinkInfo (uint16_t onuId);






  ////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;



private:
  std::vector< Ptr<XgponLinkInfo> > m_linkInfos;     //the information related with the links between OLT and ONUs
                                                     //table index should equal to ONU ID.

  /* more variables may be needed */
  std::list< Ptr<XgponXgtcPloam> > m_ploamMsgsToBroadcast;   //a list of ploam message whose destination is not a specific onu.

  std::list< Ptr<XgponXgtcPloam> > m_ploamMsgsToBroadcastInCurrentFrame;   //a list of ploam message whose destination is not a specific onu.

};









////////////////////////////////////////////////////////////////////INLINE Functions
inline void 
XgponOltPloamEngine::AddLinkInfo (const Ptr<XgponLinkInfo>& linkInfo, uint16_t onuId)
{
  NS_ASSERT_MSG(onuId<m_linkInfos.size(),"Trying to set an out of bounds link info");
  m_linkInfos[onuId] = linkInfo;
}
inline const Ptr<XgponLinkInfo>& 
XgponOltPloamEngine::GetLinkInfo (uint16_t onuId)
{
  NS_ASSERT_MSG(onuId<m_linkInfos.size(),"Trying to get an out of bounds link info");
  return m_linkInfos[onuId];
}


}; // namespace ns3

#endif // XGPON_OLT_PLOAM_ENGINE_H
