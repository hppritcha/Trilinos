// @HEADER
// ************************************************************************
//
//               Rapid Optimization Library (ROL) Package
//                 Copyright (2014) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact lead developers:
//              Drew Kouri   (dpkouri@sandia.gov) and
//              Denis Ridzal (dridzal@sandia.gov)
//
// ************************************************************************
// @HEADER

#pragma once
#ifndef VDP_DYNAMICCONSTRAINT_HPP
#define VDP_DYNAMICCONSTRAINT_HPP

#include "ROL_DynamicConstraint.hpp"
#include "ROL_StdVector.hpp"

/** \class VdP::DynamicConstraint
    \brief Compute time-step for the Van der Pol optimal control problem

    The Van der Pol equation is

    \f[ \dot u_1 = u_2,\quad \dot u_2 = z(1-u_1^2)u_2-u_1 \f]
   
    The initial conditions are

    \f[ u_1(0) = 1,\quad u_2(0) = 0 \f]

*/

namespace VdP {

template<typename Real> 
class DynamicConstraint : public ROL::DynamicConstraint<Real> {

  using V  = ROL::Vector<Real>;
  using SV = ROL::StdVector<Real>;
  using TS = ROL::TimeStamp<Real>;

private:

  ROL::Ptr<std::vector<Real>> getVector( V& x ) {
    return (static_cast<SV&>(x)).getVector();
  }

  ROL::Ptr<const std::vector<Real>> getVector( const V& x ) {
    return (static_cast<const SV&>(x)).getVector();
  }


public: 
  void value( V& c, const V& uo, const V& un, 
                    const V& z, const TS& ts ) const override {

    auto cp  = getVector(c);  
    auto uop = getVector(uo);   
    auto unp = getVector(un);
    Real dt = ts.t[1]-ts.t[0];

    auto term = (1-uop->at(0)*uop->at(0))*uop->at(1) - uop->at(0) +
                (1-unp->at(0)*unp->at(0))*unp->at(1) - unp->at(0); 

    cp->at(0) = unp->at(0) - uno->at(0) - 0.5*dt*( unp->at(1) + uop->at(1) );
    cp->at(1) = unp->at(1) - uno->at(1) - 0.5*dt*zp->at(0)*term;
    
  }

//  void solve( V& c, const V& uo, V& un, 
//                    const V& z, const TS& ts ) const override {}

  //----------------------------------------------------------------------------
  // Partial Jacobians
  void applyJacobian_uo( V& jv, const V& vo, const V& uo, 
                                const V& un, const V& z, 
                                const TS& ts ) const override {

    auto jvp = getVector(jv);   auto vop = getVector(vo);
    auto uop = getVector(uo);   auto unp = getVector(un);
    Real dt = ts.t[1]-ts.t[0];

    Real j11 = -1.0;
    Real j12 = -0.5*dt;
    Real j21 = -1.0 +     dt*uop->at(0)*uop->at(1);
    Real j22 = -1.0 - 0.5*dt*uop->at(0)*uop->at(0);

    jvp->at(0) = j11*vop->at(0) + j12*vop->at(1);
    jvp->at(1) = j21*vop->at(0) + j22*vop->at(1);  
    
  }

  void applyJacobian_un( V& jv, const V& vn, const V& uo, 
                                const V& un, const V& z, 
                                const TS& ts ) const override {

    auto jvp = getVector(jv);   auto vnp = getVector(vn);
    auto uop = getVector(uo);   auto unp = getVector(un);
    Real dt = ts.t[1]-ts.t[0];

    Real j11 =  1.0;
    Real j12 = -0.5*dt;
    Real j21 = -1.0 + dt*unp->at(0)*unp->at(1);
    Real j22 = -1.0 - 0.5*dt*unp->at(0)*unp->at(0);

    jvp->at(0) = j11*vnp->at(0) + j12*vnp->at(1);      
    jvp->at(1) = j21*vnp->at(0) + j22*vnp->at(1);    
  }

  void applyJacobian_z( V& jv, const V& vz, const V& uo, 
                               const V& un, const V& z, 
                               const TS& ts ) const override {

    auto jvp = getVector(jv);   auto vzp = getVector(vz);
    auto uop = getVector(uo);   auto unp = getVector(un);
    Real dt = ts.t[1]-ts.t[0];

    auto term = (1-uop->at(0)*uop->at(0))*uop->at(1) - uop->at(0) +
                (1-unp->at(0)*unp->at(0))*unp->at(1) - unp->at(0); 

    jvp->at(0) =  0.0;
    jvp->at(1) = -0.5*dt*vzp->at(0)*term;
  }

  //----------------------------------------------------------------------------
  // Adjoint partial Jacobians
  void applyAdjointJacobian_uo( V& ajv, const V& v, const V& uo, 
                                        const V& un, const V& z, 
                                        const TS& ts ) const override {

    auto ajvp = getVector(ajv);  auto vop = getVector(vo);
    auto uop  = getVector(uo);   auto unp = getVector(un);
    Real dt = ts.t[1]-ts.t[0];

    Real j11 = -1.0;
    Real j12 = -0.5*dt;
    Real j21 = -1.0 +     dt*uop->at(0)*uop->at(1);
    Real j22 = -1.0 - 0.5*dt*uop->at(0)*uop->at(0);

    ajvp->at(0) = j11*vop->at(0) + j21*vop->at(1);
    ajvp->at(1) = j12*vop->at(0) + j22*vop->at(1);  

  }

  void applyAdjointJacobian_un( V& ajv, const V& dualv, const V& uo, 
                                        const V& un, const V& z, 
                                        const TS& ts ) const override {

    auto ajvp = getVector(ajv);  auto vnp = getVector(vn);
    auto uop  = getVector(uo);   auto unp = getVector(un);
    Real dt = ts.t[1]-ts.t[0];

    Real j11 =  1.0;
    Real j12 = -0.5*dt;
    Real j21 = -1.0 + dt*unp->at(0)*unp->at(1);
    Real j22 = -1.0 - 0.5*dt*unp->at(0)*unp->at(0);

    ajvp->at(0) = j11*vnp->at(0) + j21*vnp->at(1);      
    ajvp->at(1) = j12*vnp->at(0) + j22*vnp->at(1);    

  }

  void applyAdjointJacobian_z( V& ajv, const V& vz, const V& uo, 
                                       const V& un, const V& z, 
                                       const TS& ts ) const override {

    auto ajvp = getVector(ajv);  auto vzp = getVector(vz);
    auto uop  = getVector(uo);   auto unp = getVector(un);
    Real dt = ts.t[1]-ts.t[0];

    auto term = (1-uop->at(0)*uop->at(0))*uop->at(1) - uop->at(0) +
                (1-unp->at(0)*unp->at(0))*unp->at(1) - unp->at(0); 

    jvp->at(0) = -0.5*dt*vzp->at(1)*term;

  }

  //----------------------------------------------------------------------------
  // Inverses
  void applyInverseJacobian_un( V& ijv, const V& vn, const V& uo, 
                                        const V& un, const V& z, 
                                        const TS& ts ) const override {

    auto jvp = getVector(jv);   auto vnp = getVector(vn);
    auto uop = getVector(uo);   auto unp = getVector(un);
    Real dt = ts.t[1]-ts.t[0];

    Real j11 =  1.0;
    Real j12 = -0.5*dt;
    Real j21 = -1.0 + dt*unp->at(0)*unp->at(1);
    Real j22 = -1.0 - 0.5*dt*unp->at(0)*unp->at(0);
    Real det = j11*j22-j21*j12;

    ijvp->at(0) = ( j22*vnp->at(0) - j12*vnp->at(1) )/det;      
    ijvp->at(1) = (-j21*vnp->at(0) + j11*vnp->at(1) )/det;    

  }
    
  void applyInverseAdjointJacobian_un( V& iajv, const V& vn, const V& uo, 
                                                const V& un, const V& z, 
                                                const TS& ts ) const override {

    auto ajvp = getVector(ajv);  auto vnp = getVector(vn);
    auto uop  = getVector(uo);   auto unp = getVector(un);
    Real dt = ts.t[1]-ts.t[0];

    Real j11 =  1.0;
    Real j12 = -0.5*dt;
    Real j21 = -1.0 + dt*unp->at(0)*unp->at(1);
    Real j22 = -1.0 - 0.5*dt*unp->at(0)*unp->at(0);
    Real det = j11*j22-j21*j12;

    iajvp->at(0) = ( j22*vnp->at(0) - j21*vnp->at(1) );      
    iajvp->at(1) = (-j12*vnp->at(0) + j11*vnp->at(1) );    

  }

  //----------------------------------------------------------------------------
  // Adjoint Hessian components
  void applyAdjointHessian_un_un( V& ahwv, const V& wn, const V& vn,
                                           const V& uo, const V& un, 
                                           const V& z, const TS& ts ) const override {

    auto ahwvp = getVector(ahwv);    auto wp    = getVector(w);
    auto vp    = getVector(v);       auto uop   = getVector(uo);
    auto unp   = getVector(un);      auto zp    = getVector(z);
    Real dt = ts.t[1]-ts.t[0];
    
    Real h11 = dt*unp->at(1)*wp->at(1);
    Real h12 = dt*unp->at(0)*wp->at(1);
    Real h21 = dt*unp->at(0)*wp->at(1);
    Real h22 = 0.0;

    ahwvp->at(0) = h11*vp->at(0) + h12*vp->at(1);
    ahwvp->at(1) = h21*vp->at(0) + h22*vp->at(1);    
  }

  void applyAdjointHessian_un_uo( V& ahwv, const V& w,  const V& vn,
                                           const V& uo, const V& un, 
                                           const V& z,  const TS& ts ) const override {
    ahwv.zero();
  }

  void applyAdjointHessian_un_z( V& ahwv, const V& w,  const V& vn,
                                          const V& uo, const V& un, 
                                          const V& z,  const TS& ts ) const override {
    // TODO
  }

  void applyAdjointHessian_uo_un( V& ahwv, const V& w,  const V& vo,
                                           const V& uo, const V& un, 
                                           const V& z,  const TS& ts ) const override {
    ahwv.zero();
  }

  void applyAdjointHessian_uo_uo( V& ahwv, const V& w,  const V& v,
                                           const V& uo, const V& un, 
                                           const V& z,  const TS& ts ) const override {
  
    auto ahwvp = getVector(ahwv);    auto wp    = getVector(w);
    auto vp    = getVector(v);       auto uop   = getVector(uo);
    auto unp   = getVector(un);      auto zp    = getVector(z);
    Real dt = ts.t[1]-ts.t[0];
    
    Real h11 = dt*uop->at(1)*wp->at(1);
    Real h12 = dt*uop->at(0)*wp->at(1);
    Real h21 = dt*uop->at(0)*wp->at(1);
    Real h22 = 0.0;

    ahwvp->at(0) = h11*vp->at(0) + h12*vp->at(1);
    ahwvp->at(1) = h21*vp->at(0) + h22*vp->at(1);
  }

  void applyAdjointHessian_uo_z( V& ahwv, const V& w,  const V& vo,
                                          const V& uo, const V& un, 
                                          const V& z,  const TS& ts ) const override {
    // TODO
  }

   void applyAdjointHessian_z_un( V& ahwv, const V& w, const V& vz,
                                           const V& uo, const V& un, 
                                           const V& z, const TS& ts ) const override {
    // TODO
  }

   void applyAdjointHessian_z_uo( V& ahwv, const V& w, const V& vz,
                                           const V& uo, const V& un, 
                                           const V& z, const TS& ts ) const override {
    // TODO
  }
  
   void applyAdjointHessian_z_z( V& ahwv, const V& w, const V& vz,
                                          const V& uo, const V& un, 
                                          const V& z, const TS& ts ) const override {
    ahwv.zero();
  }


}; // VdP::DynamicConstraint


} // namespace VdP




#endif // VDP_DYNAMICCONSTRAINT_HPP
