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

#ifndef XGPON_DS_FRAME_H
#define XGPON_DS_FRAME_H

#include <cstdlib>
#include <stack>

#include "pon-frame.h"
#include "xgpon-psbd.h"
#include "xgpon-xgtc-ds-frame.h"



namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The downstream XGPON frame (XGTC Frame plus PSBd). It is allocated from a pool to avoid too many calls of malloc.
 *
 * This class has fields corresponding to those in an XGPON downstream frame (the XGTC frame and PSBd) 
 * as well as methods for serialization to and deserialization from a byte buffer.
 * It follows ITU-T G.987.3 recommendation.
 */
class XgponDsFrame : public PonFrame
{
  //static uint64_t CREATED_DS_FRAME_NUM4DEBUG;
  //static uint64_t DELETED_DS_FRAME_NUM4DEBUG;

  //used to allocate/free this class from a pool to save CPU.
  static bool m_poolEnabled;
  static std::stack<void*> m_pool;
public:

  /**
   * \brief Constructor
   */
  XgponDsFrame ();
  virtual ~XgponDsFrame ();


  ////////////////////////////////////////////////member variable accessors
  /**
   * \brief get physical layer header
   */
  XgponPsbd& GetPsbd ();

  /**
   * \brief get the content related with XGTC layer
   */
  XgponXgtcDsFrame& GetXgtcDsFrame ();



  ///////////////////////////////////////////Override new and delete to use a pool for avoiding to call malloc/free too many times.
  void* operator new(size_t size) noexcept(false); //throw(const char*);
  void operator delete(void *p);

  //called before the end of simulation to avoid memory leakage.
  static void DisablePoolAllocation();



  /////////////////////////inherited from PonFrame. In the first phase, we can just implement "GetSerializedSize".
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);



private:

  XgponPsbd m_psbd;
  XgponXgtcDsFrame m_xgtcDsFrame;



  //disable users to call new[] and delete[].
  void* operator new[](size_t size) noexcept(false) //throw(const char*) 
  {
    void *p = malloc(size);
    if (!p) throw "XgponDsFrame::operator new[]() error!!!";
    return p; 
  }
  void operator delete[](void *p) { free(p); }

};







//////////////////////////////////////////////////////INLINE functions
inline XgponPsbd& 
XgponDsFrame::GetPsbd ()
{
  return m_psbd;
}

inline XgponXgtcDsFrame&
XgponDsFrame::GetXgtcDsFrame ()
{
  return m_xgtcDsFrame;
}





}; // namespace ns3

#endif // XGPON_DS_FRAME_H
