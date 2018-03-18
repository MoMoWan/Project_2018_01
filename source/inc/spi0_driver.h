/*++

Copyright (c) 2009-2010 Razer Ltd.                                                         
All rights reserved. This program contains proprietary and confidential information.

Module Name:
  spi_driver.h

Compiler:
  Keil Software µVision v5.11, with NXP Semiconductor LPC11U2x proccessor support.

Abstract:
  This file contains the definitions for the spi_driver.c, which contains the support
  for the SPI serial interface.

--*/


#ifndef _SPI_DRIVER_H_
#define _SPI_DRIVER_H_

#include "Type.h"

#define FIFOSIZE		              (8)


#define SSP0SR_TFE                (1 << 0)                // SSP Status register 
#define SSP0SR_TNF                (1 << 1) 
#define SSP0SR_RNE                (1 << 2)
#define SSP0SR_RFF                (1 << 3) 
#define SSP0SR_BSY                (1 << 4)

#define SSP0CR0_DSS               (1 << 0)                // SSP CR0 register
#define SSP0CR0_FRF               (1 << 4)
#define SSP0CR0_SPO               (1 << 6)
#define SSP0CR0_SPH               (1 << 7)
#define SSP0CR0_SCR               (1 << 8)

#define SSP0CR1_LBM               (1 << 0)                // SSP CR1 register 
#define SSP0CR1_SSE               (1 << 1)
#define SSP0CR1_MS                (1 << 2)
#define SSP0CR1_SOD               (1 << 3)

#define SSP0IMSC_RORIM            (1 << 0)                // SSP Interrupt Mask Set/Clear register 
#define SSP0IMSC_RTIM             (1 << 1)
#define SSP0IMSC_RXIM             (1 << 2)
#define SSP0IMSC_TXIM             (1 << 3)
 
#define SS0PRIS_RORRIS            (1 << 0)                // SSP0 Interrupt Status register
#define SS0PRIS_RTRIS             (1 << 1)
#define SS0PRIS_RXRIS             (1 << 2)
#define SS0PRIS_TXRIS             (1 << 3)
 
#define SS0PMIS_RORMIS            (1 << 0)                // SSP0 Masked Interrupt register
#define SS0PMIS_RTMIS             (1 << 1)
#define SS0PMIS_RXMIS             (1 << 2)
#define SS0PMIS_TXMIS             (1 << 3)

#define SSP0ICR_RORIC             (1 << 0)                // SSP0 Interrupt clear register 
#define SSP0ICR_RTIC              (1 << 1)
 
#define SSP0CR0_DDS_MASK          (15)                    // SSP0 CR0 Register Control
#define SSP0CR0_DDS_16BIT         (15 << 0)
#define SSP0CR0_DDS_8BIT          (7  << 0)
#define SSP0CR0_FRF_SPI           (0  << 4)
#define SSP0CR0_FRF_TI            (1  << 4)
#define SSP0CR0_FRF_MICROWIRE     (2  << 4)
#define SSP0CR0_CPOL_LOW          (0  << 6)
#define SSP0CR0_CPOL_HIGH         (1  << 6)
#define SSP0CR0_CPHA_FIRST_EDGE   (0  << 7)
#define SSP0CR0_CPHA_SECOND_EDGE  (1  << 7)
#define SSP0CR0_SCR_16            (15 << 8)
#define SSP0CR0_SCR_8             (7 << 8)
#define SSP0CR0_SCR_5             (4 << 8)
#define SSP0CR0_SCR_6             (5 << 8)





#define SPI_MEMORYPOINTER_REGISTER (0x1F)
#define SPI_MEMORYDATA_REGISTER   (0x20)
#define SPI_READ                  (0x80)

#define SPI_SET_MODE8             (LPC_SSP0->CR0 = (LPC_SSP0->CR0 & ~SSP0CR0_DDS_MASK) | SSP0CR0_DDS_8BIT)
#define SPI_SET_MODE16            (LPC_SSP0->CR0 = (LPC_SSP0->CR0 & ~SSP0CR0_DDS_MASK) | SSP0CR0_DDS_16BIT)

#define SPI_CHIP_SELECT_LOW       gpio_SetValue(0, 2, 0)
#define SPI_CHIP_SELECT_HIGH      gpio_SetValue(0, 2, 1)

#define SPI_ASSERT                {SPI_CHIP_SELECT_HIGH; \
                                   __NOP();              \
                                   __NOP();              \
                                   __NOP();              \
																		__NOP();              \
                                   __NOP();              \
                                   __NOP();              \
                                   SPI_CHIP_SELECT_LOW;  \
                                  } 
#define SPI_DE_ASSERT             SPI_CHIP_SELECT_HIGH

													
																	
void SSP0_Init(void);
void SSP0_Enable(void);
void SSP0_Disable(void);
U8   spiIsEnable(void);
void spiWriteByte(U8 addr, U8 data);
U8   spiReadByte(U8 addr);
void spiWriteBytes(U8 addr, U8 *pData, U32 length);
void spiReadBytes(U8 addr, U8 *pData, U32 length);

void spiDisplacement(void);
BOOL spiChecksum_SROM(void);
void spiDownload_SROM(void);



#endif // _SPI_DRIVER_H_

