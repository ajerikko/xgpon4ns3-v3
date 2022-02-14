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

#ifndef XGPON_US_BURST_H
#define XGPON_US_BURST_H

#include <cstdlib>
#include <stack>

#include "pon-frame.h"
#include "xgpon-psbu.h"
#include "xgpon-xgtc-us-burst.h"


namespace ns3 {

/**
 * \ingroup xgpon
 * \brief The upstream XGPON burst (XGTC burst plus PSBu).
 *
 * This class has fields corresponding to those in an XGPON upstream burst (XGTC burst + PSBu) 
 * as well as methods for serialization to and deserialization from a byte buffer.
 * It follows ITU-T G.987.3 recommendation.
 */
class XgponUsBurst : public PonFrame
{
  //static uint64_t CREATED_US_BURST_NUM4DEBUG;
  //static uint64_t DELETED_US_BURST_NUM4DEBUG;

  //used to allocate this structure from a pool for saving CPU.
  static bool m_poolEnabled;
  static std::stack<void*> m_pool;

public:

  /**
   * \brief Constructor
   *
   */
  XgponUsBurst ();
  virtual ~XgponUsBurst ();


  //////////////////////////////////////member variables operators accessors
  /**
   * \brief get physical layer header
   */
  XgponPsbu& GetPsbu ();

  /**
   * \brief get the content related with XGTC layer
   */
  XgponXgtcUsBurst& GetXgtcUsBurst ();



  
  /////////////////////////////////////////Override new and delete to use a pool for call malloc/free too many times.
  void* operator new(size_t size) noexcept(false); //throw(const char*);
  void operator delete(void *p);

  //called before the end of simulation to avoid memory leakage.
  static void DisablePoolAllocation();



  ///////////////////////////inherited from PonFrame. In the first phase, we can just implement "GetSerializedSize".
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  XgponPsbu m_psbu;
  XgponXgtcUsBurst m_xgtcUsBurst;

  //disable users to call new[] and delete[].
  void* operator new[](size_t size) noexcept(false) //throw(const char*) 
  {
    void *p = malloc(size);
    if (!p) throw "XgponUsBurst::operator new[]() error!!!";
    return p; 
  }
  void operator delete[](void *p) { free(p); }
};




///////////////////////////////////////////////////INLINE Functions
inline XgponPsbu&
XgponUsBurst::GetPsbu ()
{
  return m_psbu;
}

inline XgponXgtcUsBurst&
XgponUsBurst::GetXgtcUsBurst ()
{
  return m_xgtcUsBurst;
}



}; // namespace ns3

#endif // XGPON_US_Burst_H
