
#ifndef __RESIZER_MEM_6253_SERIES_H__
#define __RESIZER_MEM_6253_SERIES_H__


/******************************************************************************
 * MT6253 resizer macros and definitions START
 *****************************************************************************/
#define RESZ_BASE         (PRZ_base)
#define RESZ_CFG          (RESZ_BASE+0)
#define RESZ_CON          (RESZ_BASE+0x4)
#define RESZ_STA          (RESZ_BASE+0x8)
#define RESZ_INT          (RESZ_BASE+0xc)
#define RESZ_SRCSZ1       (RESZ_BASE+0x10)
#define RESZ_TARSZ1       (RESZ_BASE+0x14)
#define RESZ_HRATIO1      (RESZ_BASE+0x18)
#define RESZ_VRATIO1      (RESZ_BASE+0x1c)
#define RESZ_HRES1        (RESZ_BASE+0x20)
#define RESZ_VRES1        (RESZ_BASE+0x24)
#define RESZ_FRCFG        (RESZ_BASE+0x40)
#define RESZ_PRWMBASE     (RESZ_BASE+0x5c)

#define RESZ_YUV2RGB      (RESZ_BASE+0x80)
#define RESZ_TMBASE_RGB1  (RESZ_BASE+0x84)
#define RESZ_TMBASE_RGB2  (RESZ_BASE+0x88)
#define RESZ_INFO0        (RESZ_BASE+0xb0)
#define RESZ_INFO1        (RESZ_BASE+0xb4)
#define RESZ_INFO2        (RESZ_BASE+0xb8)
#define RESZ_INFO3        (RESZ_BASE+0xbc)
#define RESZ_INFO4        (RESZ_BASE+0xc0)
#define RESZ_INFO5        (RESZ_BASE+0xc4)
#define RESZ_TMBASE_Y     (RESZ_BASE+0xd0)
#define RESZ_TMBASE_U     (RESZ_BASE+0xd4)
#define RESZ_TMBASE_V     (RESZ_BASE+0xd8)
#define RESZ_SMBASE_Y     (RESZ_BASE+0xdc)
#define RESZ_SMBASE_U     (RESZ_BASE+0xe0)
#define RESZ_SMBASE_V     (RESZ_BASE+0xe4)



#define REG_RESZ_CFG          *((volatile unsigned int *) (RESZ_BASE+0))
#define REG_RESZ_CON          *((volatile unsigned int *) (RESZ_BASE+0x4))
#define REG_RESZ_STA          *((volatile unsigned int *) (RESZ_BASE+0x8))
#define REG_RESZ_INT          *((volatile unsigned int *) (RESZ_BASE+0xc))
#define REG_RESZ_SRCSZ1       *((volatile unsigned int *) (RESZ_BASE+0x10))
#define REG_RESZ_TARSZ1       *((volatile unsigned int *) (RESZ_BASE+0x14))
#define REG_RESZ_HRATIO1      *((volatile unsigned int *) (RESZ_BASE+0x18))
#define REG_RESZ_VRATIO1      *((volatile unsigned int *) (RESZ_BASE+0x1c))
#define REG_RESZ_HRES1        *((volatile unsigned int *) (RESZ_BASE+0x20))
#define REG_RESZ_VRES1        *((volatile unsigned int *) (RESZ_BASE+0x24))
#define REG_RESZ_FRCFG        *((volatile unsigned int *) (RESZ_BASE+0x40))
#define REG_RESZ_PRWMBASE     *((volatile unsigned int *) (RESZ_BASE+0x5c))
                                                                           
#define REG_RESZ_YUV2RGB      *((volatile unsigned int *) (RESZ_BASE+0x80))
#define REG_RESZ_TMBASE_RGB1  *((volatile unsigned int *) (RESZ_BASE+0x84))
#define REG_RESZ_TMBASE_RGB2  *((volatile unsigned int *) (RESZ_BASE+0x88))
#define REG_RESZ_INFO0        *((volatile unsigned int *) (RESZ_BASE+0xb0))
#define REG_RESZ_INFO1        *((volatile unsigned int *) (RESZ_BASE+0xb4))
#define REG_RESZ_INFO2        *((volatile unsigned int *) (RESZ_BASE+0xb8))
#define REG_RESZ_INFO3        *((volatile unsigned int *) (RESZ_BASE+0xbc))
#define REG_RESZ_INFO4        *((volatile unsigned int *) (RESZ_BASE+0xc0))
#define REG_RESZ_INFO5        *((volatile unsigned int *) (RESZ_BASE+0xc4))
#define REG_RESZ_TMBASE_Y     *((volatile unsigned int *) (RESZ_BASE+0xd0))
#define REG_RESZ_TMBASE_U     *((volatile unsigned int *) (RESZ_BASE+0xd4))
#define REG_RESZ_TMBASE_V     *((volatile unsigned int *) (RESZ_BASE+0xd8))            
#define REG_RESZ_SMBASE_Y     *((volatile unsigned int *) (RESZ_BASE+0xdc))
#define REG_RESZ_SMBASE_U     *((volatile unsigned int *) (RESZ_BASE+0xe0))
#define REG_RESZ_SMBASE_V     *((volatile unsigned int *) (RESZ_BASE+0xe4))       
#define REG_RESZ_CLIPLR     *((volatile unsigned int *) (RESZ_BASE+0xf4))       
#define REG_RESZ_CLIPTB     *((volatile unsigned int *) (RESZ_BASE+0xf8))       
                              
/*RESZ_CFG*/                  
#define RESZ_CFG_SRC1  (0x00000001)
#define RESZ_CFG_SRC2	(0x00000002)
#define RESZ_CFG_PCON     (0x00000010)
#define RESZ_CFG_NORGBDB    (0x00000200)

/*RESZ_CON*/                  
#define RESZ_CON_ENA     0x00000001
#define RESZ_CON_RST     0x00010000

/*RESZ_STA*/
#define RESZ_STA_LCDBUSY	0x00000001
#define RESZ_STA_PELHRBUSY    0x00000002
#define RESZ_STA_PELVRBUSY    0x00000004
#define RESZ_STA_OUTBUSY      0x00000008
#define RESZ_STA_MEMINBUSY	0x00000010
#define RESZ_STA_INBUSY		0x00000020

/*RESZ_INT*/
#define RESZ_INT_LCDINT		0x00000001
#define RESZ_INT_MEMO1INT	0x00000002
#define RESZ_INT_FSTARTINT	0x00000004
#define RESZ_INT_PXDINT		0x00000008
#define RESZ_INT_MEMO2INT	0x00000010

/*RESZ_FRCFG*/
#define RESZ_FRCFG_VRSS       0x00000001
#define RESZ_FRCFG_AVG0    0x00000010
#define RESZ_FRCFG_AVG1    0x00000020
#define RESZ_FRCFG_PCSF1      0x00000030
#define RESZ_FRCFG_PCSF2	0x000000c0
#define RESZ_FRCFG_WMSZ_MASK  0x3ff0000
#define RESZ_FRCFG_WMSZ_SHIFT 16
#define RESZ_FRCFG_LCDINT      0x100   
#define RESZ_FRCFG_MEMO1INTEN	0x200
#define RESZ_FRCFG_FSTARTINTEN 0x400

#define RESZ_MAX_WS	1600
#define RESZ_MAX_HS	1200

/* RESZ_YUB2RGB */
#define RESZ_YUB2RGB_ENABLE_BIT        1

#define RESZ_HRES1_MAX    2046
#define RESZ_VRES1_MAX    2046
  
#define RESZ_H_RATIO_SHIFT_BITS  20
#define RESZ_V_RATIO_SHIFT_BITS  20
   
#define RESZ_PXL_EN   0x1
#define RESZ_PXL_RST  0x10000

#define IDP_VR_MAXIMUM_RESZ_WM_SIZE (240 * 4 * 3)
#define IDP_CP_MAXIMUM_RESZ_WM_SIZE (1280 * 4 * 3)
#define IDP_IC_MAXIMUM_RESZ_WM_SIZE (1280 * 4 * 3)


/*macro*/
#define RESZ_SET_WMBASE_PXL(a)      DRV_WriteReg32(RESZ_PRWMBASE, (a))
#define RESZ_START()                DRV_WriteReg32(RESZ_CON, RESZ_PXL_EN)      
#define RESZ_STOP()                 DRV_WriteReg32(RESZ_CON, RESZ_PXL_RST)      
#define RESZ_RGB_2_YUB_ENABLE()     REG_RESZ_YUV2RGB &= ~RESZ_YUB2RGB_ENABLE_BIT
#define RESZ_RGB_2_YUB_DISABLE()    REG_RESZ_YUV2RGB |= RESZ_YUB2RGB_ENABLE_BIT
#define RESZ_MEM_INT_ENABLE()		REG_RESZ_FRCFG |= RESZ_FRCFG_MEMO1INTEN
#define RESZ_MEM_INT_DISABLE()		REG_RESZ_FRCFG &= ~RESZ_FRCFG_MEMO1INTEN
#define RESZ_LCD_INT_ENABLE()        REG_RESZ_FRCFG |= RESZ_FRCFG_LCDINT
#define RESZ_LCD_INT_DISABLE()       REG_RESZ_FRCFG &= ~RESZ_FRCFG_LCDINT
#define RESZ_FSTART_INT_ENABLE()        REG_RESZ_FRCFG |= RESZ_FRCFG_FSTARTINTEN
#define RESZ_FSTART_INT_DISABLE()       REG_RESZ_FRCFG &= ~RESZ_FRCFG_FSTARTINTEN
#define RESZ_RGB2YUV_INT_ENABLE()   REG_RESZ_YUV2RGB |= RESZ_YUB2RGB_INT_ENABLE_BIT
#define RESZ_RGB2YUV_INT_DISABLE()  REG_RESZ_YUV2RGB &= ~RESZ_YUB2RGB_INT_ENABLE_BIT
 
#define SET_RESZ_SRC_SIZE(width, height)    REG_RESZ_SRCSZ1 = (width|(height<<16)) 
#define SET_RESZ_TARGET_SIZE(width, height) REG_RESZ_TARSZ1 = (width|(height<<16))
  
#define SET_RESZ_FINE_RESIZE_WORK_MEM_SIZE(line)  REG_RESZ_FRCFG &= ~RESZ_FRCFG_WMSZ_MASK; \
                                                  REG_RESZ_FRCFG |= (line<<16);
                           
#define SET_RESZ_COARSE_RESIZE_1_2  REG_RESZ_FRCFG &= RESZ_FRCFG_PCSF1; \
                                    REG_RESZ_FRCFG |= 0x100;

#define SET_RESZ_COARSE_RESIZE_1_4  REG_RESZ_FRCFG &= RESZ_FRCFG_PCSF1; \
                                    REG_RESZ_FRCFG |= 0x200;
                           
#define SET_RESZ_COARSE_RESIZE_1_8  REG_RESZ_FRCFG &= RESZ_FRCFG_PCSF1; \
                                    REG_RESZ_FRCFG |= 0x300;
                           
#define SET_RESZ_COARSE_RESIZE_NONE REG_RESZ_FRCFG &= ~RESZ_FRCFG_PCSF1;

/* marcos of CRZ control register */                           
#define RESET_RESZ()  REG_RESZ_CON = RESZ_PXL_RST;  \
                      REG_RESZ_CON = 0;
                           
///power control
#define RESZ_POWERON()  DRVPDN_Disable(DRVPDN_CON3, DRVPDN_CON3_RESZ, PDN_RESZ);
#define RESZ_POWEROFF() DRVPDN_Enable(DRVPDN_CON3, DRVPDN_CON3_RESZ, PDN_RESZ);

/// macros for forcing resizer to resize in MT6253
#define RESZ_DBGCFG                     (RESZ_BASE + 0x90)
#define REG_RESZ_DBGCFG                 *((volatile unsigned int *) (RESZ_DBGCFG))

#define RESZ_FORCE_H_RESIZING()         do { REG_RESZ_DBGCFG |= 0x0400; } while(0)
#define RESZ_FORCE_V_RESIZING()         do { REG_RESZ_DBGCFG |= 0x0200; } while(0)
#define RESZ_CLEAR_FORCE_RESIZE_BITS()  do { REG_RESZ_DBGCFG &= ~0x0600; } while(0)

/******************************************************************************
 * MT6253 resizer macros and definitions END
 *****************************************************************************/

#endif
