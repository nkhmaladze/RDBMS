#ifndef  _SWATDB_JOIN_H_
#define  _SWATDB_JOIN_H_

/**
 * \file
 */


#include <string>
#include <vector>
#include "swatdb_types.h"
#include "operation.h"


class Catalog;
class Schema;
class File;
class RelationFile;
class HeapFile;
class HeapFileScanner;
class Record;
class Data;
class Key;


class Join : public Operation {

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
     * @param catalog. Pointer to catalog
     */
    Join(FileId outer_id, FileId inner_id, FileId result_id, 
        std::vector<FieldId> outer_fields, std::vector<FieldId> inner_fields,
        Catalog *catalog);

    /**
     * @brief Destructor for the Join Operation
     */
    ~Join();


   protected:
    /*
     * FileState struct for the outer relation 
     */
    fileState outer;
    
    /*
     * FileState struct for the inner relation 
     */
    fileState inner;

    /**
     * Outer fields used in the join operation
     */
    std::vector<FieldId> outer_fields;

    /**
     * Inner fields used in the join operation
     */
    std::vector<FieldId> inner_fields;

};

#endif
