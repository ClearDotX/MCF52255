/*
 * File:        assert.c
 * Purpose:     Provide macro for software assertions
 *
 * Notes:       ASSERT macro defined in assert.h calls assert_failed()
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"

const char ASSERT_FAILED_STR[] = "Assertion failed in %s at line %d\n";

/********************************************************************/
void
assert_failed(char *file, int line)
{
    printf(ASSERT_FAILED_STR, file, line);

    while (1)
    {}
}
/********************************************************************/
