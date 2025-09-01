#include <string>
#include <vector>
#include "swatdb_types.h"
#include "tupleNLJ.h"
#include "filescan.h"
#include "key.h"
#include "record.h"
#include "heapfile.h"
#include "heapfilescanner.h"
#include "searchkeyformat.h"
#include "tupleNLJ.h"
#include "join.h"



/**
 * @brief Constructor for Join operation. Join subclasses use this 
 *    constructor
 *
 * @param outer_id. FileId of the outer relation file.
 * @param inner_id. FileId of the inner relation file.
 * @param result_id FileId of the result file.
 * @param outer_fields. Vector of FieldIds corresponding to the join fields
 *    in outer_rel
 * @param inner_fields. Vector of FieldIds corresponding to the join fields
 *    in inner_rel
 *
 */
TupleNLJ::TupleNLJ(FileId outer_id, FileId inner_id, FileId result_id, 
    std::vector<FieldId> outer_fields, std::vector<FieldId> inner_fields,
    Catalog *catalog) : Join(outer_id, inner_id, result_id, 
    outer_fields, inner_fields, catalog) { }



/**
 * @brief Destructor for FileScan join operation.
 *
 * @post File state for the relations and result file are cleaned up
 */
TupleNLJ::~TupleNLJ(){}

/**                                                                         
 * @brief Runs the operation.  
 *                                                                          
 * @pre Valid files and parameters have been passed to the contructor.      
 * @post Result file has been populated with records that meet the criteria 
 *    of the operation.                                                     
 */   
void TupleNLJ::runOperation() {
  HeapFile* outer_file = (HeapFile*)this->outer.file;
  HeapFile* inner_file = (HeapFile*)this->inner.file;
  HeapFile* result_file = (HeapFile*)this->result_state.file;
  Key* outer_key = this->outer.key;
  Key* inner_key = this->inner.key;

  HeapFileScanner scanner( outer_file );
  Record* outer_record = this->outer.rec;
  Record* inner_record = this->inner.rec;
  
  for( RecordId i = scanner.getNext( outer_record ); i != INVALID_RECORD_ID; i = scanner.getNext( outer_record ) )
  {
    outer_key->setKeyFromRecord( outer_record );
    HeapFileScanner scanner1( inner_file );

    for( RecordId j = scanner1.getNext( inner_record ); j != INVALID_RECORD_ID; j = scanner1.getNext( inner_record ) )
    {
      inner_key->setKeyFromRecord( inner_record );

      if( outer_key->compareDifferent( inner_key, Comp::EQUAL ) ){
        Record result( this->result_state.schema );

        outer_record->combineRecords( this->inner_fields, inner_record, &result );
        result_file->insertRecord( result );
      }

    }

  }
}