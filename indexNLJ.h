#ifndef  _SWATDB_INDEXNLJ_H_
#define  _SWATDB_INDEXNLJ_H_


/**
 * \file
 */

#include <string>
#include <vector>
#include "swatdb_types.h"
#include "join.h"

class FileManager;
class Catalog;
class Schema;
class File;
class RelationFile;
class HeapFile;
class HeapFileScanner;
class Record;
class Data;
class Key;
class SearchKeyFormat;


/**
 * IndexNLJ: index nested loop join class derived from Join 
 */
class IndexNLJ : public Join {

  public:
    
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
    IndexNLJ(FileId outer_id, FileId inner_id, FileId inner_index_id, 
             FileId result_id, std::vector<FieldId> outer_fields, 
             std::vector<FieldId> inner_fields, Catalog *catalog);

    /**                                                                         
     * @brief Runs the operation.   
     *                                                                          
     * @pre Valid files and parameters have been passed to the contructor.      
     * @post Result file has been populated with records that meet the criteria 
     *    of the operation.                                                     
     */   
    void runOperation(); 

  protected:

    /* index id */
    HashIndexFile *index_file;

};

#endif