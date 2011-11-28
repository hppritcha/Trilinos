#include "Xpetra_Operator.hpp"
#include "MueLu_EpetraOperator.hpp"
#include "MueLu_Level.hpp"
#include "MueLu_Utilities.hpp"

namespace MueLu {

int EpetraOperator::SetUseTranspose(bool UseTransposeBool) { return -1; }

int EpetraOperator::Apply(const Epetra_MultiVector& X, Epetra_MultiVector& Y) const { return -1; }

int EpetraOperator::ApplyInverse(const Epetra_MultiVector& X, Epetra_MultiVector& Y) const {

  try {

    Epetra_MultiVector & temp_x = const_cast<Epetra_MultiVector &>(X);

    const Xpetra::EpetraMultiVector tX(rcpFromRef(temp_x));
    Xpetra::EpetraMultiVector       tY(rcpFromRef(Y));

    // check if X and Y points to the same memory
    if(X.Values() == Y.Values()) {
      // For AztecOO X and Y point to the same memory
      // use work vectors

      // reserve memory for deep copy vectors
      RCP<Xpetra::EpetraMultiVector> epX = Teuchos::rcp(new EpetraMultiVector(tX.getMap(), tX.getNumVectors())); // oops, we don't have a copy constructor?
      RCP<Xpetra::EpetraMultiVector> epY = Teuchos::rcp(new EpetraMultiVector(tY.getMap(), tY.getNumVectors()));

      // deep copy of RHS vector
      epX->update(1.0,tX,0.0);

      //FIXME InitialGuessIsZero currently does nothing in MueLu::Hierarchy.Iterate().
      epY->putScalar(0.0);

      // apply one V/W-cycle as preconditioner
      Hierarchy_->Iterate(*epX, 1, *epY, true);

      // deep copy solution from MueLu to AztecOO
      tY.update(1.0,*epY,0.0);
    } else {
      // X and Y point to different memory
      // avoid additonal working vectors, just pass through the vectors

      //FIXME InitialGuessIsZero currently does nothing in MueLu::Hierarchy.Iterate().
      tY.putScalar(0.0);
      Hierarchy_->Iterate(tX, 1, tY, true);
    }
  } catch(std::exception& e) {
    //TODO: error msg directly on std::cerr?
    std::cerr << "Caught an exception in MueLu::EpetraOperator::ApplyInverse():" << std::endl
        << e.what() << std::endl;
    return -1;
  }

  return 0;
}

double EpetraOperator::NormInf() const { return 0; }

const char * EpetraOperator::Label() const { return "MueLu::Hierarchy"; }

bool EpetraOperator::UseTranspose() const { return false; }

bool EpetraOperator::HasNormInf() const { return false; }

const Epetra_Comm & EpetraOperator::Comm() const {
  RCP<Operator> A = Hierarchy_->GetLevel(0)->Get<RCP<Operator> >("A");
  RCP<Epetra_CrsMatrix> epA = Utils::Op2NonConstEpetraCrs(A);
  return epA->Comm();
}

const Epetra_Map & EpetraOperator::OperatorDomainMap() const {
  RCP<Operator> A = Hierarchy_->GetLevel(0)->Get<RCP<Operator> >("A");
  RCP<Epetra_CrsMatrix> epA = Utils::Op2NonConstEpetraCrs(A);
  return epA->DomainMap();
}

const Epetra_Map & EpetraOperator::OperatorRangeMap() const {
  RCP<Operator> A = Hierarchy_->GetLevel(0)->Get<RCP<Operator> >("A");
  RCP<Epetra_CrsMatrix> epA = Utils::Op2NonConstEpetraCrs(A);
  return epA->RangeMap();
}

} // namespace
