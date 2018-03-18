/******************** (C) COPYRIGHT 2014 SONiX *******************************
* COMPANY:			SONiX
* DATE:					2014/05
* AUTHOR:				SA1
* IC:				SN32F240/230/220 
* DESCRIPTION:	SPI0 related functions.
*____________________________________________________________________________
*	REVISION	Date				User		Description
*	1.0				2013/12/17	SA1			1. First release
*	1.1				2014/05/23	SA1			1. Add __SSP0_DATA_FETCH_HIGH_SPEED macro
*
*____________________________________________________________________________
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS TIME TO MARKET.
* SONiX SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL 
* DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH SOFTWARE
* AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN 
* IN CONNECTION WITH THEIR PRODUCTS.
*****************************************************************************/

/*_____ I N C L U D E S ____________________________________________________*/
#include <SN32F240B.h>
#include "SPI.h"
//#include "Mouse_RamSetting.h"
//#include "Mouse_Const.h"
#include "avago_api.h"
#include "Utility.h"
/*_____ D E C L A R A T I O N S ____________________________________________*/


/*_____ D E F I N I T I O N S ______________________________________________*/



/*_____ M A C R O S ________________________________________________________*/


/*_____ F U N C T I O N S __________________________________________________*/

/*****************************************************************************
* Function			: SPI0_Init
* Description		: Initialization of SPI0 init
* Input				: None
* Output			: None
* Return			: None
* Note				: None
*****************************************************************************/
void SPI0_Init(void)
{
	//Enable HCLK for SSP0
	//SN_SYS1->AHBCLKEN |= (0x1 << 12);								//Enable clock for SSP0.
	SN_SYS1->AHBCLKEN_b.SPI0CLKEN = 1;							//Enable clock for SSP0.

	//SN_SPI0 setting
	SN_SPI0->CTRL0_b.DL = SSP_DL_8;									//3 ~ 16 Data length     
	SN_SPI0->CTRL0_b.MS = SSP_MS_MASTER_MODE;				//Master/Slave selection bit
	SN_SPI0->CTRL0_b.LOOPBACK = SSP_LOOPBACK_DIS; 	//Loop back mode
	SN_SPI0->CTRL0_b.SDODIS = SSP_SDODIS_EN; 				//Slave data output 
																									//(ONLY used in slave mode)																							
	SN_SPI0->CLKDIV_b.DIV = (SSP_DIV/2) - 1;				//SSPn clock divider	1M Clock

	//SSP0 SPI mode
	//Clock phase for edge sampling//Clock polarity selection bit//MSB/LSB selection bit
	SN_SPI0->CTRL1 = (SSP_CPHA_RISING_EDGE|SSP_CPOL_SCK_IDLE_HIGH|SSP_MLSB_MSB);				
									 					
	//SSP0 SEL0 setting
	SN_SPI0->CTRL0_b.SELDIS = SSP_SELDIS_DIS; 			//Auto-SEL disable bit
	__SPI0_SET_SEL0;

	//SSP0 Fifo reset
	__SPI0_FIFO_RESET;
	__SPI0_OUTPUTMODE_SEL0;
	
	//__SSP0_DATA_FETCH_HIGH_SPEED;									//Enable if Freq. of SCK > 6MHz

	//SPI0 enable	
	SN_SPI0->CTRL0_b.SPIEN  = mskSSP_SSPEN_EN;    			//SSP enable bit	
}

/*****************************************************************************
* Function		: SPI0_Enable
* Description	: SPI0 enable setting
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void SPI0_Enable(void)
{
	//Enable HCLK for SSP0
	SN_SYS1->AHBCLKEN |= (0x1 << 12);								//Enable clock for SSP0.

	SN_SPI0->CTRL0_b.SPIEN  = mskSSP_SSPEN_EN;    			//SSP enable bit
	__SPI0_FIFO_RESET;
}

/*****************************************************************************
* Function		: SPI0_Disable
* Description	: SPI0 disable setting
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void SPI0_Disable(void)
{
	SN_SPI0->CTRL0_b.SPIEN  = mskSSP_SSPEN_DIS;    		//SSP disable bit

	//Disable HCLK for SSP0
	SN_SYS1->AHBCLKEN &=~ (0x1 << 12);							//Disable clock for SSP0.
}

/*****************************************************************************
* Function		: SPI_W_2BYTE
* Description	: Write 2 Byte Data by SPI
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void SPI_W_2BYTE( uint8_t bRegAddr, uint8_t bData)
{	
	volatile uint8_t	i;		
	__SPI0_CLR_SEL0;

	SN_SPI0->DATA = (WRITE_BIT|bRegAddr);
	SN_SPI0->DATA = bData;	
	
	while(SN_SPI0->STAT_b.BUSY);
	// SYNC FIFO.
	i = SN_SPI0->DATA;	
	// SYNC FIFO.
	i = SN_SPI0->DATA;	
	UT_MAIN_DelayNx1us(3);
	__SPI0_SET_SEL0;
  __UT_MAIN_tSWR

}
/*****************************************************************************
* Function		: SPI_W_NBYTE
* Description	: SPI Write N bytes hwSPI_Rx_Fifo[] FIFO Data to SLED17341
* Input			: the start Address of the Ram of SLED17341 , and the Data Length SPI Data 
* Output		: 
* Return		: None
* Note			: None
******************************************************************************/
void SPI_W_NBYTE(uint8_t bRegAddr, uint8_t* pdata, uint16_t blength, uint8_t bCS)
{	
	uint16_t i;
	
	switch(bCS)
	{
		case 0:				//broadcast
			__SPI0_CLR_SEL0;
			__SPI0_CLR_SEL1;
			break;
		case 1:
			__SPI0_CLR_SEL0;
			break;
		case 2:
			__SPI0_CLR_SEL1;
		default: 
			break;
	}

  SN_SPI0->DATA = (WRITE_BIT|bRegAddr);
    while(SN_SPI0->STAT_b.BUSY);
	  UT_MAIN_DelayNx1us(30);	
	for(i = 0;i < blength;i++)
	{
//		while(SN_SPI0->STAT_b.TX_FULL);
		SN_SPI0->DATA = pdata[i];
    while(SN_SPI0->STAT_b.BUSY);
	  UT_MAIN_DelayNx1us(30);
	}

	while(SN_SPI0->STAT_b.BUSY);
	UT_MAIN_DelayNx1us(30);
	switch(bCS)
	{
		case 0:				//broadcast
			__SPI0_SET_SEL0;
			__SPI0_SET_SEL1;
			break;
		case 1:
			__SPI0_SET_SEL0;
			break;
		case 2:
			__SPI0_SET_SEL1;
		default: 
			break;
	}												//SEL is high
  __UT_MAIN_tSWR
}
/*****************************************************************************
* Function		: SPI_R_BYTE
* Description	: Read Byte Data by SPI
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
uint8_t SPI_R_BYTE(uint8_t bRegAddr)
{		
	volatile uint8_t	i;
	//SSP0 Fifo reset
	__SPI0_FIFO_RESET;	
	__SPI0_CLR_SEL0;

	//PMW3325 Read SPI CMD => setting bit7=0
	SN_SPI0->DATA = (READ_BIT|bRegAddr); 

	while(SN_SPI0->STAT_b.BUSY);
	//Bi-direction FIFO , Write FIFO and Read FIFO need to do at the same time. 
	i = SN_SPI0->DATA;

	//Delay for PMW3325 read data SPEC 
	__UT_MAIN_tSRAD;

	//Read Data from SPI
	SN_SPI0->DATA = 0x0FF;			//
	while(SN_SPI0->STAT_b.BUSY);

	//Check having any data in RXFIFO
	while(SN_SPI0->STAT & mskSSP_RX_EMPTY);	
	i = SN_SPI0->DATA;
//  while(SN_SPI0->STAT & mskSSP_RX_EMPTY);
//  i = SN_SPI0->DATA;
	UT_MAIN_DelayNx1us(3);	
	__SPI0_SET_SEL0;
__UT_MAIN_tSWR
	return(i);
}

/*****************************************************************************
* Function		: SPI_Burst_Read
* Description	: Burst	Read 12 Byte Data by SPI		//only 6 byte
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void SPI_Burst_Read(uint8_t bRegAddr, uint8_t* buf)
{
	volatile uint8_t	i;
//	uint8_t b_arySPI_Rx_Fifo[16];
	__SPI0_FIFO_RESET;				//SPI DATA FIFO	Reset  Clear All Data

	__SPI0_CLR_SEL0;

	//Read SPI CMD => setting bit7=0
	SN_SPI0->DATA = (READ_BIT|bRegAddr); 

	while(SN_SPI0->STAT_b.BUSY);
	//Bi-direction FIFO , Write FIFO and Read FIFO need to do at the same time. 
	i = SN_SPI0->DATA;

	//Delay for PMW3360 & 3325 read data SPEC 
	__UT_MAIN_tSRAD_MOTBR;

	//Read Data from SPI			//12byte 
	SN_SPI0->DATA = 0x0FF;			//0			
	SN_SPI0->DATA = 0x0FF;			//1			
	SN_SPI0->DATA = 0x0FF;			//2			
	SN_SPI0->DATA = 0x0FF;			//3			
	SN_SPI0->DATA = 0x0FF;			//4			
	SN_SPI0->DATA = 0x0FF;			//5			
	SN_SPI0->DATA = 0x0FF;			//6
	SN_SPI0->DATA = 0x0FF;			//7
	
	while(SN_SPI0->STAT_b.BUSY);

	//Check having any data in RXFIFO
	while(SN_SPI0->STAT & mskSSP_RX_EMPTY);	
	
//	b_arySPI_Rx_Fifo[0] = SN_SPI0->DATA;		//Motion
//	b_arySPI_Rx_Fifo[1] = SN_SPI0->DATA;		//Observation
//	b_arySPI_Rx_Fifo[2] = SN_SPI0->DATA;		//Delta_X_L
//	b_arySPI_Rx_Fifo[3] = SN_SPI0->DATA;		//Delta_X_H
//	b_arySPI_Rx_Fifo[4] = SN_SPI0->DATA;		//Delta_Y_L
//	b_arySPI_Rx_Fifo[5] = SN_SPI0->DATA;		//Delta_Y_H
    buf[0] = SN_SPI0->DATA;		//Motion
	  buf[1] = SN_SPI0->DATA;		//Observation
	  buf[2] = SN_SPI0->DATA;		//Delta_X_L
	  buf[3] = SN_SPI0->DATA;		//Delta_X_H
	  buf[4] = SN_SPI0->DATA;		//Delta_Y_L
	  buf[5] = SN_SPI0->DATA;		//Delta_Y_H	
	  buf[6] = SN_SPI0->DATA;
	  buf[7] = SN_SPI0->DATA;
//	
	__SPI0_FIFO_RESET;				//SPI DATA FIFO	Reset  Clear All Data
	
	//Read Data from SPI
	SN_SPI0->DATA = 0x0FF;			//8
	SN_SPI0->DATA = 0x0FF;			//9
	SN_SPI0->DATA = 0x0FF;			//10
	SN_SPI0->DATA = 0x0FF;			//11
	
	while(SN_SPI0->STAT_b.BUSY);

	//Check having any data in RXFIFO
	while(SN_SPI0->STAT & mskSSP_RX_EMPTY);	
	
//	b_arySPI_Rx_Fifo[8] = SN_SPI0->DATA;
//	b_arySPI_Rx_Fifo[9] = SN_SPI0->DATA;
//	b_arySPI_Rx_Fifo[10] = SN_SPI0->DATA;
//	b_arySPI_Rx_Fifo[11] = SN_SPI0->DATA;
	buf[8] = SN_SPI0->DATA;
	buf[9] = SN_SPI0->DATA;
	buf[10] = SN_SPI0->DATA;
	buf[11] = SN_SPI0->DATA;	
	__SPI0_SET_SEL0;
	__UT_MAIN_tSRW
}

