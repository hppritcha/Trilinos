/*
// @HEADER
// ***********************************************************************
// 
//          Tpetra: Templated Linear Algebra Services Package
//                 Copyright (2008) Sandia Corporation
// 
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
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
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ************************************************************************
// @HEADER
*/

#include "Tpetra_CrsMatrix.hpp"

#ifdef HAVE_TPETRA_EXPLICIT_INSTANTIATION

#include <Kokkos_SerialNode.hpp>
#if defined(HAVE_KOKKOS_TBB)
#  include <Kokkos_TBBNode.hpp>
#endif
#if defined(HAVE_KOKKOS_THREADPOOL)
#  include <Kokkos_TPINode.hpp>
#endif
#if defined(HAVE_KOKKOS_OPENMP)
#  include <Kokkos_OpenMPNode.hpp>
#endif
#if defined(HAVE_KOKKOS_THRUST)
#  include <Kokkos_ThrustGPUNode.hpp>
#endif

#include "Tpetra_CrsMatrix_def.hpp"
#include "Tpetra_CrsGraph_def.hpp"

namespace Tpetra {

#if defined(HAVE_TPETRA_INST_DOUBLE)
  TPETRA_CRSMATRIX_INSTANT(double,int,int,Kokkos::SerialNode)

  template<>
  Teuchos::RCP<CrsMatrix<double, int, int, Kokkos::SerialNode> >
  importAndFillCompleteCrsMatrix (const Teuchos::RCP<const CrsMatrix<double, int, int, Kokkos::SerialNode> >& sourceMatrix,
				  const Import<CrsMatrix<double, int, int, Kokkos::SerialNode>::local_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::SerialNode>::global_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::SerialNode>::node_type>& importer,
				  const Teuchos::RCP<Teuchos::ParameterList>& plist);

  template<>
  Teuchos::RCP<CrsMatrix<double, int, int, Kokkos::SerialNode> >
  exportAndFillCompleteCrsMatrix (const Teuchos::RCP<const CrsMatrix<double, int, int, Kokkos::SerialNode> >& sourceMatrix,
				  const Export<CrsMatrix<double, int, int, Kokkos::SerialNode>::local_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::SerialNode>::global_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::SerialNode>::node_type>& exporter,
				  const Teuchos::RCP<Teuchos::ParameterList>& plist);

#if defined(HAVE_KOKKOS_TBB)
  TPETRA_CRSMATRIX_INSTANT(double,int,int,Kokkos::TBBNode)

  template<>
  Teuchos::RCP<CrsMatrix<double, int, int, Kokkos::TBBNode> >
  importAndFillCompleteCrsMatrix (const Teuchos::RCP<const CrsMatrix<double, int, int, Kokkos::TBBNode> >& sourceMatrix,
				  const Import<CrsMatrix<double, int, int, Kokkos::TBBNode>::local_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::TBBNode>::global_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::TBBNode>::node_type>& importer,
				  const Teuchos::RCP<Teuchos::ParameterList>& plist);

  template<>
  Teuchos::RCP<CrsMatrix<double, int, int, Kokkos::TBBNode> >
  exportAndFillCompleteCrsMatrix (const Teuchos::RCP<const CrsMatrix<double, int, int, Kokkos::TBBNode> >& sourceMatrix,
				  const Export<CrsMatrix<double, int, int, Kokkos::TBBNode>::local_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::TBBNode>::global_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::TBBNode>::node_type>& exporter,
				  const Teuchos::RCP<Teuchos::ParameterList>& plist);
#endif
#if defined(HAVE_KOKKOS_THREADPOOL)
    TPETRA_CRSMATRIX_INSTANT(double,int,int,Kokkos::TPINode)

  template<>
  Teuchos::RCP<CrsMatrix<double, int, int, Kokkos::TPINode> >
  importAndFillCompleteCrsMatrix (const Teuchos::RCP<const CrsMatrix<double, int, int, Kokkos::TPINode> >& sourceMatrix,
				  const Import<CrsMatrix<double, int, int, Kokkos::TPINode>::local_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::TPINode>::global_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::TPINode>::node_type>& importer,
				  const Teuchos::RCP<Teuchos::ParameterList>& plist);

  template<>
  Teuchos::RCP<CrsMatrix<double, int, int, Kokkos::TPINode> >
  exportAndFillCompleteCrsMatrix (const Teuchos::RCP<const CrsMatrix<double, int, int, Kokkos::TPINode> >& sourceMatrix,
				  const Export<CrsMatrix<double, int, int, Kokkos::TPINode>::local_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::TPINode>::global_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::TPINode>::node_type>& exporter,
				  const Teuchos::RCP<Teuchos::ParameterList>& plist);
#endif
#if defined(HAVE_KOKKOS_OPENMP)
    TPETRA_CRSMATRIX_INSTANT(double,int,int,Kokkos::OpenMPNode)

  template<>
  Teuchos::RCP<CrsMatrix<double, int, int, Kokkos::OpenMPNode> >
  importAndFillCompleteCrsMatrix (const Teuchos::RCP<const CrsMatrix<double, int, int, Kokkos::OpenMPNode> >& sourceMatrix,
				  const Import<CrsMatrix<double, int, int, Kokkos::OpenMPNode>::local_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::OpenMPNode>::global_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::OpenMPNode>::node_type>& importer,
				  const Teuchos::RCP<Teuchos::ParameterList>& plist);

  template<>
  Teuchos::RCP<CrsMatrix<double, int, int, Kokkos::OpenMPNode> >
  exportAndFillCompleteCrsMatrix (const Teuchos::RCP<const CrsMatrix<double, int, int, Kokkos::OpenMPNode> >& sourceMatrix,
				  const Export<CrsMatrix<double, int, int, Kokkos::OpenMPNode>::local_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::OpenMPNode>::global_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::OpenMPNode>::node_type>& exporter,
				  const Teuchos::RCP<Teuchos::ParameterList>& plist);
#endif
#if defined(HAVE_KOKKOS_THRUST) && defined(HAVE_KOKKOS_CUDA_DOUBLE)
    TPETRA_CRSMATRIX_INSTANT(double,int,int,Kokkos::ThrustGPUNode)

  template<>
  Teuchos::RCP<CrsMatrix<double, int, int, Kokkos::ThrustGPUNode> >
  importAndFillCompleteCrsMatrix (const Teuchos::RCP<const CrsMatrix<double, int, int, Kokkos::ThrustGPUNode> >& sourceMatrix,
				  const Import<CrsMatrix<double, int, int, Kokkos::ThrustGPUNode>::local_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::ThrustGPUNode>::global_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::ThrustGPUNode>::node_type>& importer,
				  const Teuchos::RCP<Teuchos::ParameterList>& plist);

  template<>
  Teuchos::RCP<CrsMatrix<double, int, int, Kokkos::ThrustGPUNode> >
  exportAndFillCompleteCrsMatrix (const Teuchos::RCP<const CrsMatrix<double, int, int, Kokkos::ThrustGPUNode> >& sourceMatrix,
				  const Export<CrsMatrix<double, int, int, Kokkos::ThrustGPUNode>::local_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::ThrustGPUNode>::global_ordinal_type, 
				               CrsMatrix<double, int, int, Kokkos::ThrustGPUNode>::node_type>& exporter,
				  const Teuchos::RCP<Teuchos::ParameterList>& plist);
#endif
#endif // DOUBLE
#if defined(HAVE_TPETRA_INST_FLOAT)
  TPETRA_CRSMATRIX_INSTANT(float,int,int,Kokkos::SerialNode)
#if defined(HAVE_KOKKOS_TBB)
  TPETRA_CRSMATRIX_INSTANT(float,int,int,Kokkos::TBBNode)
#endif
#if defined(HAVE_KOKKOS_THREADPOOL)
    TPETRA_CRSMATRIX_INSTANT(float,int,int,Kokkos::TPINode)
#endif
#if defined(HAVE_KOKKOS_OPENMP)
    TPETRA_CRSMATRIX_INSTANT(float,int,int,Kokkos::OpenMPNode)
#endif
#if defined(HAVE_KOKKOS_THRUST) && defined(HAVE_KOKKOS_CUDA_FLOAT)
    TPETRA_CRSMATRIX_INSTANT(float,int,int,Kokkos::ThrustGPUNode)
#endif
#endif // FLOAT
#if defined(HAVE_TPETRA_INST_COMPLEX_DOUBLE)
  TPETRA_CRSMATRIX_INSTANT(std::complex<double>,int,int,Kokkos::SerialNode)
#if defined(HAVE_KOKKOS_TBB)
  TPETRA_CRSMATRIX_INSTANT(std::complex<double>,int,int,Kokkos::TBBNode)
#endif
#if defined(HAVE_KOKKOS_THREADPOOL)
    TPETRA_CRSMATRIX_INSTANT(std::complex<double>,int,int,Kokkos::TPINode)
#endif
#if defined(HAVE_KOKKOS_OPENMP)
    TPETRA_CRSMATRIX_INSTANT(std::complex<double>,int,int,Kokkos::OpenMPNode)
#endif
// no complex on GPU support for now
//#if defined(HAVE_KOKKOS_THRUST) && defined(HAVE_KOKKOS_CUDA_DOUBLE)
//    TPETRA_CRSMATRIX_INSTANT(double,int,int,Kokkos::ThrustGPUNode)
//#endif
#endif // COMPLEX DOUBLE
#if defined(HAVE_TPETRA_INST_COMPLEX_FLOAT)
  TPETRA_CRSMATRIX_INSTANT(std::complex<float>,int,int,Kokkos::SerialNode)
#if defined(HAVE_KOKKOS_TBB)
  TPETRA_CRSMATRIX_INSTANT(std::complex<float>,int,int,Kokkos::TBBNode)
#endif
#if defined(HAVE_KOKKOS_THREADPOOL)
    TPETRA_CRSMATRIX_INSTANT(std::complex<float>,int,int,Kokkos::TPINode)
#endif
#if defined(HAVE_KOKKOS_OPENMP)
    TPETRA_CRSMATRIX_INSTANT(std::complex<float>,int,int,Kokkos::OpenMPNode)
#endif
// no complex on GPU support for now
//#if defined(HAVE_KOKKOS_THRUST) && defined(HAVE_KOKKOS_CUDA_DOUBLE)
//    TPETRA_CRSMATRIX_INSTANT(double,int,int,Kokkos::ThrustGPUNode)
//#endif
#endif // COMPLEX FLOAT

#ifdef HAVE_TPETRA_INST_INT_LONG
#if defined(HAVE_TPETRA_INST_DOUBLE)
  TPETRA_CRSMATRIX_INSTANT(double,int,long,Kokkos::SerialNode)
#if defined(HAVE_KOKKOS_TBB)
  TPETRA_CRSMATRIX_INSTANT(double,int,long,Kokkos::TBBNode)
#endif
#if defined(HAVE_KOKKOS_THREADPOOL)
    TPETRA_CRSMATRIX_INSTANT(double,int,long,Kokkos::TPINode)
#endif
#if defined(HAVE_KOKKOS_OPENMP)
    TPETRA_CRSMATRIX_INSTANT(double,int,long,Kokkos::OpenMPNode)
#endif
#if defined(HAVE_KOKKOS_THRUST) && defined(HAVE_KOKKOS_CUDA_DOUBLE)
    TPETRA_CRSMATRIX_INSTANT(double,int,long,Kokkos::ThrustGPUNode)
#endif
#endif // DOUBLE
#if defined(HAVE_TPETRA_INST_FLOAT)
  TPETRA_CRSMATRIX_INSTANT(float,int,long,Kokkos::SerialNode)
#if defined(HAVE_KOKKOS_TBB)
  TPETRA_CRSMATRIX_INSTANT(float,int,long,Kokkos::TBBNode)
#endif
#if defined(HAVE_KOKKOS_THREADPOOL)
    TPETRA_CRSMATRIX_INSTANT(float,int,long,Kokkos::TPINode)
#endif
#if defined(HAVE_KOKKOS_OPENMP)
    TPETRA_CRSMATRIX_INSTANT(float,int,long,Kokkos::OpenMPNode)
#endif
#if defined(HAVE_KOKKOS_THRUST) && defined(HAVE_KOKKOS_CUDA_FLOAT)
    TPETRA_CRSMATRIX_INSTANT(float,int,long,Kokkos::ThrustGPUNode)
#endif
#endif // FLOAT
#if defined(HAVE_TPETRA_INST_COMPLEX_DOUBLE)
  TPETRA_CRSMATRIX_INSTANT(std::complex<double>,int,long,Kokkos::SerialNode)
#if defined(HAVE_KOKKOS_TBB)
  TPETRA_CRSMATRIX_INSTANT(std::complex<double>,int,long,Kokkos::TBBNode)
#endif
#if defined(HAVE_KOKKOS_THREADPOOL)
    TPETRA_CRSMATRIX_INSTANT(std::complex<double>,int,long,Kokkos::TPINode)
#endif
#if defined(HAVE_KOKKOS_OPENMP)
    TPETRA_CRSMATRIX_INSTANT(std::complex<double>,int,long,Kokkos::OpenMPNode)
#endif
// no complex on GPU support for now
//#if defined(HAVE_KOKKOS_THRUST) && defined(HAVE_KOKKOS_CUDA_DOUBLE)
//    TPETRA_CRSMATRIX_INSTANT(double,int,long,Kokkos::ThrustGPUNode)
//#endif
#endif // COMPLEX DOUBLE
#if defined(HAVE_TPETRA_INST_COMPLEX_FLOAT)
  TPETRA_CRSMATRIX_INSTANT(std::complex<float>,int,long,Kokkos::SerialNode)
#if defined(HAVE_KOKKOS_TBB)
  TPETRA_CRSMATRIX_INSTANT(std::complex<float>,int,long,Kokkos::TBBNode)
#endif
#if defined(HAVE_KOKKOS_THREADPOOL)
    TPETRA_CRSMATRIX_INSTANT(std::complex<float>,int,long,Kokkos::TPINode)
#endif
#if defined(HAVE_KOKKOS_OPENMP)
    TPETRA_CRSMATRIX_INSTANT(std::complex<float>,int,long,Kokkos::OpenMPNode)
#endif
// no complex on GPU support for now
//#if defined(HAVE_KOKKOS_THRUST) && defined(HAVE_KOKKOS_CUDA_DOUBLE)
//    TPETRA_CRSMATRIX_INSTANT(double,int,long,Kokkos::ThrustGPUNode)
//#endif
#endif // COMPLEX FLOAT
#endif


} // namespace Tpetra

#endif // HAVE_TPETRA_EXPLICIT_INSTANTIATION
