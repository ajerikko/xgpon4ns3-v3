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

#ifndef XGPON_FIFO_QUEUE_H
#define XGPON_FIFO_QUEUE_H

#include <queue>

#include "xgpon-queue.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The FIFO queue used for each XG-PON connection at the sender side.  
 *        Other queue disciplines may be implemented in the future.
 */
class XgponFifoQueue : public XgponQueue
{
public:

  /**
   * \brief Constructor
   */
  XgponFifoQueue ();
  virtual ~XgponFifoQueue (); 


  ////////////////////////////////////////////////////Functions required by NS-3
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;


private:

  virtual bool DoEnqueue (const Ptr<Packet>& p);
  virtual const Ptr<Packet> DoDequeue (void);     //note that we cannot return one reference since the queue might be empty.
  virtual const Ptr<const Packet> DoPeek (void) const;  //note that we cannot return one reference since the queue might be empty.


  std::queue<Ptr<Packet> > m_packets;

};

}; // namespace ns3

#endif // XGPON_FIFO_QUEUE_H
