#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <ctime>
#include <algorithm> 
#include <UnitTest++/UnitTest++.h>
#include <UnitTest++/TestReporterStdout.h>
#include <UnitTest++/TestRunner.h>

#define private public

#include "swatdb_exceptions.h"
#include "swatdb.h"
#include "diskmgr.h"
#include "bufmgr.h"
#include "page.h"
#include "catalog.h"
#include "file.h"
#include "filemgr.h"
#include "relopsmgr.h"
#include "record.h"
#include "schema.h"
#include "heapfile.h"
#include "hashindexfile.h"
#include "relationfile.h"
#include "data.h"
#include "heapfilescanner.h"
#include "operation.h"
#include "select.h"
#include "filescan.h"
#include "join.h"
#include "tupleNLJ.h"
#include "hashjoin.h"

#include "testerconf.h"

/*
 * 
 * Unit tests for RelopsMgr Class.
 */

/*
 * TestFixture Class for initializing and cleaning up objects. Any test called
 * with this class as TEST_FIXTURE has access to any public and protected data
 * members and methods of the object as if they were local variables and
 * helper functions of test functions. The constructor is called at the start
 * of the test to initialize the data members and destructor is called at the
 * end of the test to deallocate/free memory associated with the objects.
 * It is possible to declare another custom class and associate with tests via
 * TEST_FIXTURE. It is also possible to add more data members and functions.
 * Students are encouraged to do so as they see fit so lon as they are careful
 * not to cause any naming conflicts with other tests.
 */
class TestFixture{

  public:
    /*
     * Public data members that tests have access to as if they are local
     * variables.
     */
    SwatDB *swatdb;
    FileId file_id;
    FileId res_id;
    std::string file_name;
    std::vector<FieldEntry> field_list;
    Schema *schema;
    HeapFile *file;
    std::vector<std::vector<std::string>> data;
    FileId depts_file_id, courses_file_id, profs_file_id, undergrads_file_id,
           masters_file_id, phds_file_id, taking_file_id;
    std::string prof_index_file_name, name_index_file_name, gpa_index_file_name;
    


    TestFixture(){

      /*
       * Initializes all variables needed for testing. All DBMS objects are
       * created and initialized. A file, named "testrel.rel" is added through
       * the buf_mgr. Initializes a simple student schema and adds 5 records to 
       * the initial file. Creates a list of record_ids that were added to the
       * file. 
       */

      this->swatdb = new SwatDB(testdb_file, testdb_dir.c_str());
      this->swatdb->setSaveDB(testdb_save_file);
      depts_file_id = this->swatdb->getCatalog()->getFileId("departments");
      courses_file_id = this->swatdb->getCatalog()->getFileId("courses");
      profs_file_id = this->swatdb->getCatalog()->getFileId("profs");
      undergrads_file_id = 
        this->swatdb->getCatalog()->getFileId("undergrad_students");
      masters_file_id = 
        this->swatdb->getCatalog()->getFileId("masters_students");
      phds_file_id = this->swatdb->getCatalog()->getFileId("phd_students");
      taking_file_id = this->swatdb->getCatalog()->getFileId("is_taking");
      prof_index_file_name = "courses_prof_index";
      name_index_file_name = "undergrads_name_index";
      gpa_index_file_name = "phds_gpa_index";

    }

    /*
     * Clean up and deallocates all objects initilalized by the constructor.
     * The unix file is explicitly removed.
     */
    ~TestFixture(){

      delete swatdb;
    }

};


/**
 * Tests the functionality of the relational operators manager functions.
 * Creates an empty result file and fills with the appropriate results from the
 * Join condition.
 */
SUITE(TupleNLJTests) {

  /**
   * Tests the tuple NLJ on two simple relations
   */
  TEST_FIXTURE(TestFixture, basicTupleJoin){

    std::vector<FieldId> o_fields = {3};
    std::vector<FieldId> i_fields = {0};

    HeapFile *result = this->swatdb->getRelOpsMgr()->join(TupleNLJT, 
        profs_file_id, depts_file_id, o_fields, i_fields);

    std::cout << "Tuple NLJ Test\nSELECT *\n FROM professors, departments\n"
      << "WHERE professors.dept_id = departments.dept_id" << std::endl;
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 12);
  }

  /**
   * Tests tuple NLJ on two bigger relations
   */
  TEST_FIXTURE(TestFixture, TupleJoin2){

    std::vector<FieldId> o_fields = {3};
    std::vector<FieldId> i_fields = {0};

    HeapFile *result = this->swatdb->getRelOpsMgr()->join(TupleNLJT, 
        masters_file_id, depts_file_id, o_fields, i_fields);

    std::cout << "Tuple NLJ Test\n SELECT *\n FROM masters, departments\n"
      << "WHERE masters.major_dept_id = departments.dept_id" << std::endl;
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 5000);
  }
  
}

SUITE(JoinConsecutive) {

  TEST_FIXTURE(TestFixture, test) {
    std::vector<FieldId> o_fields = {0};
    std::vector<FieldId> i_fields = {3};

    HeapFile *result = this->swatdb->getRelOpsMgr()->join(TupleNLJT,
        profs_file_id, courses_file_id, o_fields, i_fields, EQUAL);

    std::cout << "\nTuple NLJ Test\n SELECT *\n FROM professors, courses\n"
      "WHERE professors.prof_id = courses.prof_tid" << std::endl;
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 20);

    o_fields = {3};
    i_fields = {0};


    result = this->swatdb->getRelOpsMgr()->join(TupleNLJT,
        this->swatdb->getCatalog()->getFileId("0result"),
        depts_file_id, o_fields, i_fields);

    std::cout << "\nTuple NLJ Test on first result\n"
      "SELECT *\nFROM professors, departments, courses\n"
      "WHERE professors.dept_id = departments.dept_id AND" <<
      "courses.prof_id = professors.prof_id" << std::endl;
    this->swatdb->getFileMgr()->printFile(result->getFileId());
    CHECK_EQUAL(result->getNumRecords(), 20);
  }
}




/*
 * Prints usage
 */
void usage(){
  std::cout << "Usage: ./bucketunittests -s <suite_name> -h help\n";
  std::cout << "Available Suites: " << "TupleNLJTests " << std::endl;
}

/*
 * The main program either run all tests or tests a specific SUITE, given its
 * name via command line argument option 's'. 
 * option 's', main runs all tests by default. If invalid argument is given by
 * option 's', 0 test is run
 */
int main(int argc, char** argv){
  const char* suite_name;
  bool test_all = true;
  int c;

  //check for suite_name argument if provided
  while ((c = getopt (argc, argv, "hs:")) != -1){
    switch(c) {
      case 'h': usage();
                exit(1);
      case 's': suite_name = optarg;
                test_all  = false;
                break;
      default: printf("optopt: %c\n", optopt);

    }
  }

  //run all tests
  if (test_all){
    return UnitTest::RunAllTests();
  }

  //run the SUITE of the given suite name
  UnitTest::TestReporterStdout reporter;
  UnitTest::TestRunner runner(reporter);
  return runner.RunTestsIf(UnitTest::Test::GetTestList(), suite_name,
      UnitTest::True(), 0);
}
