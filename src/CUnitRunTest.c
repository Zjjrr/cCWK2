#include <stdio.h>
#include <assert.h>
#include "CUnit-2.1-0/include/CUnit/console.h"

extern int AddTestMainModule();

int main() {
    if( CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();
        
    assert(NULL != CU_get_registry());
    assert(!CU_is_test_running());
    
    if (0 != AddTestMainModule()){
        CU_cleanup_registry(); 
        return CU_get_error(); 
    }
    
    CU_console_run_tests();
    CU_cleanup_registry();
    
    return 0;
}
