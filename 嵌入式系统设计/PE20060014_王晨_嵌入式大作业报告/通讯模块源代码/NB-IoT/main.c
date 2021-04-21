/*
 * This is template for main module created by KSDK Project Generator. Enjoy!
 */

/*
 * [File Name]     main.c
 * [Platform]      MKL17Z32VFM4
 * [Project]       shuncom_project
 * [Version]       1.00
 * [Author]        6646
 * [Date]          04/11/2019
 * [Language]      'C'
 * [History]       1.00 - Original Release
 *
 */

//-----------------------------------------------------------------------
// Standard C/C++ Includes
//-----------------------------------------------------------------------

#include <stdio.h>
//-----------------------------------------------------------------------
// KSDK Includes
//-----------------------------------------------------------------------
#include "main.h"
#include "board.h"
#include "ZComDef.h"
#include "osal.h"
#include "osal_nv.h"
#include "ZGlobals.h"
//-----------------------------------------------------------------------
// Application Includes
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Main Function
//-----------------------------------------------------------------------

int main(void)
{

    // Configure board clock
	BoardClockInit();

    BoardInitMcu();

    osal_nv_init(NULL);
   // concentrator_Init(2);
    // Initialize basic NV items
    zgInit();

    // Initialize the operating system
    osal_init_system();

    osal_start_system(); // No Return from here
    return 0;

}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
