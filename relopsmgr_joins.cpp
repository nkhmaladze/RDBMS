#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <string.h>
#include <math.h>
#include <mutex>
#include <unordered_set>
#include "filemgr.h"
#include "catalog.h"
#include "swatdb_types.h"
#include "file.h"
#include "relopsmgr.h"
#include "swatdb_exceptions.h"
#include "heapfile.h"
#include "hashindexfile.h"
#include "heapfilescanner.h"
#include "blockheapfilescanner.h"
#include "hashindexscanner.h"
#include "record.h"
#include "relationfile.h"
#include "heapfile.h"
#include "heappage.h"
#include "heappagescanner.h"
#include "bufmgr.h"
#include "data.h"
#include "schema.h"
#include "key.h"
#include "searchkeyformat.h"
#include "operation.h"
#include "select.h"
#include "filescan.h"
#include "indexscan.h"
#include "join.h"
#include "tupleNLJ.h"
#include "indexNLJ.h"
#include "blockNLJ.h"
#include "hashjoin.h"
#include "parallelHashJoin.h"
#include "project.h"
#include "testingconfig.h"

/**
 * SwatDB RelOpsManager Class.
 * The interface to the relational operators of the system:
 * manages relational operations on files.
 * This file contains the join interface implementation.
 */


/**
 * @brief Runs the Join operation using the type of join given by the 
 *    argument jtype. This overloaded function only runs TupleNLJ and 
 *    BLockNLJ, the others are implemented in the other overloaded versions
 *
 * @pre Input parameters o_fid and i_fid are valid relation ids 
 *
 * @param jtype. JoinType indicating type of join (tuplenlj, blocknlj, etc)
 * @param o_fid. FileId of the outer relation of the join. 
 * @param i_fid. FileId of the inner relation of the join. 
 * @param outer_field_ids. Vector of field ids for the outer relation.
 *                         (the join fields only)
 * @param inner_field_ids. Vector of field ids for the inner relation.
 *                         (the join fields only)
 * @param block_size. Uint32 indicating block size if block nlj is run, 
 *    and inner_index_id if index nlj is run.
 * ***NOTE: block_size is a std::uint32_t, which is the same type as FileId, so
 *          for index nested loop join, the block_size will be used to hold the
 *          index file id.
 * 
 * @throw MismatchingFieldsRelOpsManager if the fields parameters 
 *        don't match in number and type, or if there are 
 *        invalid field Ids or are empty
 *
 * @return HeapFile * of the result file with results of the join.
 *
 */ 
HeapFile *RelOpsManager::join(JoinType jtype, FileId o_fid, FileId i_fid, 
               std::vector<FieldId> outer_field_ids, 
               std::vector<FieldId> inner_field_ids,  
               std::uint32_t block_size){

  FileId res_id = this->_createJoinRes(o_fid, i_fid, outer_field_ids, 
      inner_field_ids);

  // Tuple Nested Loop Join
  switch(jtype) {
    case TupleNLJT: {
      TupleNLJ *tuple = new TupleNLJ(o_fid, i_fid, res_id, outer_field_ids, 
          inner_field_ids, this->catalog);
      tuple->runOperation();
      delete tuple;
      break;
    }
    // Block Nested Loop Join
    case BlockNLJT: {
      BlockNLJ *block = new BlockNLJ(o_fid, i_fid, res_id, outer_field_ids, 
          inner_field_ids, block_size, this->catalog);
      block->runOperation();
      delete block;
      break;
    }
    // Index Nested Loop Join 
    case IndexNLJT: {
      FileId inner_index_id = block_size;
      IndexNLJ *index = new IndexNLJ(o_fid, i_fid, inner_index_id, 
          res_id, outer_field_ids, 
          inner_field_ids, this->catalog);
      index->runOperation();
      delete index;
      break;
    }
    default: throw;

  }
  return ((HeapFile *)this->catalog->getFile(res_id));
}


/**
 * @brief RUns the Join operation using the type of join given by the 
 *    argument jtype. This overloaded function only runs HashJoin and 
 *    ParallelHashJoin, the others are implemented in the other 
 *    overloaded versions
 *
 * @pre Input parameters o_fid and i_fid are valid relation ids 
 *
 * @param jtype. JoinType indicating type of join (tuplenlj, blocknlj, etc)
 * @param o_fid. FileId of the outer relation of the join. 
 * @param i_fid. FileId of the inner relation of the join. 
 * @param outer_field_ids. Vector of field ids for the outer relation.
 * @param inner_field_ids. Vector of field ids for the inner relation.
 * @param num_buckets. Number of buckets used for hash joins..
 * @param num_threads. Number of threads for parallel hash join.
 */ 
HeapFile *RelOpsManager::join(JoinType jtype, FileId o_fid, FileId i_fid, 
               std::uint32_t num_buckets, std::vector<FieldId> outer_field_ids, 
               std::vector<FieldId> inner_field_ids, 
               std::uint32_t num_threads){

  
  FileId res_id = this->_createJoinRes(o_fid, i_fid, outer_field_ids, 
      inner_field_ids);

  switch(jtype) {
    case SeqHashJoinT: {
      HashJoin *hj = new HashJoin(o_fid, i_fid, res_id, outer_field_ids, 
          inner_field_ids, num_buckets, "/local/", this->catalog, 
          this->buf_mgr, this->file_mgr);
      try {
        hj->runOperation();
      } catch (const InsufficientSpaceHeapPage& e) {
        std::cout << "Insufficient space in HeapPage bucket " <<
          "during HashJoin. During the second hash, buckets aren't"<<
          " evenly spread enough. Try adding more buckets.";
        throw;
      }
      delete hj;
      break;
    }
    case ParHashJoinT: {
      ParallelHashJoin *hj = new ParallelHashJoin(o_fid, i_fid, res_id, 
          outer_field_ids, inner_field_ids, num_buckets, num_threads, 
          "/local/", this->catalog, this->buf_mgr, this->file_mgr);
      hj->runOperation();
      delete hj;
      break;
    }
    default: throw;
  }
  return ((HeapFile *)this->catalog->getFile(res_id));
}


/**
 * Creates a result file with the schema of the joined relations 
 *
 *   The schema for the result relation is the combination of the 
 *   outer file schema followed by the inner file schema without 
 *   the join fields duplicated (it should be removed from the second
 *   (inner) relation's schema and kept in the part from the outer 
 *   relation's.)
 *
 *   The primary key of the result is the primary key of the 
 *   outer relation. 
 *
 *   The field names of the result schema need to be unique.  As a result,
 *   if the inner relation has fields with the same name as the outer 
 *   relation, then those result schema field names are prefixed
 *   with the inner relation name to make them unique (and only in this 
 *   case are they prefixed with the inner relation's name)
 *
 *   after setting up schema for result relation, call the
 *   _createResultFile method to create the result relation file 
 *
 *   @param outer_fid. FileId of the outer relation of the join.
 *   @param inner_fid. FileId of the inner relation of the join.
 *   @param o_fields. Vector of field ids for the outer relation.
 *                         (the join fields only)
 *   @param i_fields. Vector of field ids for the inner relation.
 *                         (the join fields only)
 *
 *   @throw MismatchingFieldsRelOpsManager
 *   @return FileId of the HeapFile created for the result of the join.
 */
FileId RelOpsManager::_createJoinRes(FileId outer_fid, FileId inner_fid, 
                      std::vector<FieldId> o_fields, 
                      std::vector<FieldId> i_fields) {
                      
  std::vector<FieldEntry> new_field_list;
  FieldEntry fentry;
  FileId res_id = INVALID_FILE_ID;
  Schema *outer_schema = NULL, *inner_schema = NULL, *result_schema = NULL;
  std::string inner_name;
  std::unordered_set<std::string> seen_names;
  std::unordered_set<FieldId> inner_set_ids(i_fields.begin(), i_fields.end());

  try{
    outer_schema = this->catalog->getSchema(outer_fid);
  }catch(InvalidFileIdCat &e){
    throw;
  }
  try{
    inner_schema = this->catalog->getSchema(inner_fid);
  }catch(InvalidFileIdCat &e){
    throw;
  }
  
  // outer-layer relations copy
  for( FieldEntry &fe : outer_schema->field_list ){
    new_field_list.push_back( fe );
    seen_names.insert( fe.field_name );
  }
  // handles sizing issue
  if(o_fields.size() != i_fields.size()){
    throw MismatchingFieldsRelOpsManager();
  }
  // checks fieldtype are the same
  for (FieldId i=0; i<o_fields.size(); i++){
    if(outer_schema->getFieldType(o_fields[i]) != inner_schema->getFieldType(i_fields[i])){
      throw MismatchingFieldsRelOpsManager();
    }
  }

  // this extracts just the name of the inner file from its whole
  // pathname (remove path and .rel suffix).  Used to prefix any 
  // result field names from the inner relation that match outer names
  inner_name = this->catalog->getFileName(inner_fid);
  inner_name = inner_name.substr(inner_name.find_last_of("/")+1);
  inner_name = inner_name.substr(0, inner_name.length()-4);

  for( FieldId fid = 0; fid < inner_schema->field_list.size(); fid++ ){
    // skip duplicate join column
    if( inner_set_ids.count( fid ) == 1 ) continue;
    
    FieldEntry fe = inner_schema->field_list[fid];

    if( seen_names.count( fe.field_name ) == 1 )
        fe.field_name = inner_name + "." + fe.field_name;

    seen_names.insert( fe.field_name );
    new_field_list.push_back( fe );

  }

  std::vector<std::string> ans_pk;
  for( FileId fid : outer_schema->primary_key ) 
    ans_pk.push_back( outer_schema->field_list[fid].field_name ); 

  Schema *ans = new Schema( new_field_list, ans_pk );
  res_id = this->_createResultFile( ans );

  return res_id;
}