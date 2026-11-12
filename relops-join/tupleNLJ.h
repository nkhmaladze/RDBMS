#ifndef  _SWATDB_TUPLENLJJOIN_H_
#define  _SWATDB_TUPLENLJJOIN_H_


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
 * TupleNLJ: defines the tuple nested loop join version, derived from Join
 */
class TupleNLJ : public Join {

  public:
    
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
    TupleNLJ(FileId outer_id, FileId inner_id, FileId result_id, 
        std::vector<FieldId> outer_fields, std::vector<FieldId> inner_fields,
        Catalog *catalog);

    /**
     * @brief Destructor for TupleNLJ. Deletes dynamic member variables. 
     */
    ~TupleNLJ();

    /**                                                                         
     * @brief Runs the operation.   
     *                                                                          
     * @pre Valid files and parameters have been passed to the contructor.      
     * @post Result file has been populated with records that meet the criteria 
     *    of the operation.                                                     
     */   
    void runOperation(); 

};

#endif