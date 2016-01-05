// @HEADER
// ************************************************************************
//
//                           Intrepid2 Package
//                 Copyright (2007) Sandia Corporation
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
// Questions? Contact Pavel Bochev  (pbboche@sandia.gov)
//                    Denis Ridzal  (dridzal@sandia.gov), or
//                    Kara Peterson (kjpeter@sandia.gov)
//
// ************************************************************************
// @HEADER

/** \file test_01.cpp
\brief  Unit tests for the Intrepid2::G_TRIPRISM_C1_FEM class.
\author Created by P. Bochev, D. Ridzal, K. Peterson and M. Perego.
*/
#include "Intrepid2_FieldContainer.hpp"
#include "Intrepid2_HGRAD_PYR_C1_FEM.hpp"
#include "Teuchos_oblackholestream.hpp"
#include "Teuchos_RCP.hpp"
#include "Teuchos_GlobalMPISession.hpp"

using namespace std;
using namespace Intrepid2;

#define INTREPID_TEST_COMMAND( S , throwCounter, nException )                                                              \
{                                                                                                                          \
  ++nException;                                                                                                            \
  try {                                                                                                                    \
    S ;                                                                                                                    \
  }                                                                                                                        \
  catch (std::logic_error err) {                                                                                           \
      ++throwCounter;                                                                                                      \
      *outStream << "Expected Error " << nException << " -------------------------------------------------------------\n"; \
      *outStream << err.what() << '\n';                                                                                    \
      *outStream << "-------------------------------------------------------------------------------" << "\n\n";           \
  };                                                                                                                       \
}

int main(int argc, char *argv[]) {
  
  Teuchos::GlobalMPISession mpiSession(&argc, &argv);
Kokkos::initialize();
  // This little trick lets us print to std::cout only if
  // a (dummy) command-line argument is provided.
  int iprint     = argc - 1;
  Teuchos::RCP<std::ostream> outStream;
  Teuchos::oblackholestream bhs; // outputs nothing
  if (iprint > 0)
    outStream = Teuchos::rcp(&std::cout, false);
  else
    outStream = Teuchos::rcp(&bhs, false);
  
  // Save the format state of the original std::cout.
  Teuchos::oblackholestream oldFormatState;
  oldFormatState.copyfmt(std::cout);
  
  *outStream \
    << "===============================================================================\n" \
    << "|                                                                             |\n" \
    << "|                 Unit Test (Basis_HGRAD_PYR_C1_FEM)                         |\n" \
    << "|                                                                             |\n" \
    << "|     1) Conversion of Dof tags into Dof ordinals and back                    |\n" \
    << "|     2) Basis values for VALUE, GRAD, CURL, and Dk operators                 |\n" \
    << "|                                                                             |\n" \
    << "|  Questions? Contact  Pavel Bochev  (pbboche@sandia.gov),                    |\n" \
    << "|                      Denis Ridzal  (dridzal@sandia.gov),                    |\n" \
    << "|                      Kara Peterson (kjpeter@sandia.gov).                    |\n" \
    << "|                                                                             |\n" \
    << "|  Intrepid's website: http://trilinos.sandia.gov/packages/intrepid           |\n" \
    << "|  Trilinos website:   http://trilinos.sandia.gov                             |\n" \
    << "|                                                                             |\n" \
    << "===============================================================================\n"\
    << "| TEST 1: Basis creation, exception testing                                   |\n"\
    << "===============================================================================\n";
  
  // Define basis and error flag
  Basis_HGRAD_PYR_C1_FEM<double, FieldContainer<double> > pyrBasis;
  int errorFlag = 0;

  // Initialize throw counter for exception testing
  int nException     = 0;
  int throwCounter   = 0;

  // Define array containing the 6 vertices of the reference PYRMID and some other points.
  FieldContainer<double> pyrNodes(10, 3);
  pyrNodes(0,0) = -1.0;  pyrNodes(0,1) = -1.0;  pyrNodes(0,2) =  0;
  pyrNodes(1,0) =  1.0;  pyrNodes(1,1) = -1.0;  pyrNodes(1,2) =  0;
  pyrNodes(2,0) =  1.0;  pyrNodes(2,1) =  1.0;  pyrNodes(2,2) =  0;
  pyrNodes(3,0) = -1.0;  pyrNodes(3,1) =  1.0;  pyrNodes(3,2) =  0;
  pyrNodes(4,0) =  0.0;  pyrNodes(4,1) =  0.0;  pyrNodes(4,2) =  1.0;
    
  pyrNodes(5,0) =  0.25; pyrNodes(5,1) =  0.5;  pyrNodes(5,2) = 0.2;
  pyrNodes(6,0) = -0.7 ; pyrNodes(6,1) =  0.3;  pyrNodes(6,2) = 0.3;
  pyrNodes(7,0) =  0.;   pyrNodes(7,1) = -0.05; pyrNodes(7,2) = 0.95;
  pyrNodes(8,0) = -0.15; pyrNodes(8,1) = -0.2;  pyrNodes(8,2) = 0.75;
  pyrNodes(9,0) = -0.4;  pyrNodes(9,1) =  0.9;  pyrNodes(9,2) = 0.0;


  // Generic array for the output values; needs to be properly resized depending on the operator type
  FieldContainer<double> vals;

  try{
    // exception #1: CURL cannot be applied to scalar functions
    // resize vals to rank-3 container with dimensions (num. points, num. basis functions)
    vals.resize(pyrBasis.getCardinality(), pyrNodes.dimension(0), 3 );
    INTREPID_TEST_COMMAND( pyrBasis.getValues(vals, pyrNodes, OPERATOR_CURL), throwCounter, nException );

    // exception #2: DIV cannot be applied to scalar functions
    // resize vals to rank-2 container with dimensions (num. points, num. basis functions)
    vals.resize(pyrBasis.getCardinality(), pyrNodes.dimension(0) );
    INTREPID_TEST_COMMAND( pyrBasis.getValues(vals, pyrNodes, OPERATOR_DIV), throwCounter, nException );
        
    // Exceptions 3-7: all bf tags/bf Ids below are wrong and should cause getDofOrdinal() and 
    // getDofTag() to access invalid array elements thereby causing bounds check exception
    // exception #3
    INTREPID_TEST_COMMAND( pyrBasis.getDofOrdinal(3,0,0), throwCounter, nException );
    // exception #4
    INTREPID_TEST_COMMAND( pyrBasis.getDofOrdinal(1,1,1), throwCounter, nException );
    // exception #5
    INTREPID_TEST_COMMAND( pyrBasis.getDofOrdinal(0,6,0), throwCounter, nException );
    // exception #6
    INTREPID_TEST_COMMAND( pyrBasis.getDofTag(7), throwCounter, nException );
    // exception #7
    INTREPID_TEST_COMMAND( pyrBasis.getDofTag(-1), throwCounter, nException );
    
#ifdef HAVE_INTREPID2_DEBUG
    // Exceptions 8-18 test exception handling with incorrectly dimensioned input/output arrays
    // exception #8: input points array must be of rank-2
    FieldContainer<double> badPoints1(4, 5, 3);
    INTREPID_TEST_COMMAND( pyrBasis.getValues(vals, badPoints1, OPERATOR_VALUE), throwCounter, nException );
    
    // exception #9 dimension 1 in the input point array must equal space dimension of the cell
    FieldContainer<double> badPoints2(4, 2);
    INTREPID_TEST_COMMAND( pyrBasis.getValues(vals, badPoints2, OPERATOR_VALUE), throwCounter, nException );
    
    // exception #10 output values must be of rank-2 for OPERATOR_VALUE
    FieldContainer<double> badVals1(4, 3, 1);
    INTREPID_TEST_COMMAND( pyrBasis.getValues(badVals1, pyrNodes, OPERATOR_VALUE), throwCounter, nException );
    
    // exception #11 output values must be of rank-3 for OPERATOR_GRAD
    FieldContainer<double> badVals2(4, 3);
    INTREPID_TEST_COMMAND( pyrBasis.getValues(badVals2, pyrNodes, OPERATOR_GRAD), throwCounter, nException );
    
    // exception #12 output values must be of rank-3 for OPERATOR_D1
    INTREPID_TEST_COMMAND( pyrBasis.getValues(badVals2, pyrNodes, OPERATOR_D1), throwCounter, nException );
    
    // exception #13 output values must be of rank-3 for OPERATOR_D2
    //INTREPID_TEST_COMMAND( pyrBasis.getValues(badVals2, pyrNodes, OPERATOR_D2), throwCounter, nException );
    
    // exception #14 incorrect 0th dimension of output array (must equal number of basis functions)
    FieldContainer<double> badVals3(pyrBasis.getCardinality() + 1, pyrNodes.dimension(0));
    INTREPID_TEST_COMMAND( pyrBasis.getValues(badVals3, pyrNodes, OPERATOR_VALUE), throwCounter, nException );
    
    // exception #15 incorrect 1st dimension of output array (must equal number of points)
    FieldContainer<double> badVals4(pyrBasis.getCardinality(), pyrNodes.dimension(0) + 1);
    INTREPID_TEST_COMMAND( pyrBasis.getValues(badVals4, pyrNodes, OPERATOR_VALUE), throwCounter, nException );
    
    // exception #16: incorrect 2nd dimension of output array (must equal the space dimension)
    FieldContainer<double> badVals5(pyrBasis.getCardinality(), pyrNodes.dimension(0), 4);
    INTREPID_TEST_COMMAND( pyrBasis.getValues(badVals5, pyrNodes, OPERATOR_GRAD), throwCounter, nException );
    
    // exception #17: incorrect 2nd dimension of output array (must equal D2 cardinality in 3D)
    //FieldContainer<double> badVals6(pyrBasis.getCardinality(), pyrNodes.dimension(0), 40);
    //INTREPID_TEST_COMMAND( pyrBasis.getValues(badVals6, pyrNodes, OPERATOR_D2), throwCounter, nException );
    
    // exception #18: incorrect 2nd dimension of output array (must equal D3 cardinality in 3D)
    //INTREPID_TEST_COMMAND( pyrBasis.getValues(badVals6, pyrNodes, OPERATOR_D3), throwCounter, nException );
#endif

  }
  catch (std::logic_error err) {
    *outStream << "UNEXPECTED ERROR !!! ----------------------------------------------------------\n";
    *outStream << err.what() << '\n';
    *outStream << "-------------------------------------------------------------------------------" << "\n\n";
    errorFlag = -1000;
  };
  
  // Check if number of thrown exceptions matches the one we expect - 18
  if (throwCounter != nException) {
    errorFlag++;
    *outStream << std::setw(70) << "^^^^----FAILURE!" << "\n";
  }
  
  *outStream \
    << "\n"
    << "===============================================================================\n"\
    << "| TEST 2: correctness of tag to enum and enum to tag lookups                  |\n"\
    << "===============================================================================\n";
  
  try{
    std::vector<std::vector<int> > allTags = pyrBasis.getAllDofTags();
    
    // Loop over all tags, lookup the associated dof enumeration and then lookup the tag again
    for (unsigned i = 0; i < allTags.size(); i++) {
      int bfOrd  = pyrBasis.getDofOrdinal(allTags[i][0], allTags[i][1], allTags[i][2]);
      
      std::vector<int> myTag = pyrBasis.getDofTag(bfOrd);
       if( !( (myTag[0] == allTags[i][0]) &&
              (myTag[1] == allTags[i][1]) &&
              (myTag[2] == allTags[i][2]) &&
              (myTag[3] == allTags[i][3]) ) ) {
        errorFlag++;
        *outStream << std::setw(70) << "^^^^----FAILURE!" << "\n";
        *outStream << " getDofOrdinal( {" 
          << allTags[i][0] << ", " 
          << allTags[i][1] << ", " 
          << allTags[i][2] << ", " 
          << allTags[i][3] << "}) = " << bfOrd <<" but \n";   
        *outStream << " getDofTag(" << bfOrd << ") = { "
          << myTag[0] << ", " 
          << myTag[1] << ", " 
          << myTag[2] << ", " 
          << myTag[3] << "}\n";        
      }
    }
    
    // Now do the same but loop over basis functions
    for( int bfOrd = 0; bfOrd < pyrBasis.getCardinality(); bfOrd++) {
      std::vector<int> myTag  = pyrBasis.getDofTag(bfOrd);
      int myBfOrd = pyrBasis.getDofOrdinal(myTag[0], myTag[1], myTag[2]);
      if( bfOrd != myBfOrd) {
        errorFlag++;
        *outStream << std::setw(70) << "^^^^----FAILURE!" << "\n";
        *outStream << " getDofTag(" << bfOrd << ") = { "
          << myTag[0] << ", " 
          << myTag[1] << ", " 
          << myTag[2] << ", " 
          << myTag[3] << "} but getDofOrdinal({" 
          << myTag[0] << ", " 
          << myTag[1] << ", " 
          << myTag[2] << ", " 
          << myTag[3] << "} ) = " << myBfOrd << "\n";
      }
    }
  }
  catch (std::logic_error err){
    *outStream << err.what() << "\n\n";
    errorFlag = -1000;
  };
  
  *outStream \
    << "\n"
    << "===============================================================================\n"\
    << "| TEST 3: correctness of basis function values                                |\n"\
    << "===============================================================================\n";
  
  outStream -> precision(20);
  
  // VALUE: Each row gives the 4 correct basis set values at an evaluation point
  double basisValues[] = {
    1.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    //
    0.0515625,   0.0984375,  0.4265625,  0.2234375,  0.2,
    0.2,         0,          0.,         0.5,        0.3,
    0.025,       0.025,      0.,         0,          0.95,
    0.18,        0.045,      0.005,      0.02,       0.75,
    0.035,       0.015,      0.285,      0.665,      0.,
  };
  
  // GRAD and D1: each row gives the 3 x 5 correct values of the gradients of the 5 basis functions
  double basisGrads[] = {
      -0.5, -0.5,  0.0,  0.5,  0.0, -0.5,  0.0,  0.0,  0.0,  0.0,  0.5, -0.5,  0.0,  0.0,  1.0, \
      -0.5,  0.0, -0.5,  0.5, -0.5,  0.0,  0.0,  0.5, -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  1.0, \
       0.0,  0.0,  0.0,  0.0, -0.5, -0.5,  0.5,  0.5,  0.0, -0.5,  0.0, -0.5,  0.0,  0.0,  1.0, \
       0.0, -0.5, -0.5,  0.0,  0.0,  0.0,  0.5,  0.0, -0.5, -0.5,  0.5,  0.0,  0.0,  0.0,  1.0, \
      -0.25,-0.25,-0.25, 0.25,-0.25,-0.25, 0.25, 0.25,-0.25,-0.25, 0.25,-0.25, 0.0,  0.0,  1.0, \
      -0.09375, -0.171875, -0.201171875,  0.09375, -0.328125, -0.298828125, 0.40625,  0.328125, -0.201171875, -0.40625,  0.171875, -0.298828125,  0.0,  0.0,  1.0, \
      -0.1428571428571429, -0.5, -0.3571428571428571,  0.1428571428571429,  0.0, -0.1428571428571429,  0.3571428571428571, 0.0, -0.3571428571428571, -0.3571428571428571,  0.5, -0.1428571428571429,  0.0,  0.0,  1.0, \
      -0.5, -0.25, -0.25,  0.5, -0.25, -0.25,  0.0,  0.25, -0.25,  0.,  0.25, -0.25, 0.0,  0.0,  1.0, \
      -0.45, -0.4, -0.13,  0.45, -0.1, -0.37,  0.05,  0.1, -0.13, -0.05,  0.4, -0.37,  0.0,  0.0,  1.0, \
      -0.025, -0.35, -0.34,  0.025, -0.15, -0.16,  0.475,  0.15, -0.34, -0.475,  0.35, -0.16,  0.0,  0.0,  1.0
  };


  //D2: flat array with the values of D2 applied to basis functions. Multi-index is (P,F,K)
  const double eps = std::numeric_limits<double>::epsilon( );
  double basisD2[] = {
      0, 0.25,-0.25, 0,-0.25, 0.5, 0,-0.25, 0.25, 0, 0.25,-0.5, 0, 0.25,-0.25, 0,-0.25, 0.5, 0,-0.25, 0.25, 0, 0.25,-0.5, 0, 0, 0, 0, 0, 0, \
      0, 0.25,-0.25, 0, 0.25,-0.5, 0,-0.25, 0.25, 0,-0.25, 0.5, 0, 0.25,-0.25, 0, 0.25,-0.5, 0,-0.25, 0.25, 0,-0.25, 0.5, 0, 0, 0, 0, 0, 0, \
      0, 0.25, 0.25, 0, 0.25, 0.5, 0,-0.25,-0.25, 0,-0.25,-0.5, 0, 0.25, 0.25, 0, 0.25, 0.5, 0,-0.25,-0.25, 0,-0.25,-0.5, 0, 0, 0, 0, 0, 0, \
      0, 0.25, 0.25, 0,-0.25,-0.5, 0,-0.25,-0.25, 0, 0.25, 0.5, 0, 0.25, 0.25, 0,-0.25,-0.5, 0,-0.25,-0.25, 0, 0.25, 0.5, 0, 0, 0, 0, 0, 0, \
      0, 0.25/eps, 0, 0, 0, 0, 0,-0.25/eps, 0, 0, 0, 0, 0, 0.25/eps, 0, 0, 0, 0, 0,-0.25/eps, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, \
      0, 0.3125, 0.1953125, 0, 0.09765625, 0.1220703125, 0,-0.3125,-0.1953125, 0,-0.09765625,-0.1220703125, 0, 0.3125, 0.1953125, 0, 0.09765625, 0.1220703125, 0,-0.3125,-0.1953125, 0,-0.09765625,-0.1220703125, 0, 0, 0, 0, 0, 0, \
      0, 0.3571428571428571, 0.1530612244897959, 0,-0.3571428571428571,-0.306122448979592, 0,-0.3571428571428572,-0.1530612244897959, 0, 0.3571428571428571, 0.306122448979592, 0, 0.3571428571428571, 0.1530612244897959, 0,-0.3571428571428571,-0.306122448979592, 0,-0.3571428571428571,-0.1530612244897959, 0, 0.3571428571428571, 0.306122448979592, 0, 0, 0, 0, 0, 0, \
      0, 5,-5, 0, 0, 0, 0,-5, 5, 0, 0, 0, 0, 5,-5, 0, 0, 0, 0, -5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
      0, 1,-0.8, 0,-0.6, 0.96, 0, -1, 0.8, 0, 0.6,-0.96, 0,1,-0.8, 0, -0.6, 0.96, 0, -1, 0.8, 0, 0.6, -0.96, 0, 0, 0, 0, 0, 0, \
      0, 0.25, 0.225, 0,-0.1,-0.18, 0,-0.25,-0.225, 0, 0.1, 0.18, 0, 0.25, 0.225, 0,-0.1,-0.18, 0,-0.25,-0.225,0,0.1,0.18, 0, 0, 0, 0, 0, 0
  };
  try{
        
    // Dimensions for the output arrays:
    int numFields = pyrBasis.getCardinality();
    int numPoints = pyrNodes.dimension(0);
    int spaceDim  = pyrBasis.getBaseCellTopology().getDimension();
    int D2Cardin  = Intrepid2::getDkCardinality(OPERATOR_D2, spaceDim);
    
    // Generic array for values, grads, curls, etc. that will be properly sized before each call
    FieldContainer<double> vals;
    
    // Check VALUE of basis functions: resize vals to rank-2 container:
    vals.resize(numFields, numPoints);
    pyrBasis.getValues(vals, pyrNodes, OPERATOR_VALUE);
    for (int i = 0; i < numFields; i++) {
      for (int j = 0; j < numPoints; j++) {
          int l =  i + j * numFields;
           if (std::abs(vals(i,j) - basisValues[l]) > INTREPID_TOL) {
             errorFlag++;
             *outStream << std::setw(70) << "^^^^----FAILURE!" << "\n";

             // Output the multi-index of the value where the error is:
             *outStream << " At multi-index { ";
             *outStream << i << " ";*outStream << j << " ";
             *outStream << "}  computed value: " << vals(i,j)
               << " but reference value: " << basisValues[l] << "\n";
         }
      }
    }

    // Check GRAD of basis function: resize vals to rank-3 container
    vals.resize(numFields, numPoints, spaceDim);
    pyrBasis.getValues(vals, pyrNodes, OPERATOR_GRAD);
    for (int i = 0; i < numFields; i++) {
      for (int j = 0; j < numPoints; j++) {
        for (int k = 0; k < spaceDim; k++) {
           int l = k + i * spaceDim + j * spaceDim * numFields;
           if (std::abs(vals(i,j,k) - basisGrads[l]) > INTREPID_TOL) {
             errorFlag++;
             *outStream << std::setw(70) << "^^^^----FAILURE!" << "\n";

             // Output the multi-index of the value where the error is:
             *outStream << " At multi-index { ";
             *outStream << i << " ";*outStream << j << " ";*outStream << k << " ";
             *outStream << "}  computed grad component: " << vals(i,j,k)
               << " but reference grad component: " << basisGrads[l] << "\n";
            }
         }
      }
    }

    // Check D1 of basis function (do not resize vals because it has the correct size: D1 = GRAD)
    pyrBasis.getValues(vals, pyrNodes, OPERATOR_D1);
    for (int i = 0; i < numFields; i++) {
      for (int j = 0; j < numPoints; j++) {
        for (int k = 0; k < spaceDim; k++) {
           int l = k + i * spaceDim + j * spaceDim * numFields;
           if (std::abs(vals(i,j,k) - basisGrads[l]) > INTREPID_TOL) {
             errorFlag++;
             *outStream << std::setw(70) << "^^^^----FAILURE!" << "\n";

             // Output the multi-index of the value where the error is:
             *outStream << " At multi-index { ";
             *outStream << i << " ";*outStream << j << " ";*outStream << k << " ";
             *outStream << "}  computed D1 component: " << vals(i,j,k)
               << " but reference D1 component: " << basisGrads[l] << "\n";
            }
         }
      }
    }

    // Check D2 of basis function
    vals.resize(numFields, numPoints, D2Cardin);    
    pyrBasis.getValues(vals, pyrNodes, OPERATOR_D2);
    for (int i = 0; i < numFields; i++) {
      for (int j = 0; j < numPoints; j++) {
        if(j == 4) continue; //derivatives are singular when z = 1.
        for (int k = 0; k < D2Cardin; k++) {
           int l = k + i * D2Cardin + j * D2Cardin * numFields;
           if (std::abs(vals(i,j,k) - basisD2[l]) > INTREPID_TOL) {
             errorFlag++;
             *outStream << std::setw(70) << "^^^^----FAILURE!" << "\n";

             // Output the multi-index of the value where the error is:
             *outStream << " At multi-index { ";
             *outStream << i << " ";*outStream << j << " ";*outStream << k << " ";
             *outStream << "}  computed D2 component: " << vals(i,j,k)
               << " but reference D2 component: " << basisD2[l] << "\n";
            }
         }
      }
    }
  }
  

  // Catch unexpected errors
  catch (std::logic_error err) {
    *outStream << err.what() << "\n\n";
    errorFlag = -1000;
  };
  
  if (errorFlag != 0)
    std::cout << "End Result: TEST FAILED\n";
  else
    std::cout << "End Result: TEST PASSED\n";
  
  // reset format state of std::cout
  std::cout.copyfmt(oldFormatState);
  Kokkos::finalize();
  return errorFlag;
}
