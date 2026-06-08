/******************************************************************************
 ** File Name:      block_mem.h                                               *
 ** Author:         Richard Yang                                              *
 ** DATE:           05/09/2005                                                *
 ** Copyright:      2005 Spreadtrum, Incoporated. All Rights Reserved.         *
 ** Description:    This file defines the basic operation interfaces of       *
 **                 large block table for different applications.             *
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
#ifndef _BLOCK_MEM_H_
#define _BLOCK_MEM_H_

#include "os_api.h"
#include "block_mem_def.h"
/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
    extern   "C" 
    {
#endif

/* REF */
#ifdef _ZOOM_SUPPORT_
#define BLOCK_MEM_POOL_REF_DC_SIZE BLOCK_MEM_POOL_REF_DC_SIZE_ZOOM
#else
#define BLOCK_MEM_POOL_REF_DC_SIZE BLOCK_MEM_POOL_REF_DC_SIZE_NO_ZOOM
#endif
#define BLOCK_POOL_SIZE         1024 

/* To increase the speed of special effect, we shoule keep the buffer 256 bytes align, so we add 256 bytes 
    for for align, and two buffers is 256*2 */
#define BLOCK_MEM_POOL_MMI_DESKTOP3D_SIZE          (MMI_MAINSCREEN_WIDTH*MMI_MAINSCREEN_HEIGHT * 2 * 2 + 256*2)
#define BLOCK_MEM_POOL_MMI_MENU3D_SIZE          (MMI_MAINSCREEN_WIDTH*MMI_MAINSCREEN_HEIGHT * 2 * 3 + 256*2)

#ifdef MAINLCD_SIZE_176X220
#define BLOCK_MEM_UINT8_SCREEN_BUFFER_SIZE   (38 * 1024)
#elif defined MAINLCD_SIZE_240X320        
#define BLOCK_MEM_UINT8_SCREEN_BUFFER_SIZE   (75 * 1024)
#elif defined MAINLCD_SIZE_128X64
#define BLOCK_MEM_UINT8_SCREEN_BUFFER_SIZE   (8 * 1024)
#elif defined MAINLCD_SIZE_240X400        
#define BLOCK_MEM_UINT8_SCREEN_BUFFER_SIZE   (94 * 1024)
#elif defined MAINLCD_SIZE_320X480        
#define BLOCK_MEM_UINT8_SCREEN_BUFFER_SIZE   (150 * 1024)
#else   //240*400
#define BLOCK_MEM_UINT8_SCREEN_BUFFER_SIZE   (94 * 1024)
#endif
//============== MET MEX  start ===================
#ifdef MET_MEX_SUPPORT
#include "MexLib.h"
#define BLOCK_MEM_POOL_MET_MEX_MEM_INTERNAL_SIZE      (240*1024)
#ifdef MEX_G7_UI_SUPPORT
#define BLOCK_MEM_POOL_MET_MEX_MEM_EXTERNAL_SIZE      (1024*1024)
#define BLOCK_MEM_POOL_MET_MEX_MEM_RESERVED_SIZE      (0)		//G7 UI 第2,3块内存合并为一块了
#else
#define BLOCK_MEM_POOL_MET_MEX_MEM_EXTERNAL_SIZE      (160*1024)
#define BLOCK_MEM_POOL_MET_MEX_MEM_RESERVED_SIZE      (680*1024)
#endif
#define BLOCK_MEM_POOL_MET_MEX_MEM_INTERNAL_OFFSET    (0)
#define BLOCK_MEM_POOL_MET_MEX_MEM_EXTERNAL_OFFSET    (BLOCK_MEM_POOL_MET_MEX_MEM_INTERNAL_OFFSET + BLOCK_MEM_POOL_MET_MEX_MEM_INTERNAL_SIZE)
#define BLOCK_MEM_POOL_MET_MEX_MEM_RESERVED_OFFSET    (BLOCK_MEM_POOL_MET_MEX_MEM_EXTERNAL_OFFSET + BLOCK_MEM_POOL_MET_MEX_MEM_EXTERNAL_SIZE)
#endif // MET_MEX_SUPPORT
//============== MET MEX  end ===================

//所有内存ID都在这儿定义，ID必须不同
typedef enum
{
    BLOCK_MEM_POOL_WAP = BLOCK_MEM_POOL_MMI,    // mmi_wap.c                        s_wap_memory_arr        MMIWAP_WAP_MEMORY_SIZE(1M)
    BLOCK_MEM_POOL_DC_OSD,                          // for dc
    BLOCK_MEM_POOL_MP4SNAPSHOT,
    BLOCK_MEM_POOL_MMS,                         // for mms
    BLOCK_MEM_POOL_JAVA,                        //for java
#ifdef JAVA_SUPPORT_MYRIAD
    BLOCK_MEM_POOL_DEL_JAVA,
#endif
    BLOCK_MEM_POOL_OCR,
    BLOCK_MEM_POOL_GPS,
    BLOCK_MEM_POOL_STREAMING,//for streaming
    BLOCK_MEM_POOL_PIC_PROCESS,
    BLOCK_MEM_POOL_PIC_UNDO,
    BLOCK_MEM_POOL_PIC_SOURCE,
    BLOCK_MEM_POOL_PIC_TARGET,
    BLOCK_MEM_POOL_KURO,
    BLOCK_MEM_POOL_MMI_3D_DESKTOP,    //3D翻转特效
    BLOCK_MEM_POOL_MMI_3D_ICON_MENU,    //3D特效
    BLOCK_MEM_POOL_MMI_3D_SECOND_LIST, //second menu 3d effect.    
    BLOCK_MEM_POOL_MMI_SPRITE,  //桌面精灵
    BLOCK_MEM_POOL_CAF_IMAGE,     //动态库可执行文件
    BLOCK_MEM_POOL_CAF_RESOURCE,  //动态库资源文件
    BLOCK_MEM_POOL_MTV_LCDTV,
    BLOCK_MEM_POOL_MTV_EXTPCM,
    BLOCK_MEM_POOL_MTV_FIRMWARE,
    BLOCK_MEM_POOL_MTV_GZIPBUF,
    BLOCK_MEM_POOL_MTV_RGB,
    BLOCK_MEM_POOL_MTV_YUV422,
    BLOCK_MEM_POOL_MTV_Y,
    BLOCK_MEM_POOL_MTV_UV,
    BLOCK_MEM_POOL_MTV_JPG,
    BLOCK_MEM_POOL_MTV_JPGENCODE,
    BLOCK_MEM_POOL_ROTATE_OSD,
#ifdef TTS_SUPPORT
#ifdef TTS_FILE_RES
    BLOCK_MEM_POOL_TTSRESCATHEBUFFER,
#endif
    BLOCK_MEM_POOL_TTSHEAPBUFFER,
    BLOCK_MEM_POOL_TTSVOICEDATA,
#endif
#ifdef MRAPP_SUPPORT
	BLOCK_MEM_POOL_MRAPP,
#endif
	//============== MET MEX  start ===================
#ifdef MET_MEX_SUPPORT
		BLOCK_MEM_POOL_MET_MEX_MEM_INTERNAL,
		BLOCK_MEM_POOL_MET_MEX_MEM_EXTERNAL,
		BLOCK_MEM_POOL_MET_MEX_MEM_RESERVED,
#endif // MET_MEX_SUPPORT
	//============== MET MEX  end ===================

    BLOCK_MEM_POOL_MMI_MAX = BLOCK_MEM_POOL_MAX
}MMI_BLOCK_MEM_ID_E;

#define MMI_BL_MALLOC(_ID)          MMI_BL_Malloc(_ID, __FILE__, __LINE__)
#define MMI_BL_MALLOCE(_ID,_SIZE)   MMI_BL_MallocExt(_ID,_SIZE, __FILE__, __LINE__)
#define MMI_BL_MALLOCA(_ID)         MMI_BL_MallocA(_ID, __FILE__, __LINE__)
#define MMI_BL_FREE(_ID)            MMI_BL_Free(_ID, __FILE__, __LINE__)

/*****************************************************************************/
//  Description:    Judge whether mem are crossed
//  Global resource dependence: 
//  Author: gang.tong
//  Note:
/*****************************************************************************/
//PUBLIC BOOLEAN BL_CheckMemCrossByID(BLOCK_MEM_ID_E src_id, BLOCK_MEM_ID_E des_id);

/*****************************************************************************/
//  Description:get block poiner
//  Global resource dependence: 
//  Author: jassmine
//  Note:
/*****************************************************************************/
PUBLIC void * BL_GetBlockPtr(BLOCK_MEM_ID_E id);

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
                    );

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
                    );
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
                    );
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
                     );
/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif

#endif
