#include <string>
#include <vector>
#include "swatdb_types.h"
#include "tupleNLJ.h"
#include "data.h"
#include "filescan.h"
#include "key.h"
#include "record.h"
#include "heapfile.h"
#include "hashindexfile.h"
#include "hashindexscanner.h"
#include "catalog.h"
#include "heapfilescanner.h"
#include "searchkeyformat.h"
#include "indexNLJ.h"
#include "join.h"
#include "swatdb_exceptions.h"

/**
 * @brief Constructor for Index Nested Loop Join operation. 
 *
 * @param outer_id. FileId of the outer relation file.
 * @param inner_id. FileId of the inner relation file.
 * @param inner_index_id. FileId of the index for the inner relation file.
 * @param result_id FileId of the result file.
 * @param outer_fields. Vector of FieldIds corresponding to the join fields
 *    in outer_rel
 * @param inner_fields. Vector of FieldIds corresponding to the join fields
 *    in inner_rel
 */
IndexNLJ::IndexNLJ(FileId outer_id, FileId inner_id, FileId inner_index_id, 
          FileId result_id, std::vector<FieldId> outer_fields, 
          std::vector<FieldId> inner_fields, Catalog *catalog) :
          Join(outer_id, inner_id, result_id, outer_fields, inner_fields,
          catalog)
{
  this->index_file = (HashIndexFile *)(this->catalog->getFile(inner_index_id));
  if(this->catalog->getRelationFileId(inner_index_id) != inner_id) {
    throw InvalidFileIdRelOpsManager();
  }
}

/**                                                                         
 * @brief Runs the operation.   
 *                                                                          
 * @pre Valid files and parameters have been passed to the contructor.      
 * @post Result file has been populated with records that meet the criteria 
 *    of the operation.                                                     
 */   
void IndexNLJ::runOperation(){
  HeapFile* outer_file = (HeapFile*)this->outer.file;
  HeapFile* inner_file = (HeapFile*)this->inner.file;
  HeapFile* result_file = (HeapFile*)this->result_state.file;
  Key* outer_key = this->outer.key;
  Key* inner_key = this->inner.key;

  // heap scanner on outside
  HeapFileScanner heap_scanner( outer_file ); 

  Record* outer_record = this->outer.rec;
  Record* inner_record = this->inner.rec;

  for( RecordId i = heap_scanner.getNext(outer_record);
            i != INVALID_RECORD_ID;
                i = heap_scanner.getNext(outer_record) )
  {
    outer_key->setKeyFromRecord( outer_record );

    // index scanner on inside
    HashIndexScanner index_scanner(this->index_file, outer_key);

    for( RecordId i = index_scanner.getNext();
              i != INVALID_RECORD_ID;
                    i = index_scanner.getNext() ){
      inner_file->getRecord(i, inner_record);

      Record result( this->result_state.schema );
      outer_record->combineRecords( this->inner_fields, inner_record, &result );
      result_file->insertRecord( result );
      
    }
  }

}