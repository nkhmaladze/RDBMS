#include <string>
#include <vector>
#include "swatdb_types.h"
#include "join.h"
#include "operation.h"
#include "catalog.h"

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
Join::Join(FileId outer_id, FileId inner_id, FileId result_id, 
    std::vector<FieldId> outer_fields, std::vector<FieldId> inner_fields,
    Catalog* catalog) : Operation(result_id, catalog){

  this->_initState(outer_id, outer_fields, &this->outer);
  this->_initState(inner_id, inner_fields, &this->inner);
  this->inner_fields = inner_fields;
  this->outer_fields = outer_fields;

  //TODO: check type mismatch

}

/**
 * @brief Destructor for the Join Operation.
 */
Join::~Join() {
  this->_delState(&this->outer);
  this->_delState(&this->inner);
}