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
#ifndef PON_FRAME_H
#define PON_FRAME_H

#include "ns3/simple-ref-count.h"
#include "ns3/buffer.h"


namespace ns3 {

/**
 * \ingroup pon
 * \brief The frame transmitted over PON.
 *
 * This class is an abstract class that defines the general operations for (de)serialization. 
 * In XG-PON, it is instantiated by XgponDsFrame and XgponUsBurst for downstream and upstream directions, respectively.
 */
class PonFrame : public SimpleRefCount<PonFrame>
{
public:

  /**
   * \brief Constructor
   */
  PonFrame () { }
  virtual ~PonFrame () { }

  //general operations for (de)serialization. In the first phase, we can just implement "GetSerializedSize".
  virtual void Print (std::ostream &os) const = 0;
  virtual uint32_t GetSerializedSize (void) const =0;
  virtual void Serialize (Buffer::Iterator start) const = 0;
  virtual uint32_t Deserialize (Buffer::Iterator start) = 0;

};

}; // namespace ns3

#endif // PON_FRAME_H
