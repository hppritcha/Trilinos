#include <map>

#include "Panzer_GeometricAggFieldPattern.hpp"

#include "Teuchos_DefaultMpiComm.hpp"

using Teuchos::RCP;

namespace panzer {

// ************************************************************
// class BlockedDOFManager
// ************************************************************

template <typename LocalOrdinalT,typename GlobalOrdinalT>
BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::BlockedDOFManager()
   : fieldsRegistered_(false), maxSubFieldNum_(-1)
{ }

template <typename LocalOrdinalT,typename GlobalOrdinalT>
BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::BlockedDOFManager(const Teuchos::RCP<ConnManager<LocalOrdinalT,GlobalOrdinalT> > & connMngr,MPI_Comm mpiComm)
   : fieldsRegistered_(false), maxSubFieldNum_(-1)
{
   setConnManager(connMngr,mpiComm);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

template <typename LocalOrdinalT,typename GlobalOrdinalT>
int BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::getFieldNum(const std::string & str) const
{
   std::map<std::string,int>::const_iterator itr = fieldStrToNum_.find(str);

   // return based on what was found
   if(itr==fieldStrToNum_.end()) {
      // incorrect field name
      TEST_FOR_EXCEPTION(true,std::logic_error,
                         "BlockedDOFManager::getFieldNum No field with the name \"" + str + "\" has been added");
   }
   else {
      return itr->second;
   }
}

template <typename LocalOrdinalT,typename GlobalOrdinalT>
bool BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::fieldInBlock(const std::string & field,const std::string & block) const
{
   // try  to find element block
   std::map<std::string,std::set<std::string> >::const_iterator fieldsItr = blockIdToFieldStrings_.find(block);
   TEST_FOR_EXCEPTION(fieldsItr==blockIdToFieldStrings_.end(),std::logic_error,
                      "BlockedDOFManager::fieldInBlock could not find the element block \""+block+"\"");

   // find field in element block 
   const std::set<std::string> & fields = fieldsItr->second;
   std::set<std::string>::const_iterator itr = fields.find(field);
   return itr!=fields.end(); 
}

/** Get field numbers associated with a particular element block.
  */
template <typename LocalOrdinalT,typename GlobalOrdinalT>
const std::vector<int> & BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::getBlockFieldNumbers(const std::string & block) const
{
   // try to find element block
   std::map<std::string,std::vector<int> >::const_iterator fieldsItr = blockIdToFieldNumbers_.find(block);
   TEST_FOR_EXCEPTION(fieldsItr==blockIdToFieldNumbers_.end(),std::logic_error,
                      "BlockedDOFManager::getBlockFieldNumbers cannot field elemenet block, has registerFields() been called?");

   return fieldsItr->second;
}

template <typename LocalOrdinalT,typename GlobalOrdinalT>
void BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::getElementGIDs(LocalOrdinalT localElmtId,std::vector<GlobalOrdinal> & gids) const
{
   // TODO
}

template <typename LocalOrdinalT,typename GlobalOrdinalT>
void BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::getElementOrientation(LocalOrdinalT localElmtId,std::vector<double> & gidsOrientation) const
{
   TEST_FOR_EXCEPTION(true,std::logic_error,"BlockedDOFManager::getElementOrientation not implemented yet!");
}

template <typename LocalOrdinalT,typename GlobalOrdinalT>
const std::vector<int> & BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::getGIDFieldOffsets(const std::string & blockId,int fieldNum) const
{
   // TODO
}

template <typename LocalOrdinalT,typename GlobalOrdinalT>
const std::pair<std::vector<int>,std::vector<int> > & 
BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::getGIDFieldOffsets_closure(const std::string & blockId,int fieldNum,int subcellDim,int subcellId) const
{
   // TODO
}

template <typename LocalOrdinalT,typename GlobalOrdinalT>
void BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::getOwnedIndices(std::vector<GlobalOrdinal> & indices) const
{
   // loop over field block manager and grab indices
   for(std::size_t fbm=0;fbm<fieldBlockManagers_.size();fbm++) {
      std::vector<GlobalOrdinalT> fieldBlockOwned;

      fieldBlockManagers_[fbm]->getOwnedIndices(fieldBlockOwned);

      for(std::size_t i=0;i<fieldBlockOwned.size();i++) 
         indices.push_back(std::make_pair(fbm,fieldBlockOwned[i]));
   }
}

template <typename LocalOrdinalT,typename GlobalOrdinalT>
void BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::getOwnedAndSharedIndices(std::vector<GlobalOrdinal> & indices) const
{
   // loop over field block manager and grab indices
   for(std::size_t fbm=0;fbm<fieldBlockManagers_.size();fbm++) {
      std::vector<GlobalOrdinalT> fieldBlockOwned;

      fieldBlockManagers_[fbm]->getOwnedAndSharedIndices(fieldBlockOwned);

      for(std::size_t i=0;i<fieldBlockOwned.size();i++) 
         indices.push_back(std::make_pair(fbm,fieldBlockOwned[i]));
   }
}

template <typename LocalOrdinalT,typename GlobalOrdinalT>
void BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::ownedIndices(const std::vector<GlobalOrdinal> & indices,std::vector<bool> & isOwned) const
{
   // TODO
}


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


/** \brief Set the connection manager and MPI_Comm objects.
  *
  * Set the connection manager and MPI_Comm objects. If this method
  * is called more than once, the behavior is to reset the indices in
  * the DOF manager.  However, the fields will be the same (this assumes
  * that the element blocks are consistent with the fields). The indices
  * will need to be rebuilt by calling <code>buildGlobalUnknowns</code>.
  *
  * \param[in] connMngr Connection manager to use.
  * \param[in] mpiComm  Communicator to use.
  */
template <typename LocalOrdinalT,typename GlobalOrdinalT>
void BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::setConnManager(const Teuchos::RCP<ConnManager<LocalOrdinalT,GlobalOrdinalT> > & connMngr,MPI_Comm mpiComm)
{
   // this kills any old connection manager as well as the old FEI objects
   resetIndices();

   connMngr_ = connMngr;

   mpiComm_ = mpiComm;
   communicator_ = Teuchos::rcp(new Teuchos::MpiComm<int>(Teuchos::opaqueWrapper(mpiComm)));
}

/** \brief Reset the indicies for this DOF manager.
  *
  * This method resets the indices and wipes out internal state. This method
  * does preserve the fields and the patterns added to the object. Also the
  * old connection manager is returned.
  *
  * \returns Old connection manager.
  */
template <typename LocalOrdinalT,typename GlobalOrdinalT>
Teuchos::RCP<ConnManager<LocalOrdinalT,GlobalOrdinalT> > BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::resetIndices()
{
   Teuchos::RCP<ConnManager<LocalOrdinalT,GlobalOrdinalT> > connMngr = connMngr_;

   connMngr_ = Teuchos::null;
   ownedGIDHashTable_.clear(); 

   // TODO: Loop over sub dofmanagers wiping them out as well

   return connMngr;
}

template <typename LocalOrdinalT,typename GlobalOrdinalT>
void BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::addField(const std::string & str,
                                                        const Teuchos::RCP<const FieldPattern> & pattern)
{
   std::vector<std::string> elementBlockIds;
   connMngr_->getElementBlockIds(elementBlockIds);

   // loop over blocks adding field pattern to each 
   for(std::size_t i=0;i<elementBlockIds.size();i++)
      addField(elementBlockIds[i],str,pattern);
}

template <typename LocalOrdinalT,typename GlobalOrdinalT>
void BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::addField(const std::string & blockId,const std::string & str,
                                                        const Teuchos::RCP<const FieldPattern> & pattern)
{
   TEST_FOR_EXCEPTION(fieldsRegistered(),std::logic_error,
                      "BlockedDOFManager::addField: addField cannot be called after registerFields or"
                      "buildGlobalUnknowns has been called"); 

   fieldStringToPattern_[std::make_pair(blockId,str)] = pattern;
   blockIdToFieldStrings_[blockId].insert(str);
}

template <typename LocalOrdinalT,typename GlobalOrdinalT>
void BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::registerFields() 
{
   fieldBlockManagers_.clear();
   fieldStrToNum_.clear();
   fieldNumToFieldBlk_.clear();
   maxSubFieldNum_ = -1;
   
   fieldsRegistered_ = false;

   // test validity of the field order, build default if none is provided
   {
      // build a unique set of fields, so we can compare validate the ordered list
      std::set<std::string> fields;
      for(std::map<std::pair<std::string,std::string>,Teuchos::RCP<const FieldPattern> >::const_iterator
          fieldItr=fieldStringToPattern_.begin(); fieldItr!=fieldStringToPattern_.end();++fieldItr) {
         std::string fieldName = fieldItr->first.second;
         fields.insert(fieldName);
      }

      // construct default field order if neccessary
      if(fieldOrder_.size()==0) {
         std::set<std::string>::const_iterator itr;
         for(itr=fields.begin();itr!=fields.end();itr++) {
            std::vector<std::string> block;
            block.push_back(*itr);
            fieldOrder_.push_back(block);
         }
      }

      // check validity of field order: no repeats, and everything is accounted for
      bool validOrder = validFieldOrder(fieldOrder_,fields);
      if(!validOrder) {
         // for outputing
         std::stringstream ss;

         ss << "BlockedDOFManager::registerFields - Field order is invalid!\n";

         ss << "   fields = [ ";
         for(std::set<std::string>::const_iterator itr=fields.begin();
             itr!=fields.end();++itr)
            ss << "\"" << *itr << "\" ";
         ss << " ]\n";

         ss << "   fieldOrder = [ ";
         for(std::vector<std::vector<std::string> >::const_iterator bitr=fieldOrder_.begin();
             bitr!=fieldOrder_.end();++bitr) {
            ss << "[ ";
            for(std::vector<std::string>::const_iterator itr=bitr->begin();
                itr!=bitr->end();++itr) {
               ss << "\"" << *itr << "\" ";
            }
            ss << " ], ";
         }
         ss << " ]\n";

         TEST_FOR_EXCEPTION(!validOrder,std::logic_error,ss.str());
      }
   }

   // build sub DOFManagers for each field block
   for(std::size_t fldBlk=0;fldBlk<fieldOrder_.size();fldBlk++) {
      Teuchos::RCP<panzer::DOFManager<LocalOrdinalT,GlobalOrdinalT> > dofManager =
         Teuchos::rcp(new panzer::DOFManager<LocalOrdinalT,GlobalOrdinalT>);
      dofManager->setConnManager(getConnManager(),mpiComm_);

      // add in these fields to the new manager
      this->addFieldsToFieldBlockManager(fieldOrder_[fldBlk],*dofManager);

      fieldBlockManagers_.push_back(dofManager); 
   }
    
   ////////////////////////////////
   // build field numbers: two stage algorithm

   // 1st Stage: Extract field numbers used by each sub DOFManager.
   //            determine largest of these
   //
   // - note at this point since "validFieldOrder" has
   //   been called we are gurranteed to not have repeated fields
   maxSubFieldNum_ = -1;
   std::map<std::string,int> tempStrToNum;
   for(std::size_t fldBlk=0;fldBlk<fieldBlockManagers_.size();fldBlk++) {
      Teuchos::RCP<panzer::DOFManager<LocalOrdinalT,GlobalOrdinalT> > dofManager =
         fieldBlockManagers_[fldBlk];
      const std::vector<std::string> & activeFields = fieldOrder_[fldBlk];

      int fieldNum = 0;
      for(std::size_t f=0;f<activeFields.size();f++) {
         fieldNum = dofManager->getFieldNum(activeFields[f]); 
         tempStrToNum[activeFields[f]] = fieldNum;

         maxSubFieldNum_ = (fieldNum>maxSubFieldNum_) ? fieldNum : maxSubFieldNum_;
      }
   }

   // 2nd Stage: Using field block index, field number and largest field number
   //            build a up fieldStrToNum_ map and fieldNumToFieldBlk_
   int numOffset = 0;
   for(std::size_t fldBlk=0;fldBlk<fieldBlockManagers_.size();fldBlk++) {
      const std::vector<std::string> & activeFields = fieldOrder_[fldBlk];
      for(std::size_t f=0;f<activeFields.size();f++) {
         // compute offset field number
         int fieldNum = tempStrToNum[activeFields[f]]+numOffset;

         // build up map data
         fieldStrToNum_[activeFields[f]] = fieldNum;
         fieldNumToFieldBlk_[fieldNum] = fldBlk; 
      }

      // increament field number offset based on largest sub field number
      numOffset += (maxSubFieldNum_+1);
   }

   // end build field numbers 
   ////////////////////////////////

   // build block to field numbers: this requires field numbers have been built
   // and that "getFieldNum" behaves correctly
   for(std::map<std::string,std::set<std::string> >::const_iterator itr=blockIdToFieldStrings_.begin();
       itr!=blockIdToFieldStrings_.end();++itr) {
      const std::set<std::string> & fields = itr->second;

      std::vector<int> & fieldNums = blockIdToFieldNumbers_[itr->first];
      for(std::set<std::string>::const_iterator fldItr=fields.begin();  
          fldItr!=fields.end();++fldItr) {
         fieldNums.push_back(getFieldNum(*fldItr));
      }
   }

   // everything completed, mark as fields registered
   fieldsRegistered_ = true;
}

template <typename LocalOrdinalT,typename GlobalOrdinalT>
void BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::
addFieldsToFieldBlockManager(const std::vector<std::string> & activeFields,
                             DOFManager<LocalOrdinalT,GlobalOrdinalT> & fieldBlockManager) const
{
   std::vector<std::size_t> correctnessCheck(activeFields.size(),0);
   std::vector<std::string> elementBlocks;
   this->getElementBlockIds(elementBlocks);

   // loop over element blocks adding each field in this element block and this field block
   for(std::size_t eb=0;eb<elementBlocks.size();eb++) {
      std::string elementBlock = elementBlocks[eb];

      // loop over active fields extracting those that are associated with this element block
      for(std::size_t f=0;f<activeFields.size();f++) {
         std::string fieldName = activeFields[f];
         Teuchos::RCP<const FieldPattern> fp = this->getFieldPattern(elementBlock,fieldName);

         if(fp!=Teuchos::null) {
            fieldBlockManager.addField(elementBlock,fieldName,fp);
            correctnessCheck[f] = 1; // all active fields should be placed in DOFManager
         }
      }
   }

   // verify correctness check
   std::size_t correctFlag = std::accumulate(correctnessCheck.begin(),correctnessCheck.end(),0);
   TEST_FOR_EXCEPTION(correctFlag!=activeFields.size(),std::logic_error,
                      "BlockedDOFManager::addFieldsToFieldBlockManager detected inconsistincies in the active fields.");

   // register added fields
   fieldBlockManager.registerFields();
}

template <typename LocalOrdinalT,typename GlobalOrdinalT>
void BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::setFieldOrder(const std::vector<std::vector<std::string> > & fieldOrder)
{
   fieldOrder_ = fieldOrder;
}

/** Get the field order used. Return the field strings.
  */
template <typename LocalOrdinalT,typename GlobalOrdinalT>
void BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::getFieldOrder(std::vector<std::vector<std::string> > & fieldOrder) const
{
   fieldOrder = fieldOrder_;
}

template <typename LocalOrdinalT,typename GlobalOrdinalT>
int BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::getNumFields() const
{
   if(fieldsRegistered())
      return fieldStrToNum_.size();

   // more work needs to be done if the fields have not yet been registered
   // pull it from the (block id x field name) ==> pattern map
   std::set<std::string> fields;
   std::map<std::pair<std::string,std::string>,Teuchos::RCP<const FieldPattern> >::const_iterator itr;
   for(itr=fieldStringToPattern_.begin();itr!=fieldStringToPattern_.end();++itr)
      fields.insert(itr->first.second);

   return fields.size();
}

// build the global unknown numberings
//   1. this builds the pattens
//   2. initializes the connectivity
//   3. calls initComplete
template <typename LocalOrdinalT,typename GlobalOrdinalT>
void BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::buildGlobalUnknowns(const Teuchos::RCP<const FieldPattern> & geomPattern)
{
   if(!fieldsRegistered()) {
      std::cout << "register fields in" << std::endl;
      registerFields();
   }

   // save the geometry pattern
   geomPattern_ = geomPattern;

   // build global unknowns for each field block
   for(std::size_t fb=0;fb<fieldBlockManagers_.size();fb++) {
      std::cout << "building field block fb = " << fb << std::endl;
      fieldBlockManagers_[fb]->buildGlobalUnknowns(geomPattern_);
   }
}

// build the global unknown numberings
//   1. this builds the pattens
//   2. initializes the connectivity
//   3. calls initComplete
template <typename LocalOrdinalT,typename GlobalOrdinalT>
void BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::buildGlobalUnknowns()
{
   if(!fieldsRegistered()) {
      std::cout << "register fields out" << std::endl;
      registerFields();
   }

   std::cout << "Build geometric layout" << std::endl;

   // build the pattern for the ID layout on the mesh
   std::vector<RCP<const FieldPattern> > patVector;
   RCP<GeometricAggFieldPattern> aggFieldPattern = Teuchos::rcp(new GeometricAggFieldPattern);;
   std::map<std::pair<std::string,std::string>,Teuchos::RCP<const FieldPattern> >::iterator f2p_itr;
   for(f2p_itr=fieldStringToPattern_.begin();f2p_itr!=fieldStringToPattern_.end();f2p_itr++)
      patVector.push_back(f2p_itr->second);
   aggFieldPattern->buildPattern(patVector);

   std::cout << "Build connectivity" << std::endl;

   // setup connectivity mesh
   connMngr_->buildConnectivity(*aggFieldPattern);

   std::cout << "Build sub unknowns" << std::endl;

   // using new geometric pattern, build global unknowns
   buildGlobalUnknowns(aggFieldPattern);
}

template <typename LocalOrdinalT,typename GlobalOrdinalT>
void BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::printFieldInformation(std::ostream & os) const
{
   os << "BlockedDOFManager Field Information: " << std::endl;
   
   if(fieldsRegistered()) {
      // TODO
   }
   else {
      // fields are not registered
      os << "Fields not yet registered! Unknowns not built (call registerFields or buildGlobalUnknowns)" << std::endl;
   }
}

template <typename LocalOrdinalT,typename GlobalOrdinalT>
Teuchos::RCP<const FieldPattern> 
BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::getFieldPattern(const std::string & blockId, const std::string & fieldName) const
{
   std::map<std::pair<std::string,std::string>,Teuchos::RCP<const FieldPattern> >::const_iterator itr;
   itr = fieldStringToPattern_.find(std::make_pair(blockId,fieldName));

   if(itr==fieldStringToPattern_.end()) // not found
      return Teuchos::null;
   else // found
      return itr->second;
}

/** Check the validity of a field order. This is used internally
  * as a sanity check. Checks for no repeats, bogus fields, and all fields
  * being included.
  *
  * \param[in] fieldOrder_ut Field order vector under test (ut).
  *
  * \returns true if the vector is valid, false otherwise.
  */
template <typename LocalOrdinalT,typename GlobalOrdinalT>
bool BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::validFieldOrder(const std::vector<std::vector<std::string> > & fieldOrder_ut,
                                                                      const std::set<std::string> & fields) const
{
   std::set<std::string> orderedFields;
   std::size_t numberInOrder = 0;

   for(std::size_t b=0;b<fieldOrder_ut.size();b++) {
      numberInOrder += fieldOrder_ut[b].size();
      orderedFields.insert(fieldOrder_ut[b].begin(),
                           fieldOrder_ut[b].end());
   }

   bool correctCount = (numberInOrder==fields.size());
   bool sameFields = (orderedFields==fields);
 
   return correctCount && sameFields;
}

template <typename LocalOrdinalT,typename GlobalOrdinalT>
int BlockedDOFManager<LocalOrdinalT,GlobalOrdinalT>::getNumFieldBlocks() const
{
   if(fieldOrder_.size()==0) 
      return 1; // only one field block
   return fieldOrder_.size();
}

///////////////////////////////////////////////////////////////////////////

}
