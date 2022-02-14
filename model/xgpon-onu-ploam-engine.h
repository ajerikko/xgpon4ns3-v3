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

#ifndef XGPON_ONU_PLOAM_ENGINE_H
#define XGPON_ONU_PLOAM_ENGINE_H

#include "xgpon-onu-engine.h"

#include "xgpon-link-info.h"
#include "xgpon-xgtc-ploam.h"



namespace ns3 {

class XgponOnuNetDevice;

/**
 * \ingroup xgpon
 * \brief The class used to instantiate PLOAM messages related functions of XG-PON at ONU side. It maintains the ONU's related information.
 *        Currently, we just implement the interface of this class.
 */
class XgponOnuPloamEngine : public XgponOnuEngine
{
public:

  /**
   * \brief Constructor
   */
  XgponOnuPloamEngine ();
  ~XgponOnuPloamEngine ();



  ////////////////////////////////////////////////////////////Main Functions
  /**
   * \brief receive and process one PLOAM message
   * \param msg the message to be processed
   */
  void ReceivePloamMessage (const Ptr<XgponXgtcPloam>& msg);  



  /**
   * \brief generate one PLOAM message. Note that this message is put into the queue of XgponLinkInfo to be transmitted. 
   */  
  void GeneratePloamMessage ( );  




  /////////////////////////////////////////////////////Member variables accessors
  //INLINED Functions
  void SetLinkInfo (const Ptr<XgponLinkInfo>& linkInfo);
  const Ptr<XgponLinkInfo>& GetLinkInfo ();



  /////////////////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;


private:
  Ptr<XgponLinkInfo> m_linkInfo;     //the information related with the link between OLT and this ONU

  /* more variables may be needed */

};



inline void 
XgponOnuPloamEngine::SetLinkInfo (const Ptr<XgponLinkInfo>& linkInfo)
{
  m_linkInfo = linkInfo;
}
inline const Ptr<XgponLinkInfo>& 
XgponOnuPloamEngine::GetLinkInfo ()
{
  NS_ASSERT_MSG((m_linkInfo!=0), "Link info has not been set yet.");
  return m_linkInfo;
}





}; // namespace ns3

#endif // XGPON_ONU_PLOAM_ENGINE_H
