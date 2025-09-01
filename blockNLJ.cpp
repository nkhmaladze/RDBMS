#include <string>                                                               
#include <vector>                                                               
#include "swatdb_types.h"                                                       
#include "filescan.h"                                                           
#include "key.h"                                                                
#include "record.h"                                                             
#include "heapfile.h"                                                           
#include "heapfilescanner.h"                                                    
#include "blockheapfilescanner.h"
#include "searchkeyformat.h"                                                    
#include "blockNLJ.h"
#include "join.h"
                                                                                
                                                                                
                                                                                
/**                                                                          
 * @brief Performs join operation using the block nested loop join algorithm 
 *                                                                           
 * @pre Input result file is empty. The types of the fields being joined on  
 *    have the same type. The parameters are valid.                          
 *                                                                           
 * @post The result file is filled with the records that match the condition 
 *    of the select.                                                         
 *                                                                           
 * @param outer_id. FileId of the outer relation file.                       
 * @param inner_id. FileId of the inner relation file.                       
 * @param result_file File * pointing to an empty file which will be         
 *    filled with the tuples that satisfy the comparision                    
 * @param outer_fields. vector of FieldIds corresponding to the join fields  
 *    in outer_rel                                                              
 * @param inner_fields. vector of FieldIds corresponding to the join fields  
 *    in inner_rel                                                              
 * @param block_size. std::uint32_t size of block.                           
 */                                                                          
BlockNLJ::BlockNLJ(FileId outer_id, FileId inner_id, FileId result_id,
         std::vector<FieldId> outer_fields, std::vector<FieldId> inner_fields,
         std::uint32_t block_size, Catalog *catalog) : Join(outer_id,
         inner_id, result_id, outer_fields,
         inner_fields, catalog)
{
  this->block_size = block_size;

}           

BlockNLJ::~BlockNLJ() {}
                                                                                
/**
 * @brief Runs the operation.
 *
 * @pre Valid files and parameters have been passed to the constructor.
 * @post Result file has been populated with records that meet the criteria of
 *    the operation.
 */
void BlockNLJ::runOperation() {
  HeapFile* outer_file = (HeapFile*)this->outer.file;
  HeapFile* inner_file = (HeapFile*)this->inner.file;
  HeapFile* result_file = (HeapFile*)this->result_state.file;

  Key* outer_key = this->outer.key;
  Key* inner_key = this->inner.key;
  
  // first: create block scanners on inner and outer relations
  // (outer: block_size blocks at a time, inner: 1 block at a time)
  BlockHeapFileScanner *outer_scanner = 
      new BlockHeapFileScanner((HeapFile *)this->outer.file, this->block_size);
  BlockHeapFileScanner *inner_scanner = 
      new BlockHeapFileScanner((HeapFile *)this->inner.file, 1);

  Record* outer_record = this->outer.rec;
  Record* inner_record = this->inner.rec;

  outer_scanner->resetBlock();
  // go through the blocks and scan outer and inner records
  while(true){ 
    for (RecordId rid = outer_scanner->getNext(outer_record);
           rid != INVALID_RECORD_ID; 
            rid = outer_scanner->getNext(outer_record)) {
      outer_key->setKeyFromRecord(outer_record);
      inner_scanner->resetBlock();
      for( RecordId inner_rid = inner_scanner->getNext(inner_record);
           inner_rid != INVALID_RECORD_ID;
           inner_rid = inner_scanner->getNext(inner_record)) {

          inner_key->setKeyFromRecord( inner_record );
          if(outer_key->compareDifferent( inner_key, Comp::EQUAL ) ){
            Record result( this->result_state.schema );
            outer_record->combineRecords( this->inner_fields, inner_record, &result );
            result_file->insertRecord( result );
          }
      }
    }
    if(outer_scanner->nextBlock() == false){
      break;
    }
  }
  delete inner_scanner;
  delete outer_scanner;
}