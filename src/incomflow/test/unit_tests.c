#include <assert.h>
#include "incomflow/minunit.h"
#include "incomflow/dbg.h"

#include "basic_tests.h"
#include "icfIO_tests.h"

/************************************************************
* Run all unit test functions
************************************************************/
char *all_tests()
{
  mu_suite_start();

  /**********************************************************
  * 
  **********************************************************/
  //mu_run_test(test_basic_structures);
  mu_run_test(test_icfIO_readerFunctions);
  mu_run_test(test_icfIO_readMesh);


  return NULL;
}

  
/************************************************************
* Main function to run unit tests
************************************************************/
int main(int argc, char *argv[])
{
  debug("----- RUNNING %s\n", argv[0]);
  
  char *result;
  result = all_tests();

  if (result != 0)
  {
    debug("FAILED: %s\n", result);
  }
  else
  {
    debug("\nALL TESTS PASSED!\n");
  }

  mu_print_tests_run();

  exit(result != 0);
}
