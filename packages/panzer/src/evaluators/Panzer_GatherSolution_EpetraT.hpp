#include "Teuchos_TestForException.hpp"
#include "Phalanx_DataLayout.hpp"

#include "Panzer_UniqueGlobalIndexer.hpp"
#include "Panzer_Basis.hpp"
#include "Panzer_EpetraLinearObjContainer.hpp"

#include "Teuchos_FancyOStream.hpp"

#include "Epetra_Vector.h"
#include "Epetra_Map.h"

// **********************************************************************
// Specialization: Residual
// **********************************************************************

template<typename Traits,typename LO,typename GO>
panzer::GatherSolution_Epetra<panzer::Traits::Residual, Traits,LO,GO>::
GatherSolution_Epetra(
  const Teuchos::RCP<const panzer::UniqueGlobalIndexer<LO,GO> > & indexer,
  const Teuchos::ParameterList& p)
  : globalIndexer_(indexer),
    useTimeDerivativeSolutionVector_(false)
{ 
  const std::vector<std::string>& names = 
    *(p.get< Teuchos::RCP< std::vector<std::string> > >("DOF Names"));

  indexerNames_ = p.get< Teuchos::RCP< std::vector<std::string> > >("Indexer Names");

  Teuchos::RCP<panzer::Basis> basis = 
    p.get< Teuchos::RCP<panzer::Basis> >("Basis");

  gatherFields_.resize(names.size());
  for (std::size_t fd = 0; fd < names.size(); ++fd) {
    gatherFields_[fd] = 
      PHX::MDField<ScalarT,Cell,NODE>(names[fd],basis->functional);
    this->addEvaluatedField(gatherFields_[fd]);
  }

  if (p.isType<bool>("Use Time Derivative Solution Vector"))
    useTimeDerivativeSolutionVector_ = p.get<bool>("Use Time Derivative Solution Vector");

  this->setName("Gather Solution");
}

// **********************************************************************
template<typename Traits,typename LO,typename GO> 
void panzer::GatherSolution_Epetra<panzer::Traits::Residual, Traits,LO,GO>::
postRegistrationSetup(typename Traits::SetupData d, 
		      PHX::FieldManager<Traits>& fm)
{
  // globalIndexer_ = d.globalIndexer_;
  TEUCHOS_ASSERT(gatherFields_.size() == indexerNames_->size());

  fieldIds_.resize(gatherFields_.size());

  for (std::size_t fd = 0; fd < gatherFields_.size(); ++fd) {
    // get field ID from DOF manager
    //std::string fieldName = gatherFields_[fd].fieldTag().name();
    const std::string& fieldName = (*indexerNames_)[fd];
    fieldIds_[fd] = globalIndexer_->getFieldNum(fieldName);

    // setup the field data object
    this->utils.setFieldData(gatherFields_[fd],fm);
  }

  indexerNames_ = Teuchos::null;  // Don't need this anymore
}

// **********************************************************************
template<typename Traits,typename LO,typename GO>
void panzer::GatherSolution_Epetra<panzer::Traits::Residual, Traits,LO,GO>::
evaluateFields(typename Traits::EvalData workset)
{ 
   std::vector<GO> GIDs;
   std::vector<int> LIDs;
 
   // for convenience pull out some objects from workset
   std::string blockId = workset.block_id;
   const std::vector<std::size_t> & localCellIds = workset.cell_local_ids;

   Teuchos::RCP<panzer::EpetraLinearObjContainer> epetraContainer 
         = Teuchos::rcp_dynamic_cast<panzer::EpetraLinearObjContainer>(workset.ghostedLinContainer);
   Teuchos::RCP<Epetra_Vector> x;
   if (useTimeDerivativeSolutionVector_)
     x = epetraContainer->dxdt;
   else
     x = epetraContainer->x; 
 
   // NOTE: A reordering of these loops will likely improve performance
   //       The "getGIDFieldOffsets may be expensive.  However the
   //       "getElementGIDs" can be cheaper. However the lookup for LIDs
   //       may be more expensive!
 
   // gather operation for each cell in workset
   for(std::size_t worksetCellIndex=0;worksetCellIndex<localCellIds.size();++worksetCellIndex) {
      std::size_t cellLocalId = localCellIds[worksetCellIndex];
 
      globalIndexer_->getElementGIDs(cellLocalId,GIDs); 
 
      // caculate the local IDs for this element
      LIDs.resize(GIDs.size());
      for(std::size_t i=0;i<GIDs.size();i++)
         LIDs[i] = x->Map().LID(GIDs[i]);
 
      // loop over the fields to be gathered
      for (std::size_t fieldIndex=0; fieldIndex<gatherFields_.size();fieldIndex++) {
         int fieldNum = fieldIds_[fieldIndex];
         const std::vector<int> & elmtOffset = globalIndexer_->getGIDFieldOffsets(blockId,fieldNum);
 
         // loop over basis functions and fill the fields
         for(std::size_t basis=0;basis<elmtOffset.size();basis++) {
            int offset = elmtOffset[basis];
            int lid = LIDs[offset];
            (gatherFields_[fieldIndex])(worksetCellIndex,basis) = (*x)[lid];
         }
      }
   }
}

// **********************************************************************
// Specialization: Jacobian
// **********************************************************************

template<typename Traits,typename LO,typename GO>
panzer::GatherSolution_Epetra<panzer::Traits::Jacobian, Traits,LO,GO>::
GatherSolution_Epetra(
  const Teuchos::RCP<const panzer::UniqueGlobalIndexer<LO,GO> > & indexer,
  const Teuchos::ParameterList& p)
  : globalIndexer_(indexer),
    useTimeDerivativeSolutionVector_(false)
{ 
  const std::vector<std::string>& names = 
    *(p.get< Teuchos::RCP< std::vector<std::string> > >("DOF Names"));

  indexerNames_ = p.get< Teuchos::RCP< std::vector<std::string> > >("Indexer Names");

  Teuchos::RCP<PHX::DataLayout> dl = 
    p.get< Teuchos::RCP<panzer::Basis> >("Basis")->functional;

  gatherFields_.resize(names.size());
  for (std::size_t fd = 0; fd < names.size(); ++fd) {
    PHX::MDField<ScalarT,Cell,NODE> f(names[fd],dl);
    gatherFields_[fd] = f;
    this->addEvaluatedField(gatherFields_[fd]);
  }

  if (p.isType<bool>("Use Time Derivative Solution Vector"))
    useTimeDerivativeSolutionVector_ = p.get<bool>("Use Time Derivative Solution Vector");

  this->setName("Gather Solution");
}

// **********************************************************************
template<typename Traits,typename LO,typename GO> 
void panzer::GatherSolution_Epetra<panzer::Traits::Jacobian, Traits,LO,GO>::
postRegistrationSetup(typename Traits::SetupData d, 
		      PHX::FieldManager<Traits>& fm)
{
  // globalIndexer_ = d.globalIndexer_;
  TEUCHOS_ASSERT(gatherFields_.size() == indexerNames_->size());

  fieldIds_.resize(gatherFields_.size());

  for (std::size_t fd = 0; fd < gatherFields_.size(); ++fd) {
    // get field ID from DOF manager
    //std::string fieldName = gatherFields_[fd].fieldTag().name();
    const std::string& fieldName = (*indexerNames_)[fd];
    fieldIds_[fd] = globalIndexer_->getFieldNum(fieldName);

    // setup the field data object
    this->utils.setFieldData(gatherFields_[fd],fm);
  }

  indexerNames_ = Teuchos::null;  // Don't need this anymore
}

// **********************************************************************
template<typename Traits,typename LO,typename GO>
void panzer::GatherSolution_Epetra<panzer::Traits::Jacobian, Traits,LO,GO>::
evaluateFields(typename Traits::EvalData workset)
{ 
   std::vector<GO> GIDs;
   std::vector<int> LIDs;

   // for convenience pull out some objects from workset
   std::string blockId = workset.block_id;
   const std::vector<std::size_t> & localCellIds = workset.cell_local_ids;

   Teuchos::RCP<EpetraLinearObjContainer> epetraContainer 
         = Teuchos::rcp_dynamic_cast<EpetraLinearObjContainer>(workset.ghostedLinContainer);
   Teuchos::RCP<Epetra_Vector> x;
   double seed_value = 0.0;
   if (useTimeDerivativeSolutionVector_) {
     x = epetraContainer->dxdt;
     seed_value = workset.alpha;
   }
   else {
     x = epetraContainer->x;
     seed_value = workset.beta;
   }

   // NOTE: A reordering of these loops will likely improve performance
   //       The "getGIDFieldOffsets may be expensive.  However the
   //       "getElementGIDs" can be cheaper. However the lookup for LIDs
   //       may be more expensive!

   // gather operation for each cell in workset
   for(std::size_t worksetCellIndex=0;worksetCellIndex<localCellIds.size();++worksetCellIndex) {
      std::size_t cellLocalId = localCellIds[worksetCellIndex];

      globalIndexer_->getElementGIDs(cellLocalId,GIDs); 

      // caculate the local IDs for this element
      LIDs.resize(GIDs.size());
      for(std::size_t i=0;i<GIDs.size();i++)
        LIDs[i] = x->Map().LID(GIDs[i]);

      // loop over the fields to be gathered
      for(std::size_t fieldIndex=0;
          fieldIndex<gatherFields_.size();fieldIndex++) {
         int fieldNum = fieldIds_[fieldIndex];
         const std::vector<int> & elmtOffset = globalIndexer_->getGIDFieldOffsets(blockId,fieldNum);

         // loop over basis functions and fill the fields
         for(std::size_t basis=0;basis<elmtOffset.size();basis++) {
            int offset = elmtOffset[basis];
            int lid = LIDs[offset];

            // set the value and seed the FAD object
            (gatherFields_[fieldIndex])(worksetCellIndex,basis) = Sacado::Fad::DFad<double>(GIDs.size(), (*x)[lid]);
            (gatherFields_[fieldIndex])(worksetCellIndex,basis).fastAccessDx(offset) = seed_value;
         }
      }
   }
}

// **********************************************************************
