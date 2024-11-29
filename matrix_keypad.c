/*
 * File:   matrix_keypad.c
 * Author: HARISH BILLUR
 *
 * Created on 11 November, 2024, 8:43 AM
 */


#include <xc.h>
#include "matrix_keypad.h"

void init_matrix_keypad(void)          // Function to initialize the matrix keypad
{
	/* Config PORTB as digital */
	ADCON1 = 0x0F;

	/* Set Rows (RB7 - RB5) as Outputs and Columns (RB4 - RB1) as Inputs */
	TRISB = 0x1E;

	/* Set PORTB input as pull up for columns */
	RBPU = 0;

	MATRIX_KEYPAD_PORT = MATRIX_KEYPAD_PORT | 0xE0;
}

unsigned char scan_key(void)
{
	ROW1 = LO; // Set ROW1 low
	ROW2 = HI; // Set ROW2 high
	ROW3 = HI; // Set ROW3 high
	
    if (COL1 == LO) // Check if COL1 is low
	{
		return 1; // Return key code 1
	}
	else if (COL2 == LO) // Check if COL2 is low
	{
		return 4; // Return key code 4
	}
	else if (COL3 == LO) // Check if COL3 is low
	{
		return 7; // Return key code 7
	}
	else if (COL4 == LO) // Check if COL4 is low
	{
		return 10; // Return key code 10
	}
    
	ROW1 = HI; // Set ROW1 high
	ROW2 = LO; // Set ROW2 low
	ROW3 = HI; // Set ROW3 high



	if (COL1 == LO)
	{
		return 2;
	}
	else if (COL2 == LO)
	{
		return 5;
	}
	else if (COL3 == LO)
	{
		return 8;
	}
	else if (COL4 == LO)
	{
		return 11;
	}

	ROW1 = HI;
	ROW2 = HI;
	ROW3 = LO;
    ROW3 = LO;
	
	if (COL1 == LO)
	{
		return 3;
	}
	else if (COL2 == LO)
	{
		return 6;
	}
	else if (COL3 == LO)
	{
		return 9;
	}
	else if (COL4 == LO)
	{
		return 12;
	}

	return 0xFF;
}

unsigned char read_switches(unsigned char detection_type)
{
	static unsigned char once = 1, key;

	if (detection_type == STATE_CHANGE)
	{
		key = scan_key();
		if(key != 0xFF && once  )
		{
			once = 0;
			return key;
		}
		else if(key == 0xFF)
		{
			once = 1;
		}
	}
	else if (detection_type == LEVEL_CHANGE)
	{
		return scan_key();
	}

	return 0xFF;
}
