/******************************************************************************
 ** File Name:      block_mem.c                                               *
 ** Author:         Richard Yang                                              *
 ** DATE:           05/09/2005                                                *
 ** Copyright:      2005 Spreadtrum, Incorporated. All Rights Reserved.       *
 ** Description:    This file defines the basic operation interfaces of       *
 **                 large block memory including malloc and free              *
 ******************************************************************************

 ******************************************************************************
 **                        Edit History                                       *
 ** ------------------------------------------------------------------------- *
 ** DATE           NAME             DESCRIPTION                               *
 ** 05/09/2005     Richard.Yang     Create.                                   *
 ******************************************************************************/

/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/

/*
//QVGA TOTAL SIZE = 1103K
|*************************************************1024************************************************--------| ID=256	BLOCK_MEM_POOL_WAP
|******************400******************----------------------------------------------------------------------| ID=259	BLOCK_MEM_POOL_MMS
|------**************300*************-------------------------------------------------------------------------| ID=276	BLOCK_MEM_POOL_MMI_3D
|*************300*************--------------------------------------------------------------------------------| ID=271	BLOCK_MEM_POOL_PIC_PROCESS
|-----------------------------******150******-----------------------------------------------------------------| ID=272	BLOCK_MEM_POOL_PIC_UNDO
|--------------------------------------------******150******--------------------------------------------------| ID=273	BLOCK_MEM_POOL_PIC_SOURCE
|-----------------------------------------------------------******150******-----------------------------------| ID=274	BLOCK_MEM_POOL_PIC_TARGET
|******************400******************----------------------------------------------------------------------| ID=275	BLOCK_MEM_POOL_KURO
|***100***----------------------------------------------------------------------------------------------------| ID=277	BLOCK_MEM_POOL_MMI_SPRITE
|------****100***---------------------------------------------------------------------------------------------| ID=278	BLOCK_MEM_POOL_CAF_IMAGE
|----------------****100***-----------------------------------------------------------------------------------| ID=279	BLOCK_MEM_POOL_CAF_RESOURCE
|*****150******-----------------------------------------------------------------------------------------------| ID=280	BLOCK_MEM_POOL_MTV_LCDTV
|***********************500***********************------------------------------------------------------------| ID=282	BLOCK_MEM_POOL_MTV_FIRMWARE
|-------------------------------------------------***********250***********-----------------------------------| ID=283	BLOCK_MEM_POOL_MTV_GZIPBUF
|--------------******150******--------------------------------------------------------------------------------| ID=284	BLOCK_MEM_POOL_MTV_RGB
|--------------******150******--------------------------------------------------------------------------------| ID=285	BLOCK_MEM_POOL_MTV_YUV422
|-----------------------------**75***-------------------------------------------------------------------------| ID=286	BLOCK_MEM_POOL_MTV_Y
|------------------------------------**75***------------------------------------------------------------------| ID=287	BLOCK_MEM_POOL_MTV_UV
|-----------------------------**75***-------------------------------------------------------------------------| ID=288	BLOCK_MEM_POOL_MTV_JPG
|------------------------------------***80***-----------------------------------------------------------------| ID=289	BLOCK_MEM_POOL_MTV_JPGENCODE
|----------------------------------------------------------------------------------------------******150******| ID=290	BLOCK_MEM_POOL_ROTATE_OSD
|*******190********-------------------------------------------------------------------------------------------| ID=257	BLOCK_MEM_POOL_DC_OSD
|*****************************************************1103****************************************************| ID=8	BLOCK_MEM_POOL_REF_DC
|*************500*********************************------------------------------------------------------------| ID=5	BLOCK_MEM_POOL_MP4ENC
|-------------------------------------------------*********************450*********************---------------| ID=9	BLOCK_MEM_POOL_REF_DV

//WQVGA TOTAL SIZE = 1241K
|*************************************************1024************************************************----------------------| ID=256	BLOCK_MEM_POOL_WAP
|******************400******************------------------------------------------------------------------------------------| ID=259	BLOCK_MEM_POOL_MMS
|------*****************375*****************--------------------------------------------------------------------------------| ID=276	BLOCK_MEM_POOL_MMI_3D
|****************375*****************---------------------------------------------------------------------------------------| ID=271	BLOCK_MEM_POOL_PIC_PROCESS
|------------------------------------********187*******---------------------------------------------------------------------| ID=272	BLOCK_MEM_POOL_PIC_UNDO
|-------------------------------------------------------********187*******--------------------------------------------------| ID=273	BLOCK_MEM_POOL_PIC_SOURCE
|--------------------------------------------------------------------------********187*******-------------------------------| ID=274	BLOCK_MEM_POOL_PIC_TARGET
|******************400******************------------------------------------------------------------------------------------| ID=275	BLOCK_MEM_POOL_KURO
|***100***------------------------------------------------------------------------------------------------------------------| ID=277	BLOCK_MEM_POOL_MMI_SPRITE
|------****100***-----------------------------------------------------------------------------------------------------------| ID=278	BLOCK_MEM_POOL_CAF_IMAGE
|----------------****100***-------------------------------------------------------------------------------------------------| ID=279	BLOCK_MEM_POOL_CAF_RESOURCE
|*******188*******----------------------------------------------------------------------------------------------------------| ID=280	BLOCK_MEM_POOL_MTV_LCDTV
|***********************500***********************--------------------------------------------------------------------------| ID=282	BLOCK_MEM_POOL_MTV_FIRMWARE
|-------------------------------------------------***********250***********-------------------------------------------------| ID=283	BLOCK_MEM_POOL_MTV_GZIPBUF
|-----------------********188*******----------------------------------------------------------------------------------------| ID=284	BLOCK_MEM_POOL_MTV_RGB
|-----------------********188*******----------------------------------------------------------------------------------------| ID=285	BLOCK_MEM_POOL_MTV_YUV422
|-----------------------------------***94****-------------------------------------------------------------------------------| ID=286	BLOCK_MEM_POOL_MTV_Y
|--------------------------------------------***94****----------------------------------------------------------------------| ID=287	BLOCK_MEM_POOL_MTV_UV
|-----------------------------------***94****-------------------------------------------------------------------------------| ID=288	BLOCK_MEM_POOL_MTV_JPG
|--------------------------------------------**80**-------------------------------------------------------------------------| ID=289	BLOCK_MEM_POOL_MTV_JPGENCODE
|--------------------------------------------------------------------------------------------------------********188********| ID=290	BLOCK_MEM_POOL_ROTATE_OSD
|*******190********---------------------------------------------------------------------------------------------------------| ID=257	BLOCK_MEM_POOL_DC_OSD
|************************************************************1241***********************************************************| ID=8	    BLOCK_MEM_POOL_REF_DC
|*****************500*****************************--------------------------------------------------------------------------| ID=5	    BLOCK_MEM_POOL_MP4ENC
|-------------------------------------------------*********************450*********************-----------------------------| ID=9	    BLOCK_MEM_POOL_REF_DV
*/
#include "mmi_app_kernel_trc.h"
#ifdef WIN32
#include "std_header.h"
#endif
#include "os_api.h"
#include "block_mem.h"
#include "mmidc_export.h"
//#include "mmiwap.h"
#include "mmidisplay_data.h"
#ifdef BROWSER_SUPPORT
#include "mmibrowser_export.h"
#endif
#include "mmimms_export.h"
//#include "mp4_block_mem_define.h"
#ifdef JAVA_SUPPORT
#include "mmijava_export.h"
#endif
//#include "mmiocr_export.h"
#ifdef GPS_SUPPORT
#include "mmigps_osapi.h"
#endif

#ifdef TTS_SUPPORT
#include "mmitts_export.h"
#endif

#ifdef CMMB_SUPPORT
#include "mmimtv_export.h"
#endif

#ifdef MRAPP_SUPPORT
#include "mmimrapp_export.h"
#endif

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
    extern   "C" 
    {
#endif

/**---------------------------------------------------------------------------*
 **                         Function Propertype                               *
 **---------------------------------------------------------------------------*/
//大内存分配表
//Base on
#define BLOCK_MEM_BASE_OFFSET           0

//wap
#ifdef BROWSER_SUPPORT_NETFRONT
#define BLOCK_MEM_POOL_WAP_OFFSET           (1500 * 1024)
#else
#define BLOCK_MEM_POOL_WAP_OFFSET           0
#endif

#if 0
#ifdef BROWSER_SUPPORT
#define BLOCK_MEM_POOL_WAP_SIZE             MMIWAP_WAP_MEMORY_SIZE
#else
#define BLOCK_MEM_POOL_WAP_SIZE             0
#endif
#else//@fen.xie useless
#define BLOCK_MEM_POOL_WAP_SIZE             0
#endif

//mms
#define BLOCK_MEM_POOL_MMS_OFFSET           0
#define BLOCK_MEM_POOL_MMS_SIZE             MMIMMS_BLOCK_BUFFER_SIZE
        
//dc osd
#define BLOCK_MEM_POOL_DC_OSD_OFFSET        0
#define BLOCK_MEM_POOL_DC_OSD_SIZE          (190 * 1024)//BLOCK_MEM_POOL_ROTATE_LCD_SIZE

//MP4SNAPSHOT
#define BLOCK_MEM_POOL_MP4SNAPSHOT_OFFSET   0
#define BLOCK_MEM_POOL_MP4SNAPSHOT_SIZE     0

//java
#ifdef JAVA_SUPPORT
#define BLOCK_MEM_POOL_JAVA_SIZE            MMIJAVA_MAX_SIZE
#ifdef JAVA_SUPPORT_MYRIAD
#define BLOCK_MEM_POOL_DEL_JAVA_SIZE        MMIJAVA_MAX_DEL_SIZE
#endif
#ifdef NANDBOOT_SUPPORT
#define BLOCK_MEM_POOL_JAVA_OFFSET          MAX((BLOCK_MEM_POOL_REF_DC_OFFSET + BLOCK_MEM_POOL_DCREF_SIZE),(BLOCK_MEM_POOL_WAP_OFFSET + BLOCK_MEM_POOL_WAP_SIZE))
#else
#define BLOCK_MEM_POOL_JAVA_OFFSET          0
#endif
#else
#define BLOCK_MEM_POOL_JAVA_SIZE            0
#define BLOCK_MEM_POOL_JAVA_OFFSET          0
#endif
//pic edit,in the case of QVGA
#define BLOCK_MEM_POOL_PIC_FIXED_SIZE        (100*1024)  //width*height*4  to  width*height*3
#define BLOCK_MEM_POOL_PIC_PROCESS_OFFSET    (BLOCK_MEM_BASE_OFFSET)
#define BLOCK_MEM_POOL_PIC_PROCESS_SIZE     (MMI_MAINSCREEN_WIDTH*MMI_MAINSCREEN_HEIGHT*3+BLOCK_MEM_POOL_PIC_FIXED_SIZE)  //lcd width*height*3

#define BLOCK_MEM_POOL_PIC_UNDO_OFFSET      (BLOCK_MEM_POOL_PIC_PROCESS_OFFSET + BLOCK_MEM_POOL_PIC_PROCESS_SIZE)
#define BLOCK_MEM_POOL_PIC_UNDO_SIZE        (MMI_MAINSCREEN_WIDTH*MMI_MAINSCREEN_HEIGHT*2)  //lcd width*height*2

#define BLOCK_MEM_POOL_PIC_SOURCE_OFFSET    (BLOCK_MEM_POOL_PIC_UNDO_OFFSET+BLOCK_MEM_POOL_PIC_UNDO_SIZE)
#define BLOCK_MEM_POOL_PIC_SOURCE_SIZE      (MMI_MAINSCREEN_WIDTH*MMI_MAINSCREEN_HEIGHT*2)  //lcd width*height*2

#define BLOCK_MEM_POOL_PIC_TARGET_OFFSET    (BLOCK_MEM_POOL_PIC_SOURCE_OFFSET + BLOCK_MEM_POOL_PIC_SOURCE_SIZE)
#define BLOCK_MEM_POOL_PIC_TARGET_SIZE      (MMI_MAINSCREEN_WIDTH*MMI_MAINSCREEN_HEIGHT*2)  //lcd width*height*2
 
//mmi 3d 
#define BLOCK_MEM_POOL_MMI_3D_OFFSET        BLOCK_MEM_BASE_OFFSET

//mmi sprite 100K
#define BLOCK_MEM_POOL_MMI_SPRITE_OFFSET        0
#define BLOCK_MEM_POOL_MMI_SPRITE_SIZE          (100*1024)
    
//OCR
#define BLOCK_MEM_POOL_OCR_OFFSET           0
#ifdef MMI_OCR_ENABLE
    #define BLOCK_MEM_POOL_OCR_SIZE         MMIOCR_MAX_SIZE
#else
    #define BLOCK_MEM_POOL_OCR_SIZE         0
#endif

//GPS
#define BLOCK_MEM_POOL_GPS_OFFSET           0
#ifdef GPS_SUPPORT
    #define BLOCK_MEM_POOL_GPS_SIZE         MMIGPS_MAX_SIZE
#else
    #define BLOCK_MEM_POOL_GPS_SIZE         0
#endif

// tts
#ifdef TTS_SUPPORT
#ifdef TTS_FILE_RES
#define BLOCK_MEM_POOL_TTSRESCATHEBUFFER_OFFSET		(BLOCK_MEM_POOL_MMI_3D_OFFSET+BLOCK_MEM_POOL_MMI_DESKTOP3D_SIZE)
//#define TTS_TTSRESCATHEBUFFER_SIZE (516*1024)// jun.hu 把它放到tts_api.h中去了,并修改了个名字.....
#define BLOCK_MEM_POOL_TTSHEAPBUFFER_OFFSET (BLOCK_MEM_POOL_TTSRESCATHEBUFFER_OFFSET+KiTTS_RESCATHEBUFFER_SIZE)
#define BLOCK_MEM_POOL_TTSVOICEDATA_OFFSET (BLOCK_MEM_POOL_TTSHEAPBUFFER_OFFSET+KiTTS_HEAP_BUFFER_SIZE)
#else
#define BLOCK_MEM_POOL_TTSHEAPBUFFER_OFFSET 0//(BLOCK_MEM_POOL_EBOOK_OFFSET+BLOCK_MEM_POOL_EBOOK_SIZE)
#define BLOCK_MEM_POOL_TTSVOICEDATA_OFFSET (KiTTS_RESCATHEBUFFER_SIZE+KiTTS_HEAP_BUFFER_SIZE)
#endif
#endif

//streaming

#define BLOCK_MEM_POOL_STREAMING_OFFSET     0
#define BLOCK_MEM_POOL_STREAMING_SIZE       BLOCK_MEM_POOL_JAVA_SIZE
    
//kuro
#ifdef KURO_SUPPORT
#define BLOCK_MEM_POOL_KURO_SIZE            (400*1024)
#else
#define BLOCK_MEM_POOL_KURO_SIZE            0
#endif    
#define BLOCK_MEM_POOL_KURO_OFFSET          0

#ifdef DYNAMIC_MODULE_SUPPORT
/*  by wei.zhou, adjust caf image size, 2009, Jun 20th
#define BLOCK_MEM_POOL_CAF_IMAGE_SIZE             (100*1024)
*/
#define BLOCK_MEM_POOL_CAF_IMAGE_SIZE             (200*1024)
#define BLOCK_MEM_POOL_CAF_RESOURCE_SIZE          (100*1024)
#else
#define BLOCK_MEM_POOL_CAF_IMAGE_SIZE             0
#define BLOCK_MEM_POOL_CAF_RESOURCE_SIZE          0
#endif
#define BLOCK_MEM_POOL_CAF_IMAGE_OFFSET           0
#define BLOCK_MEM_POOL_CAF_RESOURCE_OFFSET       (BLOCK_MEM_POOL_CAF_IMAGE_OFFSET + BLOCK_MEM_POOL_CAF_IMAGE_SIZE)


#ifdef CMMB_SUPPORT
#define BLOCK_MEM_POOL_MTV_LCDTV_SIZE       (2 * BLOCK_MEM_UINT8_SCREEN_BUFFER_SIZE)//(160*1024)
#define BLOCK_MEM_POOL_MTV_EXTPCM_SIZE      0//(72*1024)
#define BLOCK_MEM_POOL_FIRMWARE_SIZE        (500*1024)
#define BLOCK_MEM_POOL_MTV_GIZPBUF_SIZE     (250*1024)

#define BLOCK_MEM_POOL_MTV_RGB_SIZE         (2 * BLOCK_MEM_UINT8_SCREEN_BUFFER_SIZE)
#define BLOCK_MEM_POOL_MTV_YUV422_SIZE      (2 * BLOCK_MEM_UINT8_SCREEN_BUFFER_SIZE)
#define BLOCK_MEM_POOL_MTV_Y_SIZE           BLOCK_MEM_UINT8_SCREEN_BUFFER_SIZE
#define BLOCK_MEM_POOL_MTV_UV_SIZE          BLOCK_MEM_UINT8_SCREEN_BUFFER_SIZE
#define BLOCK_MEM_POOL_MTV_JPG_SIZE         BLOCK_MEM_UINT8_SCREEN_BUFFER_SIZE
#define BLOCK_MEM_POOL_MTV_JPGENCODE_SIZE   MMIMTV_SNAPSHOT_JPGENCODE_SIZE

#else
#define BLOCK_MEM_POOL_MTV_LCDTV_SIZE       0
#define BLOCK_MEM_POOL_MTV_EXTPCM_SIZE      0
#define BLOCK_MEM_POOL_FIRMWARE_SIZE        0
#define BLOCK_MEM_POOL_MTV_GIZPBUF_SIZE     0

#define BLOCK_MEM_POOL_MTV_RGB_SIZE         0
#define BLOCK_MEM_POOL_MTV_YUV422_SIZE      0
#define BLOCK_MEM_POOL_MTV_Y_SIZE           0
#define BLOCK_MEM_POOL_MTV_UV_SIZE          0
#define BLOCK_MEM_POOL_MTV_JPG_SIZE         0
#define BLOCK_MEM_POOL_MTV_JPGENCODE_SIZE   0
#endif

#define BLOCK_MEM_POOL_ROTATE_OSD_SIZE      (2 * BLOCK_MEM_UINT8_SCREEN_BUFFER_SIZE)
#define BLOCK_MEM_POOL_ROTATE_OSD_OFFSET    (BLOCK_MEM_POOL_REF_DC_OFFSET+BLOCK_MEM_POOL_DCREF_SIZE - BLOCK_MEM_POOL_ROTATE_OSD_SIZE) //total size -osd size
    
#define BLOCK_MEM_POOL_MTV_LCDTV_OFFSET     0
#define BLOCK_MEM_POOL_MTV_EXTPCM_OFFSET    (BLOCK_MEM_POOL_MTV_LCDTV_OFFSET+BLOCK_MEM_POOL_MTV_LCDTV_SIZE)//(BLOCK_MEM_POOL_MTV_LCDTV_OFFSET +BLOCK_MEM_POOL_MTV_LCDTV_SIZE)
#define BLOCK_MEM_POOL_MTV_FIRMWARE_OFFSET  0
#define BLOCK_MEM_POOL_MTV_GIZPBUF_OFFSET   (BLOCK_MEM_POOL_FIRMWARE_SIZE)

#define BLOCK_MEM_POOL_MTV_RGB_OFFSET       (BLOCK_MEM_POOL_MTV_EXTPCM_OFFSET+BLOCK_MEM_POOL_MTV_EXTPCM_SIZE)
#define BLOCK_MEM_POOL_MTV_YUV422_OFFSET    BLOCK_MEM_POOL_MTV_RGB_OFFSET
#define BLOCK_MEM_POOL_MTV_Y_OFFSET         (BLOCK_MEM_POOL_MTV_YUV422_OFFSET+BLOCK_MEM_POOL_MTV_YUV422_SIZE)
#define BLOCK_MEM_POOL_MTV_UV_OFFSET        (BLOCK_MEM_POOL_MTV_Y_OFFSET+BLOCK_MEM_POOL_MTV_Y_SIZE)
#define BLOCK_MEM_POOL_MTV_JPG_OFFSET       BLOCK_MEM_POOL_MTV_Y_OFFSET
#define BLOCK_MEM_POOL_MTV_JPGENCODE_OFFSET (BLOCK_MEM_POOL_MTV_JPG_OFFSET+BLOCK_MEM_POOL_MTV_JPG_SIZE)

//REF_DC
#ifdef PLATFORM_SC8800H
#define BLOCK_MEM_POOL_REF_DC_OFFSET        (BLOCK_MEM_POOL_WAP_OFFSET + BLOCK_MEM_POOL_WAP_SIZE)
#else
#define BLOCK_MEM_POOL_REF_DC_OFFSET        0
#endif
#ifdef NANDBOOT_SUPPORT

#ifdef PLATFORM_SC8800H

#define BLOCK_MEM_POOL_DCREF_SIZE           (2560*1024)//(4*1024*1024) //支持2M的Sensor 需要的Memory达到4M@8800h

#define BLOCK_MEM_POOL_ISP_VT_SIZE           (2560*1024) //   2.5M is enough for Vt application

#else
#ifdef PLATFORM_SC8800G

#define BLOCK_MEM_POOL_DCREF_SIZE           (5*1024*1024) //支持2M的Sensor 需要的Memory达到5M@8800G

#define BLOCK_MEM_POOL_ISP_VT_SIZE           (2560*1024) //   2.5M is enough for Vt application

#else
#define BLOCK_MEM_POOL_DCREF_SIZE           (5*1024*1024) //支持2M的Sensor 需要的Memory达到5M

#endif 
#endif 
#else
#define BLOCK_MEM_POOL_DCREF_SIZE           BLOCK_MEM_POOL_REF_CAMERA_SIZE
#endif
//REF_MP4ENC
#define BLOCK_MEM_POOL_MP4ENC_OFFSET        0
#define BLOCK_MEM_POOL_MP4ENC_SIZE 	        BLOCK_MEM_POOL_REF_MP4ENC_SIZE

//REF_DV
#define BLOCK_MEM_POOL_DVREF_SIZE           BLOCK_MEM_POOL_REF_DV_SIZE
#define BLOCK_MEM_POOL_DVREF_OFFSET         BLOCK_MEM_POOL_MP4ENC_SIZE


// MV_RES
#ifdef MV_SUPPORT
#define BLOCK_MEM_POOL_MV_RES_OFFSET        0
#define BLOCK_MEM_POOL_MV_RES_SIZE          (150*1024)
#endif

#ifdef MENU_CYCLONE_SUPPORT
#define BLOCK_MEM_POOL_CYCLONE_OFFSET        0
#ifdef MAINLCD_SIZE_240X320
#define BLOCK_MEM_POOL_CYCLONE_SIZE          (1000*1024)
#elif defined MAINLCD_SIZE_240X400
#define BLOCK_MEM_POOL_CYCLONE_SIZE          (1300*1024)
#elif defined MAINLCD_SIZE_320X480
#define BLOCK_MEM_POOL_CYCLONE_SIZE          (2000*1024)
#endif
#endif
#define BLOCK_MALLOC_DEFAULT_SIZE           0



//typedef BOOLEAN (*BL_CallBack)(BLOCK_MEM_ID_E block_id, BLOCK_MEM_RELATION_STATUS_E		relation_info);

typedef struct BLOCK_MEM_tag
{
    BLOCK_MEM_ID_E      block_id;
    //uint32           	block_offset;      //block相对BLOCK_POOL的起始地址
    uint32           	block_num;         //block的大小，单位是BLOCK_POOL_SIZE
    BLOCK_MEM_STATUS_E  block_mem_flag;    //表明这个block是否已经分配
	//BL_CallBack			block_callback_ptr; //Block mem回调函数
}BLOCK_MEM_T;
/*****************************************************************************/
//  Description: Callback by other block_mem alloc fail, when 3d using
//  Global resource dependence: 
//  Author: bin.ji
//  Note:
/*****************************************************************************/
// LOCAL BOOLEAN CallbackBy3D(
//                            BLOCK_MEM_ID_E id,
//                            BLOCK_MEM_RELATION_STATUS_E relation_info
//                            );


//不需要再填block_offset和block_callback_ptr字段
LOCAL BLOCK_MEM_T mem_table[] = 
{
#ifdef BROWSER_SUPPORT
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_WAP,            /*BLOCK_MEM_POOL_WAP_OFFSET,*/          BLOCK_MEM_POOL_WAP_SIZE,                NOT_USE,    /*PNULL*/},
#endif
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMS,            /*BLOCK_MEM_POOL_MMS_OFFSET,*/          BLOCK_MEM_POOL_MMS_SIZE,                NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_3D_DESKTOP, /*BLOCK_MEM_POOL_MMI_3D_OFFSET,*/       BLOCK_MEM_POOL_MMI_DESKTOP3D_SIZE,             NOT_USE,    /*CallbackBy3D*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_3D_ICON_MENU,/*BLOCK_MEM_POOL_MMI_3D_OFFSET,*/       BLOCK_MEM_POOL_MMI_MENU3D_SIZE,             NOT_USE,    /*CallbackBy3D*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_3D_SECOND_LIST,/*BLOCK_MEM_POOL_MMI_3D_OFFSET,*/       BLOCK_MEM_POOL_MMI_MENU3D_SIZE,             NOT_USE,    /*CallbackBy3D*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_JAVA,           /*BLOCK_MEM_POOL_JAVA_OFFSET,*/         BLOCK_MEM_POOL_JAVA_SIZE,               NOT_USE,    /*PNULL*/},
#ifdef JAVA_SUPPORT_MYRIAD
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_DEL_JAVA,       /*BLOCK_MEM_POOL_JAVA_OFFSET,*/         BLOCK_MEM_POOL_DEL_JAVA_SIZE,           NOT_USE,    /*PNULL*/},
#endif
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_PIC_PROCESS,    /*BLOCK_MEM_POOL_PIC_PROCESS_OFFSET,*/  BLOCK_MEM_POOL_PIC_PROCESS_SIZE,        NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_PIC_UNDO,       /*BLOCK_MEM_POOL_PIC_UNDO_OFFSET,*/     BLOCK_MEM_POOL_PIC_UNDO_SIZE,           NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_PIC_SOURCE,     /*BLOCK_MEM_POOL_PIC_SOURCE_OFFSET,*/   BLOCK_MEM_POOL_PIC_SOURCE_SIZE,         NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_PIC_TARGET,     /*BLOCK_MEM_POOL_PIC_TARGET_OFFSET,*/   BLOCK_MEM_POOL_PIC_TARGET_SIZE,         NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_KURO,           /*BLOCK_MEM_POOL_KURO_OFFSET,*/         BLOCK_MEM_POOL_KURO_SIZE,               NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_SPRITE,     /*BLOCK_MEM_POOL_MMI_SPRITE_OFFSET,*/   BLOCK_MEM_POOL_MMI_SPRITE_SIZE,         NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_CAF_IMAGE,      /*BLOCK_MEM_POOL_CAF_IMAGE_OFFSET,*/    BLOCK_MEM_POOL_CAF_IMAGE_SIZE,          NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_CAF_RESOURCE,   /*BLOCK_MEM_POOL_CAF_RESOURCE_OFFSET,*/ BLOCK_MEM_POOL_CAF_RESOURCE_SIZE,       NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MTV_LCDTV,      /*BLOCK_MEM_POOL_MTV_LCDTV_OFFSET,*/    BLOCK_MEM_POOL_MTV_LCDTV_SIZE,          NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MTV_EXTPCM,     /*BLOCK_MEM_POOL_MTV_EXTPCM_OFFSET,*/   BLOCK_MEM_POOL_MTV_EXTPCM_SIZE,         NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MTV_FIRMWARE,   /*BLOCK_MEM_POOL_MTV_FIRMWARE_OFFSET,*/ BLOCK_MEM_POOL_FIRMWARE_SIZE,           NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MTV_GZIPBUF,    /*BLOCK_MEM_POOL_MTV_GIZPBUF_OFFSET,*/  BLOCK_MEM_POOL_MTV_GIZPBUF_SIZE,        NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MTV_RGB,        /*BLOCK_MEM_POOL_MTV_RGB_OFFSET,*/      BLOCK_MEM_POOL_MTV_RGB_SIZE,            NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MTV_YUV422,     /*BLOCK_MEM_POOL_MTV_YUV422_OFFSET,*/   BLOCK_MEM_POOL_MTV_YUV422_SIZE,         NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MTV_Y,          /*BLOCK_MEM_POOL_MTV_Y_OFFSET,*/        BLOCK_MEM_POOL_MTV_Y_SIZE,              NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MTV_UV,         /*BLOCK_MEM_POOL_MTV_UV_OFFSET,*/       BLOCK_MEM_POOL_MTV_UV_SIZE,             NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MTV_JPG,        /*BLOCK_MEM_POOL_MTV_JPG_OFFSET,*/      BLOCK_MEM_POOL_MTV_JPG_SIZE,            NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MTV_JPGENCODE,  /*BLOCK_MEM_POOL_MTV_JPGENCODE_OFFSET,*/BLOCK_MEM_POOL_MTV_JPGENCODE_SIZE,      NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_ROTATE_OSD,     /*BLOCK_MEM_POOL_ROTATE_OSD_OFFSET,*/   BLOCK_MEM_POOL_ROTATE_OSD_SIZE,         NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_DC_OSD,         /*BLOCK_MEM_POOL_DC_OSD_OFFSET,*/       BLOCK_MEM_POOL_DC_OSD_SIZE,             NOT_USE,    /*PNULL*/},

//use in 3G platform
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_STREAMING,      /*BLOCK_MEM_POOL_STREAMING_OFFSET,*/    BLOCK_MEM_POOL_STREAMING_SIZE,          NOT_USE,    /*PNULL*/},
//application
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_OCR,            /*BLOCK_MEM_POOL_OCR_OFFSET,*/          BLOCK_MEM_POOL_OCR_SIZE,                NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_GPS,            /*BLOCK_MEM_POOL_GPS_OFFSET,*/          BLOCK_MEM_POOL_GPS_SIZE,                NOT_USE,    /*PNULL*/},
//ref use
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_REF_DC,         /*BLOCK_MEM_POOL_REF_DC_OFFSET,*/       BLOCK_MEM_POOL_DCREF_SIZE,              NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MP4ENC,         /*BLOCK_MEM_POOL_MP4ENC_OFFSET,*/       BLOCK_MEM_POOL_MP4ENC_SIZE,             NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_REF_DV,         /*BLOCK_MEM_POOL_DVREF_OFFSET,*/        BLOCK_MEM_POOL_DVREF_SIZE,              NOT_USE,    /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MP4SNAPSHOT,    /*BLOCK_MEM_POOL_MP4SNAPSHOT_OFFSET,*/  BLOCK_MEM_POOL_MP4SNAPSHOT_SIZE,        NOT_USE,    /*PNULL*/},
    //{(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_DC,             BLOCK_MEM_POOL_DC_OFFSET,           MMIDC_MAX_PHOTO_SIZE,                   NOT_USE,    PNULL},
    //{(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MP4DEC,         BLOCK_MEM_POOL_MP4DEC_OFFSET,       BLOCK_MEM_POOL_MP4DEC_SIZE,             NOT_USE,    PNULL},
#if defined (PLATFORM_SC8800H) || defined(PLATFORM_SC8800G)
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_ISP,            /*BLOCK_MEM_POOL_REF_DC_OFFSET,*/          BLOCK_MEM_POOL_ISP_VT_SIZE,             NOT_USE,    /*PNULL*/},
#endif
    //{(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MP4_DEMUX,      BLOCK_MEM_POOL_MP4DEMUX_OFFSET,     BLOCK_MEM_POOL_MP4DEMUX_SIZE,           NOT_USE,    PNULL},
    //{(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MP4H263_DEC,    BLOCK_MEM_POOL_MP4H263_DEC_OFFSET,  BLOCK_MEM_POOL_MP4H263_DEC_SIZE,        NOT_USE,    PNULL},
    //{(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_JPEGDEC,        BLOCK_MEM_POOL_JPEGDEC_OFFSET,      BLOCK_MEM_POOL_JPEGDEC_SIZE,            NOT_USE,    PNULL},
#ifdef TTS_SUPPORT
#ifdef TTS_FILE_RES
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_TTSRESCATHEBUFFER, 	/*BLOCK_MEM_POOL_TTSRESCATHEBUFFER_OFFSET,*/	KiTTS_RESCATHEBUFFER_SIZE,			NOT_USE,	/*PNULL*/},		
#endif
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_TTSHEAPBUFFER, 		/*BLOCK_MEM_POOL_TTSHEAPBUFFER_OFFSET,*/	KiTTS_HEAP_BUFFER_SIZE,			NOT_USE,	/*PNULL*/},		
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_TTSVOICEDATA, 		/*BLOCK_MEM_POOL_TTSVOICEDATA_OFFSET,*/	KiTTS_DATA_BUFFER_SIZE,			NOT_USE,	/*PNULL*/},		
#endif
//============== MET MEX  start ===================
#ifdef MET_MEX_SUPPORT
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MET_MEX_MEM_INTERNAL, /*BLOCK_MEM_POOL_MET_MEX_MEM_INTERNAL_OFFSET,*/ BLOCK_MEM_POOL_MET_MEX_MEM_INTERNAL_SIZE, NOT_USE, /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MET_MEX_MEM_EXTERNAL, /*BLOCK_MEM_POOL_MET_MEX_MEM_EXTERNAL_OFFSET,*/ BLOCK_MEM_POOL_MET_MEX_MEM_EXTERNAL_SIZE, NOT_USE, /*PNULL*/},
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MET_MEX_MEM_RESERVED, /*BLOCK_MEM_POOL_MET_MEX_MEM_RESERVED_OFFSET, */BLOCK_MEM_POOL_MET_MEX_MEM_RESERVED_SIZE, NOT_USE, /*PNULL*/},
#endif // MET_MEX_SUPPORT
//============== MET MEX  end ===================

#ifdef MV_SUPPORT
    {(BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MV_RES,         /*BLOCK_MEM_POOL_MV_RES_OFFSET,*/       BLOCK_MEM_POOL_MV_RES_SIZE,              NOT_USE,    /*PNULL*/},
#endif
#ifdef MRAPP_SUPPORT
    {BLOCK_MEM_POOL_MRAPP,			__MRAPP_CFG_VAR_VMMEM_SIZE__,				NOT_USE         /*MMIMRAPP_BLMemCb*/},
#endif

    };

//可用大内存池
#ifndef WIN32
__align(32)
#endif 
 
//LOCAL uint32 BLOCK_POOL[TOTAL_BLOCK_POOL_SIZE/4+1]; /*lint !e506 */ //for BLOCK_POOL_END_ADD
//#define BLOCK_POOL_START_ADD &BLOCK_POOL   /*lint !e506 */
//#define BLOCK_POOL_END_ADD   ((uint32 *)&BLOCK_POOL+TOTAL_BLOCK_POOL_SIZE/4)  /*lint !e506 */

LOCAL uint32  s_block_addr_list[ARR_SIZE(mem_table)] = {0};


/*****************************************************************************/
//  Description:    Juge id is valid ....
//  Global resource dependence: 
//  Author: Daiel.ding
//  Note:
/*****************************************************************************/
LOCAL uint32 BL_CheckID(BLOCK_MEM_ID_E id)
{
    uint32 i = 0;
    for (i=0;i<ARR_SIZE(mem_table);i++)
    {
        if (id == mem_table[i].block_id)
        {     
            return i;
        }       
    }
    //SCI_TRACE_LOW:"BL_CheckID:invalid id = %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,BLOCK_MEM_456_112_2_18_1_59_22_0,(uint8*)"d",id);
    SCI_ASSERT(0); /*assert verified*/
    return FALSE ;/*lint !e527 */
}



/*****************************************************************************/
//  Description:    Juge id is valid ....
//  Global resource dependence: 
//  Author: Daiel.ding
//  Note:
/*****************************************************************************/
// LOCAL BOOLEAN BL_CheckMemCross(uint32 src_id, uint32 des_id)
// {
//     uint32 src_start = mem_table[src_id].block_offset ;
//     uint32 src_end   = mem_table[src_id].block_offset + mem_table[src_id].block_num;
//     uint32 des_start = mem_table[des_id].block_offset ;
//     uint32 des_end   = mem_table[des_id].block_offset + mem_table[des_id].block_num;
// 
//     uint32 total_len = (src_end - src_start) + (des_end - des_start);
//     uint32 physical_len = MAX(src_end, des_end) - MIN(src_start, des_start);
// 
//     SCI_ASSERT(src_id < ARR_SIZE(mem_table));
//     SCI_ASSERT(des_id < ARR_SIZE(mem_table));
// //---src      ===des
// 
// //          -------
// //       ====                    //Case one 
// //                ====           //Case two
// //            ===                    //Case three
// //       ===============       //Case four
// 
//     if(physical_len < total_len)
// //  if (((src_start < des_end)&&(src_start >= des_start)) //Case one
// //      ||((src_end <= des_end)&&(src_end > des_start))   //Case two
// //      ||((src_start <= des_start)&&(src_end >= des_end))      //Case threee
// //      ||((src_start > des_start)&&(src_end < des_end))    //Case four
// //      )
//     {    
//         return TRUE;   
//     }
//     else
//     {
//         return FALSE ;
//     }
// }
/*****************************************************************************/
//  Description:malloc block sci_alloca
//  Global resource dependence: 
//  Author: 
//  Note:
/*****************************************************************************/
LOCAL void * Block_Malloc(
                          BLOCK_MEM_ID_E      id,
                          uint32              size,
                          const char          *file,
                          uint32              line
                          )
{
    //uint32  j = 0;
    uint32 index = BL_CheckID(id);
    uint8* buf_ptr = PNULL;

    //SCI_TRACE_LOW:"Block_Malloc: %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,BLOCK_MEM_514_112_2_18_1_59_22_1,(uint8*)"d",id);

//     if(mem_table[index].block_mem_flag == ALLOC_BY_OTHERS)
//     {
//         SCI_TRACE_LOW("Block_Malloc:in used id = %d",id);
//         return NULL;
//     }

//     SCI_DisableIRQ ();
// 
//     for(j = 0 ; j < ARR_SIZE(mem_table); j++)
//     {
//         //查询是否和其他的 Block 内存空间重合，如果重合,标志其也被占用。
//         if(j!= index && BL_CheckMemCross(index,j))
//         {
//         
//             //If neighbor block has been used ,assert here ;
//             if (mem_table[j].block_mem_flag != NOT_USE)
//             {
//                 SCI_TRACE_LOW ("Neighbor block %d has been used !",j);
// 
//                 //占用的block带有回调函数，调用free block
//                 if (PNULL != mem_table[j].block_callback_ptr)
//                 {                   
//                     if ((*mem_table[j].block_callback_ptr)(id,BORROW_FROM_CURRENT_BLOCK))
//                     {                       
//                         if(BL_GetStatus(mem_table[j].block_id) != NOT_USE)
//                         {
//                             SCI_ASSERT(0);
//                         }
//                     }
//                     else
//                     {
//                         SCI_ASSERT (0);
//                     }
//                 }
//                 else 
//                 {                   
//                     //SCI_ASSERT (0);
//                     SCI_PASSERT(0, ("%d Neighbor block %d has been used !",index, j));
//                 }
//             }
//         }
//     }
// 
//     SCI_RestoreIRQ ();
    if (PNULL != s_block_addr_list[index])
    {
        //已经分配不用再分配
        buf_ptr = (uint8*)s_block_addr_list[index];
    }
    else
    {
        if (BLOCK_MALLOC_DEFAULT_SIZE == size||mem_table[index].block_num <= size)
        {
            buf_ptr = (uint8*)SCI_MallocApp( mem_table[index].block_num + 32, file, line );
        }
        else
        {
            buf_ptr = (uint8*)SCI_MallocApp( size + 32, file, line );
        }
        s_block_addr_list[index] = (uint32)buf_ptr;
		
    }
    if (PNULL != buf_ptr)
    {
       mem_table[index].block_mem_flag = ALLOC_BY_SELF;
	   buf_ptr = (((uint32)buf_ptr+7)/8)*8;/*lint !e64*/
	   
    }
    else
    {
        //SCI_TRACE_LOW:"Block_Malloc: alloc mem fail!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,BLOCK_MEM_586_112_2_18_1_59_22_2,(uint8*)"");
    }
    return buf_ptr;
}
/*****************************************************************************/
//  Description:malloc block
//  Global resource dependence: 
//  Author: 
//  Note:
/*****************************************************************************/
void *BL_MallocEx(
                  BLOCK_MEM_ID_E id,                          
                  const char     *file,
                  uint32         line
                  )
{
    //uint32  j = 0;
    uint32 index = BL_CheckID(id);
    uint8* buf_ptr = PNULL;

    //SCI_TRACE_LOW:"BL_Malloc: %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,BLOCK_MEM_602_112_2_18_1_59_22_3,(uint8*)"d",id);

//     if(mem_table[index].block_mem_flag == ALLOC_BY_OTHERS)
//     {
//         SCI_TRACE_LOW("BL_Malloc:in used id = %d",id);
//         return NULL;
//     }

//     SCI_DisableIRQ ();
// 
//     for(j = 0 ; j < ARR_SIZE(mem_table); j++)
//     {
//         //查询是否和其他的 Block 内存空间重合，如果重合,标志其也被占用。
//         if(j!= index && BL_CheckMemCross(index,j))
//         {
//         
//             //If neighbor block has been used ,assert here ;
//             if (mem_table[j].block_mem_flag != NOT_USE)
//             {
//                 SCI_TRACE_LOW ("Neighbor block %d has been used !",j);
// 
//                 //占用的block带有回调函数，调用free block
//                 if (PNULL != mem_table[j].block_callback_ptr)
//                 {                   
//                     if ((*mem_table[j].block_callback_ptr)(id,BORROW_FROM_CURRENT_BLOCK))
//                     {                       
//                         if(BL_GetStatus(mem_table[j].block_id) != NOT_USE)
//                         {
//                             SCI_ASSERT(0);
//                         }
//                     }
//                     else
//                     {
//                         SCI_ASSERT (0);
//                     }
//                 }
//                 else 
//                 {                   
//                     //SCI_ASSERT (0);
//                     SCI_PASSERT(0, ("%d Neighbor block %d has been used !",index, j));
//                 }
//             }
//         }
//     }
// 
//     SCI_RestoreIRQ ();

    if (PNULL != s_block_addr_list[index])
    {
        //已经分配不用再分配
        buf_ptr = (uint8*)s_block_addr_list[index];
    }
    else
    {

        buf_ptr = (uint8*)SCI_MallocApp( mem_table[index].block_num + 32, file, line);
        s_block_addr_list[index] = (uint32)buf_ptr;
		
    }
    if (PNULL != buf_ptr)
    {
       mem_table[index].block_mem_flag = ALLOC_BY_SELF;
	   buf_ptr = (((uint32)buf_ptr+7)/8)*8;/*lint !e64*/
	   
    }
    else
    {
        //SCI_TRACE_LOW:"BL_Malloc: alloc mem fail!"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,BLOCK_MEM_669_112_2_18_1_59_22_4,(uint8*)"");
    }
    return buf_ptr;
}
/*****************************************************************************/
//  Description:    Judge whether mem are crossed
//  Global resource dependence: 
//  Author: gang.tong
//  Note:
/*****************************************************************************/
// BOOLEAN BL_CheckMemCrossByID(BLOCK_MEM_ID_E src_id, BLOCK_MEM_ID_E des_id)
// {
//     uint32 src_tab_id = 0;
//     uint32 des_tab_id = 0;
//     src_tab_id = BL_CheckID(src_id);
//     des_tab_id = BL_CheckID(des_id);
//     return BL_CheckMemCross(src_tab_id, des_tab_id);
// }

/*****************************************************************************/
//  Description:free block 
//  Global resource dependence: 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void BL_Free(
                     BLOCK_MEM_ID_E     id
                     )
{
    uint32 index = BL_CheckID(id);
    //uint32       j = 0;

    //SCI_TRACE_LOW:"BL_Free: %d"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,BLOCK_MEM_697_112_2_18_1_59_22_5,(uint8*)"d",id);

    if(NOT_USE == mem_table[index].block_mem_flag )
    {
        //SCI_TRACE_LOW:"BL_Free:unused id = %d"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,BLOCK_MEM_701_112_2_18_1_59_22_6,(uint8*)"d",id);
        return;        
    }
//     else if(ALLOC_BY_OTHERS == mem_table[index].block_mem_flag)
//     {
//         SCI_PASSERT(0, ("BL_Free:can't free status = ALLOC_BY_OTHERS"));
//     }
    else
    {
        mem_table[index].block_mem_flag = NOT_USE;
        
        if(s_block_addr_list[index])
        {
            SCI_FREE((void*)s_block_addr_list[index]);  /*lint !e63*/
            s_block_addr_list[index] = 0;
        }
        
//         SCI_DisableIRQ ();
// 
//         for(j = 0 ; j < ARR_SIZE(mem_table); j++)
//         {
//             //查询是否和其他的 Block 内存空间重合，如果重合,标志其也被占用。
//             if(j!= index && BL_CheckMemCross(index,j))
//             {
//                 //复用的block 带有回调函数，归还后进行些处理
//                 if ((PNULL != mem_table[j].block_callback_ptr)
//                     && (mem_table[j].block_mem_flag == NOT_USE))
//                 {
//                     SCI_RestoreIRQ ();
//                     (*mem_table[j].block_callback_ptr)(id,GIVEBACK_TO_CURRENT_BLOCK);
//                     return;
//                 }
//             }
//         }  
//         SCI_RestoreIRQ ();
    }
}

/*****************************************************************************/
//  Description:get block status
//  Global resource dependence: 
//  Author: 
//  Note:
/*****************************************************************************/
BLOCK_MEM_STATUS_E BL_GetStatus(BLOCK_MEM_ID_E id)
{

    //uint32              i = 0;
    uint32              index = BL_CheckID(id);
    BLOCK_MEM_STATUS_E  block_status = NOT_USE;

    block_status = mem_table[index].block_mem_flag;

//     if (NOT_USE == block_status)
//     {
//         SCI_DisableIRQ();
//         //查询是否和其他的 Block 内存空间重合
//         for(i=0 ; i<ARR_SIZE(mem_table); i++)
//         {
//             if(i!= index && BL_CheckMemCross(index,i))
//             {
//                 if (NOT_USE != mem_table[i].block_mem_flag)
//                 {
//                     SCI_TRACE_LOW("BL_GetStatus ALLOC_BY_OTHERS %d",i);
//                     block_status = ALLOC_BY_OTHERS;
//                     break;
//                 }
//             }
//         }
//         SCI_RestoreIRQ();
//     }

    return (block_status);
  
}


/*****************************************************************************/
//  Description:get block size
//  Global resource dependence: 
//  Author: 
//  Note:
/*****************************************************************************/
uint32 BL_GetSize(BLOCK_MEM_ID_E id)
{
    uint32 index = BL_CheckID(id);

    return mem_table[index].block_num;
}

/*****************************************************************************/
//  Description:get block poiner
//  Global resource dependence: 
//  Author: jassmine
//  Note:
/*****************************************************************************/
void * BL_GetBlockPtr(BLOCK_MEM_ID_E id)
{
    uint32 index = BL_CheckID(id);
    uint32 buf_addr = s_block_addr_list[index];
    
    //8字节对齐，需要跟BL_Malloc/Block_Malloc保持一致
    buf_addr = (((uint32)buf_addr+7)/8)*8;/*lint !e64*/
    return (void*)buf_addr;
//    return (void*) s_block_addr_list[index];
}

/*****************************************************************************/
//  Description: Callback by other block_mem alloc fail, when 3d using
//  Global resource dependence: 
//  Author: bin.ji
//  Note:
/*****************************************************************************/
// LOCAL BOOLEAN CallbackBy3D(
//                            BLOCK_MEM_ID_E id,
//                            BLOCK_MEM_RELATION_STATUS_E relation_info
//                            )
// {
//     if (BORROW_FROM_CURRENT_BLOCK == relation_info)
//     {
//         MMI_BL_FREE((BLOCK_MEM_ID_E)BLOCK_MEM_POOL_MMI_3D_DESKTOP);
//     }
// 
//     return TRUE;
// }

/*****************************************************************************/
//  Description:malloc block sci_alloc_app
//  Global resource dependence: 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void * MMI_BL_Malloc(
                    BLOCK_MEM_ID_E      id,
                    const char          *file,
                    uint32              line
                    )
{
    SCI_ASSERT(PNULL!=file); /*assert verified*/

    //SCI_TRACE_LOW:"MMI_BL_Malloc: %d,%s, line=%ld"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,BLOCK_MEM_831_112_2_18_1_59_23_7,(uint8*)"dsd",id,file,line);

    return BL_MallocEx(id, file, line);
}

/*****************************************************************************/
//  Description:malloc block sci_alloca
//  Global resource dependence: 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void * MMI_BL_MallocExt(
                    BLOCK_MEM_ID_E      id,
                    uint32              size,
                    const char          *file,
                    uint32              line
                    )
{
    SCI_ASSERT(PNULL!=file); /*assert verified*/

    //SCI_TRACE_LOW:"MMI_BL_Malloc: %d,size=%d,%s, line=%ld"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,BLOCK_MEM_849_112_2_18_1_59_23_8,(uint8*)"ddsd",id,size,file,line);

    return Block_Malloc(id,size,file,line);
}

/*****************************************************************************/
//  Description:malloc block sci_alloca
//  Global resource dependence: 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void * MMI_BL_MallocA(
                    BLOCK_MEM_ID_E      id,
                    const char          *file,
                    uint32              line
                    )
{
    SCI_ASSERT(PNULL!=file); /*assert verified*/

    //SCI_TRACE_LOW:"MMI_BL_Malloc: %d,%s, line=%ld"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,BLOCK_MEM_866_112_2_18_1_59_23_9,(uint8*)"dsd",id,file,line);

    return Block_Malloc(id,BLOCK_MALLOC_DEFAULT_SIZE,file,line);
}
/*****************************************************************************/
//  Description:free block 
//  Global resource dependence: 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void MMI_BL_Free(
                     BLOCK_MEM_ID_E     id,
                     const char         *file,
                     uint32             line
                     )
{
    SCI_ASSERT(PNULL!=file); /*assert verified*/

    //SCI_TRACE_LOW:"MMI_BL_Free: %d,%s, line=%ld"
    SCI_TRACE_ID(TRACE_TOOL_CONVERT,BLOCK_MEM_882_112_2_18_1_59_23_10,(uint8*)"dsd",id,file,line);
    
    BL_Free(id);
}

#if 0

typedef struct 
{
    BLOCK_MEM_ID_E      block_id;
    uint8               block_string[30];
}BLOCK_MEM_STR_T;

/*****************************************************************************/
//  Description:print mem block info
//  Global resource dependence: 
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void PrintMemBlock(void)
{
    uint32 i = 0;
    uint32 j = 0;
    uint32 k = 0;
    uint32 written = 0;
    uint32 strmem_len = 0;
    SFS_HANDLE  m_handle = NULL;
#ifdef MAINLCD_SIZE_176X220
    uint8 ascii_pathname[100]="E:\\qcif_memblock.txt";
#elif defined MAINLCD_SIZE_240X320
    uint8 ascii_pathname[100]="E:\\qvga_memblock.txt";
#elif defined MAINLCD_SIZE_240X400
    uint8 ascii_pathname[100]="E:\\wqvga_memblock.txt";
#elif defined MAINLCD_SIZE_320X480
    uint8 ascii_pathname[100]="E:\\hvga_memblock.txt";
#else
#error
#endif

    uint8 ucs2_filename[100] = {0};
    SFS_ERROR_E ret = SFS_NO_ERROR;
    uint8 strmem[500]={0};
    uint32 max_len = 0;
    wchar unknown_str [] = {'U','N','K','N','O','W','N','I','D',0};
    uint32 len = 0;
    BLOCK_MEM_STR_T mem_table_str[] =
    {
        {BLOCK_MEM_POOL_REF,            "BLOCK_MEM_POOL_REF"        },
        {BLOCK_MEM_POOL_SFS,            "BLOCK_MEM_POOL_SFS"        },
        {BLOCK_MEM_POOL_REF_DC,         "BLOCK_MEM_POOL_REF_DC"     },
        {BLOCK_MEM_POOL_MP4ENC,         "BLOCK_MEM_POOL_MP4ENC"     },
        {BLOCK_MEM_POOL_REF_DV,         "BLOCK_MEM_POOL_REF_DV"     },
        {BLOCK_MEM_POOL_DC,             "BLOCK_MEM_POOL_DC"         },
        {BLOCK_MEM_POOL_WAP,            "BLOCK_MEM_POOL_WAP"        },
        {BLOCK_MEM_POOL_DC_OSD,         "BLOCK_MEM_POOL_DC_OSD"     },
        {BLOCK_MEM_POOL_MP4SNAPSHOT,    "BLOCK_MEM_POOL_MP4SNAPSHOT"},
        {BLOCK_MEM_POOL_MMS,            "BLOCK_MEM_POOL_MMS"        },
        {BLOCK_MEM_POOL_JAVA,           "BLOCK_MEM_POOL_JAVA"       },
        {BLOCK_MEM_POOL_OCR,            "BLOCK_MEM_POOL_OCR"        },
        {BLOCK_MEM_POOL_GPS,            "BLOCK_MEM_POOL_GPS"        },
        {BLOCK_MEM_POOL_STREAMING,      "BLOCK_MEM_POOL_STREAMING"  },
        {BLOCK_MEM_POOL_PIC_SOURCE,     "BLOCK_MEM_POOL_PIC_SOURCE" },
        {BLOCK_MEM_POOL_PIC_PROCESS,    "BLOCK_MEM_POOL_PIC_PROCESS"},
        {BLOCK_MEM_POOL_PIC_TARGET,     "BLOCK_MEM_POOL_PIC_TARGET" },
        {BLOCK_MEM_POOL_PIC_UNDO,       "BLOCK_MEM_POOL_PIC_UNDO"   },
        {BLOCK_MEM_POOL_KURO,           "BLOCK_MEM_POOL_KURO"       },
        {BLOCK_MEM_POOL_MMI_3D_DESKTOP,         "BLOCK_MEM_POOL_MMI_3D_DESKTOP"     },
        {BLOCK_MEM_POOL_MMI_3D_MENU,         "BLOCK_MEM_POOL_MMI_3D_MENU"     },
        {BLOCK_MEM_POOL_MMI_3D_LIST,         "BLOCK_MEM_POOL_MMI_3D_LIST"     },
        {BLOCK_MEM_POOL_MMI_SPRITE,     "BLOCK_MEM_POOL_MMI_SPRITE" },
        {BLOCK_MEM_POOL_CAF_IMAGE,      "BLOCK_MEM_POOL_CAF_IMAGE"  },
        {BLOCK_MEM_POOL_CAF_RESOURCE,   "BLOCK_MEM_POOL_CAF_RESOURCE"},
        {BLOCK_MEM_POOL_MTV_LCDTV,      "BLOCK_MEM_POOL_MTV_LCDTV"  },
        {BLOCK_MEM_POOL_MTV_EXTPCM,     "BLOCK_MEM_POOL_MTV_EXTPCM" },
        {BLOCK_MEM_POOL_MTV_FIRMWARE,   "BLOCK_MEM_POOL_MTV_FIRMWARE"},
        {BLOCK_MEM_POOL_MTV_GZIPBUF,    "BLOCK_MEM_POOL_MTV_GZIPBUF"},
        {BLOCK_MEM_POOL_MTV_RGB,        "BLOCK_MEM_POOL_MTV_RGB"    },
        {BLOCK_MEM_POOL_MTV_YUV422,     "BLOCK_MEM_POOL_MTV_YUV422" },
        {BLOCK_MEM_POOL_MTV_Y,          "BLOCK_MEM_POOL_MTV_Y"      },
        {BLOCK_MEM_POOL_MTV_UV,         "BLOCK_MEM_POOL_MTV_UV"     },
        {BLOCK_MEM_POOL_MTV_JPG,        "BLOCK_MEM_POOL_MTV_JPG"    },
        {BLOCK_MEM_POOL_MTV_JPGENCODE,  "BLOCK_MEM_POOL_MTV_JPGENCODE"},
        {BLOCK_MEM_POOL_ROTATE_OSD,     "BLOCK_MEM_POOL_ROTATE_OSD" }
   };
 
    GUI_GBToWstr(ucs2_filename,ascii_pathname,strlen((char*)ascii_pathname));
    m_handle = SFS_CreateFile(ucs2_filename, SFS_MODE_WRITE|SFS_MODE_APPEND, NULL, NULL);
    if(m_handle != NULL)
    {
        for(i=0 ; i <sizeof(mem_table)/sizeof(BLOCK_MEM_T); i++)
        {
            for(j=0;j<(mem_table[i].block_offset/1024/10+mem_table[i].block_num/1024/10);j++);
            if(max_len<j)
            {
                max_len = j;
                k = i;
            }
        }
        SCI_MEMSET(strmem,0x00,500);
#ifdef MAINLCD_SIZE_176X220
        sprintf(strmem+max_len/2-9,"QCIF TOTAL SIZE =%5dK",(mem_table[k].block_num+mem_table[k].block_offset)/1024);
#elif defined MAINLCD_SIZE_240X320
        sprintf(strmem+max_len/2-9,"QVGA TOTAL SIZE =%5dK",(mem_table[k].block_num+mem_table[k].block_offset)/1024);
#elif defined MAINLCD_SIZE_240X400
        sprintf(strmem+max_len/2-9,"WQVGA TOTAL SIZE =%5dK",(mem_table[k].block_num+mem_table[k].block_offset)/1024);
#elif defined MAINLCD_SIZE_320X480
        sprintf(strmem+max_len/2-9,"HVGA TOTAL SIZE =%5dK",(mem_table[k].block_num+mem_table[k].block_offset)/1024);
#else
#error
#endif
        for(i=0 ; i <=max_len; i++)
        {
            if(0x00==strmem[i])
            {
                strmem[i] = '*';
            }
        }
        strmem[max_len+1]='\r';
        strmem[max_len+2]='\n';

        ret=SFS_WriteFile(m_handle,strmem,strlen(strmem),&written,NULL);
        
        for(i=0 ; i <sizeof(mem_table)/sizeof(BLOCK_MEM_T); i++)
        {
            if (0 ==mem_table[i].block_num )
            {
                continue;
            }
            SCI_MEMSET(strmem,0x00,500);

            len = mem_table[i].block_num/1024/10/2+mem_table[i].block_offset/1024/10;
            
            sprintf(strmem+len-1,"%d",mem_table[i].block_num/1024);
            
            strmem[0] = '|';
            for(j=1; j<max_len;j++)
            {
                
                if (j>=mem_table[i].block_offset/1024/10 && j<mem_table[i].block_num/1024/10+mem_table[i].block_offset/1024/10)
                {
                    if(0x00==strmem[j]||0x20==strmem[j])
                    {
                        strmem[j] = '*';
                    }
                }
                else
                {
                    strmem[j] = '-';
                }
            }
            strmem[max_len] = '|';
            
            for(k=0;k<sizeof(mem_table_str)/sizeof(BLOCK_MEM_STR_T);k++)
            {
                if(mem_table_str[k].block_id == mem_table[i].block_id)
                {
                    break;
                }
            }

            if(k != sizeof(mem_table_str)/sizeof(BLOCK_MEM_STR_T))
            {
                sprintf(strmem+max_len+1," ID=%d\t%s\r\n",mem_table[i].block_id,mem_table_str[k].block_string);
            }
            else
            {
                sprintf(strmem+max_len+1," ID=%d\t%s\r\n",mem_table[i].block_id,unknown_str);
            }

            ret=SFS_WriteFile(m_handle,strmem,strlen(strmem),&written,NULL);
        }
        for(i=0 ; i <sizeof(mem_table)/sizeof(BLOCK_MEM_T); i++)
        {
            for(j=0;j<sizeof(mem_table_str)/sizeof(BLOCK_MEM_STR_T);j++)
            {
                if(mem_table_str[j].block_id == mem_table[i].block_id)
                {
                    break;
                }
            }
            if(j != sizeof(mem_table_str)/sizeof(BLOCK_MEM_STR_T))
            {
                sprintf(strmem,"\r\nID=%3d\tOFFSET=%5d\tSIZE=%4dK\t%s",mem_table[i].block_id,
                mem_table[i].block_offset/1024,mem_table[i].block_num/1024,mem_table_str[j].block_string);
            }
            else
            {
                sprintf(strmem,"\r\nID=%3d\tOFFSET=%5d\tSIZE=%4dK\t%s",mem_table[i].block_id,
                mem_table[i].block_offset/1024,mem_table[i].block_num/1024,unknown_str);
            }
            ret=SFS_WriteFile(m_handle,strmem,strlen(strmem),&written,NULL);
        }
        SFS_CloseFile(m_handle);
    } 
}
#endif
/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif
