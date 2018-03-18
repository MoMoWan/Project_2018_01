#ifndef _SPI1_DRIVER_H_
#define _SPI1_DRIVER_H_

#include "Type.h"
#define SSP1SR_TFE                (1 << 0)                // SSP Status register 
#define SSP1SR_TNF                (1 << 1) 
#define SSP1SR_RNE                (1 << 2)
#define SSP1SR_RFF                (1 << 3) 
#define SSP1SR_BSY                (1 << 4)

#define SSP1CR0_DSS               (1 << 0)                // SSP CR0 register
#define SSP1CR0_FRF               (1 << 4)
#define SSP1CR0_SPO               (1 << 6)
#define SSP1CR0_SPH               (1 << 7)
#define SSP1CR0_SCR               (1 << 8)

#define SSP1CR1_LBM               (1 << 0)                // SSP CR1 register 
#define SSP1CR1_SSE               (1 << 1)
#define SSP1CR1_MS                (1 << 2)
#define SSP1CR1_SOD               (1 << 3)

#define SSP1IMSC_RORIM            (1 << 0)                // SSP Interrupt Mask Set/Clear register 
#define SSP1IMSC_RTIM             (1 << 1)
#define SSP1IMSC_RXIM             (1 << 2)
#define SSP1IMSC_TXIM             (1 << 3)
 
#define SS1PRIS_RORRIS            (1 << 0)                // SSP0 Interrupt Status register
#define SS1PRIS_RTRIS             (1 << 1)
#define SS1PRIS_RXRIS             (1 << 2)
#define SS1PRIS_TXRIS             (1 << 3)
 
#define SS1PMIS_RORMIS            (1 << 0)                // SSP0 Masked Interrupt register
#define SS1PMIS_RTMIS             (1 << 1)
#define SS1PMIS_RXMIS             (1 << 2)
#define SS1PMIS_TXMIS             (1 << 3)

#define SSP1ICR_RORIC             (1 << 0)                // SSP0 Interrupt clear register 
#define SSP1ICR_RTIC              (1 << 1)
 
#define SSP1CR0_DDS_MASK          (15)                    // SSP0 CR0 Register Control
#define SSP1CR0_DDS_16BIT         (15 << 0)
#define SSP1CR0_DDS_8BIT          (7  << 0)
#define SSP1CR0_FRF_SPI           (0  << 4)
#define SSP1CR0_FRF_TI            (1  << 4)
#define SSP1CR0_FRF_MICROWIRE     (2  << 4)
#define SSP1CR0_CPOL_LOW          (0  << 6)
#define SSP1CR0_CPOL_HIGH         (1  << 6)
#define SSP1CR0_CPHA_FIRST_EDGE   (0  << 7)
#define SSP1CR0_CPHA_SECOND_EDGE  (1  << 7)
#define SSP1CR0_SCR_16            (15 << 8)
#define SSP1CR0_SCR_8             (7 << 8)
#define SSP1CR0_SCR_5             (4 << 8)
#define SSP1CR0_SCR_6             (5 << 8)

#define FIFOSIZE                    (8)
#define SPI1_SET_MODE8             (LPC_SSP1->CR0 = (LPC_SSP1->CR0 & ~SSP1CR0_DDS_MASK) | SSP1CR0_DDS_8BIT)
#define SPI1_SET_MODE16            (LPC_SSP1->CR0 = (LPC_SSP1->CR0 & ~SSP1CR0_DDS_MASK) | SSP1CR0_DDS_16BIT)

#define SPI1_CHIP_SELECT_LOW       gpio_SetValue(1, 23, 0)
#define SPI1_CHIP_SELECT_HIGH      gpio_SetValue(1, 23, 1)
#define SPI1_ASSERT                {SPI1_CHIP_SELECT_HIGH; \
                                   __NOP();              \
                                   __NOP();              \
                                   __NOP();              \
																		__NOP();              \
                                   __NOP();              \
                                   __NOP();              \
                                   SPI1_CHIP_SELECT_LOW;  \
                                  } 
#define SPI1_DE_ASSERT             SPI1_CHIP_SELECT_HIGH			
																	
																	
void SSP1_Init(void);
void SSP1_Enable(void);
void SSP1_Disable(void);
U8   spi1IsEnable(void);
void SSP1_Duplex8(U8 *pData);
void spi1WriteByte(U8 addr,U8 pdata);
U8   spi1ReadByte(U8 addr);
void spi1WriteBytes(U8 reg,U32 addr, U8 *pData, U16 length);
void spi1ReadBytes(U8 reg,U32 addr, U8 *pData, U32 length);
void spi1WriteCommand(U8 command);		
																	
#endif
