/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------
 Module
   ShiftRegisterWrite.c

 Description
   This module acts as the low level interface to a write only shift register.
   There are 5 arrays in the file below. For every new shift register added, 
   one has to fill in the corresponding values in EACH of the 5 arrays. 
   For unused registers, fill in the arrays with the parameter NOT_USED. 
   Implementation for getiing a copy of the current register image has been 
   removed. 

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 10/11/15 19:55 jec     first pass
 10/20/17       EK      Filled in functions
 11/08/17       EK      Adding  implementation for multiple registers 
 11/10/17       EK      Adds 16 bit implementation, code tested                           
 
****************************************************************************/
//#define TEST

// Add header for this module
#include "ShiftRegisterWrite.h"

/*********************   Public Functions ***********************************/
// MODULE VARIABLE
// For initalizing hardware. SYSCTL_RCGCGPIO_R1 correspond to portB and so on
// Multiple registers can be used on the same port
uint32_t RCGCGPIO_List[] = {
                     SYSCTL_RCGCGPIO_R1,
                     NOT_USED, 
                     NOT_USED,
                     NOT_USED
                     };

// Port base for current register
// Multiple registers can be used on the same port
uint32_t PortBase_List[] = {
                     GPIO_PORTB_BASE,
                     NOT_USED, 
                     NOT_USED,
                     NOT_USED
                     };
                    
// bit value on specified port for DATA line                 
uint8_t Data_List[] = {
                  BIT0HI,
                  NOT_USED, 
                  NOT_USED,
                  NOT_USED
                  }; 

// bit value on specified port for SCLK line   
uint8_t SCLK_List[] = {
                  BIT1HI,
                  NOT_USED, 
                  NOT_USED,
                  NOT_USED
                  }; 

// bit value on specified port for RCLK line                     
uint8_t RCLK_List[] = {
                  BIT2HI,
                  NOT_USED, 
                  NOT_USED,
                  NOT_USED
                  }; 
  



/****************************************************************************
Function:
    SR_Init
    
Parameters:
    whichRegister: which register to use

Description:
    Set up Data, RCLK, SCLK for output. Clear Data and SCLK (set low).
    Set RCLK 

Author:
    EK
****************************************************************************/
void SR_Init(uint8_t whichRegister)
{
    
    uint32_t RCGPIO = RCGCGPIO_List[whichRegister];
    uint32_t PortBase = PortBase_List[whichRegister];
    uint8_t RCLK = RCLK_List[whichRegister]; 
    uint8_t SCLK = SCLK_List[whichRegister];
    uint8_t DATA = Data_List[whichRegister];

    /* Enable eripheral clock, wait till its ready to set direction */
    HWREG(SYSCTL_RCGCGPIO) |= RCGPIO; // check syntax
    while ((HWREG(SYSCTL_PRGPIO) & RCGPIO) != RCGPIO)
        ;

    /* Digital Enable */
    HWREG(PortBase+GPIO_O_DEN) |= (DATA| SCLK  | RCLK );

    /* Set Pins as Outputs */
    HWREG(PortBase+GPIO_O_DIR) |= (DATA | SCLK  | RCLK );
  
    // start with the data & sclk lines low 
    HWREG(PortBase+(GPIO_O_DATA + ALL_BITS)) &= (~SCLK & ~DATA);
    
    // RCLK line high
    HWREG(PortBase +(GPIO_O_DATA + ALL_BITS)) |= RCLK;  
}


/****************************************************************************
Function:
    SR_Write
    
Parameters:
    NewValue: byte (uint8_t) to write out the register
    whichRegister: The register to write to

Author:
    EK
****************************************************************************/
void SR_Write(uint8_t NewValue, uint8_t whichRegister)
{

    uint32_t PortBase = PortBase_List[whichRegister];
    uint8_t RCLK = RCLK_List[whichRegister]; 
    uint8_t SCLK = SCLK_List[whichRegister];
    uint8_t DATA = Data_List[whichRegister];

    uint8_t BitCounter;

    // lower the register clock
    HWREG(PortBase+(GPIO_O_DATA + ALL_BITS)) &= ~RCLK;

    // shift out the data while pulsing the serial clock  
    for (BitCounter = 0; BitCounter < 8; BitCounter++)
    {
        // Isolate the MSB of NewValue, put it into the LSB 
        if ((NewValue & BIT7HI)) 
        {
            HWREG(PortBase+(GPIO_O_DATA + ALL_BITS)) |= DATA;
        } 
        else
        {
            HWREG(PortBase+(GPIO_O_DATA + ALL_BITS)) &=  ~DATA;
        }
        
        // raise SCLK
        HWREG(PortBase+(GPIO_O_DATA + ALL_BITS)) |= SCLK;

        // lower SCLK
        HWREG(PortBase+(GPIO_O_DATA + ALL_BITS)) &= ~SCLK;
        NewValue = NewValue<<1;     
    }// finish looping through bits in NewValue
    
    // raise the register clock to latch the new data
    HWREG(PortBase+(GPIO_O_DATA + ALL_BITS)) |= RCLK;
}


/****************************************************************************
Function:
    SR_Write16
    
Parameters:
    NewValue: 16 bit number (uint16_t) to write out 2 daisy chained registers
                by writing one the MS byte then the LS byte
    whichRegister: The register to write to

Author:
    EK
****************************************************************************/
void SR_Write16(uint16_t NewValue, uint8_t whichRegister)
{
    uint8_t FirstByte = (uint8_t) NewValue>>8; // shift MS byte into LS byte
    uint8_t SecondByte = (uint8_t) NewValue; 
    SR_Write(FirstByte, whichRegister);
    SR_Write(SecondByte,whichRegister);    
}

/******************************* END OF SOURCE ******************************/

#ifdef TEST
/* test Harness for testing this module */
#include "termio.h"
int main(void)
{
    TERMIO_Init();
    puts("\r\n In test harness for ShiftRegisterWrite Write\r\n");
    SR_Init(0);   // Init Hardware for SR0
    //SR_Init(1);
    while(true)
    {
        SR_Write(0x01, 0); // byte for testing
        SR_Write(0x02, 0);
        SR_Write(0x04, 0);         
        
        //SR_Write(0x01, 1); // byte for testing
        //SR_Write(0x02, 1);
        //SR_Write(0xf0, 1);         
    }
    return 0;
}
#endif 
