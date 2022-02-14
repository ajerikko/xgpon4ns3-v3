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

#ifndef XGPON_ONU_DBA_ENGINE_H
#define XGPON_ONU_DBA_ENGINE_H

#include "xgpon-onu-engine.h"

#include "xgpon-xgtc-dbru.h"
#include "xgpon-xgtc-bwmap.h"




namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The class used to instantiate DBA functions of XG-PON at ONU side. 
 *        It is responsible to report queue status and upload data according to BWmap.
 *        When multiple upstream connections belong to the same Alloc-ID, there is a per-AllocID upstream scheduler used to schedule these connections based on BWmap and their QoS parameters.
 *
 */
class XgponOnuDbaEngine : public XgponOnuEngine
{
public:

  /**
   * \brief Constructor
   */
  XgponOnuDbaEngine ();
  virtual ~XgponOnuDbaEngine ();


  /**
   * \brief generate the buffer occupancy report for the Alloc-ID.
   * \param allocId the alloc-id that this report belongs to
   */  
  Ptr<XgponXgtcDbru> GenerateStatusReport (uint16_t allocId);  


  /**
   * \brief processing BWmap in the header, schedule packet burst accordingly.
   *        Note that multiple bursts may be allocated for the same ONU in the same BWMAP.
   * \param bwmap the BWmap in the header of a downstream XGTC frame
   */ 
  void ProcessBwMap (const Ptr<XgponXgtcBwmap>& bwmap);  



  //////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;


private:
  /* more variables may be needed */

};

}; // namespace ns3

#endif // XGPON_ONU_DBA_ENGINE_H
