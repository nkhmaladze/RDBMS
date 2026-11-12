#ifndef _SWATDB_BLOCKNLJOIN_H
#define  _SWATDB_BLOCKNLJOIN_H

#include <string>                                                               
#include <vector>                                                               
#include "swatdb_types.h"                                                       
#include "join.h"                                                               

/**
 * \file
 */
                                                                                
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
 * BlockNLJ implements the block nested loop join functionality. 
 */                                                                             
class BlockNLJ : public Join {                                                  
                                                                                
  public:                                                                       

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
    BlockNLJ(FileId outer_id, FileId inner_id, FileId result_id,                
        std::vector<FieldId> outer_fields, std::vector<FieldId> inner_fields,   
        std::uint32_t block_size, Catalog *catalog);                                           
                                                                                
    /**                                                                         
     * @brief Destructor for TupleNLJ. Deletes dynamic member variables.        
     */                                                                         
    ~BlockNLJ();                                                                
                                                                                
    /**                                                                         
     * @brief Runs the operation.                                               
     *                                                                          
     * @pre Valid files and parameters have been passed to the contructor.      
     * @post Result file has been populated with records that meet the criteria 
     *    of the operation.                                                     
     */                                                                         
    void runOperation();                                                        
                                                                                
    
  protected:

    /* size of the block to be used in the block nested loop join algorithm. */
    std::uint32_t block_size;
                                                                                
};      

#endif