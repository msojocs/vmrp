#ifndef __SKY_MM_DEF__
#define  __SKY_MM_DEF__

/********************  Release Version Note**************************
*
*2010-08-13 : River Wei.   VerNo.
           1. 修改3个task为同一个
           2. 修改文件名，代码整理
           3. 将前期的一些bug都做修复合并到一个版本
*
*
*************************************************************/


#define MTK_BIG_VERSION 0x11B

//#if (MTK_BIG_VERSION >= 0x10A)
#define MTK_VERSION		(0x11B1212)         
//#else
//#include "mmiDsmMain.h"
//#endif

#define _SKY_MM_PURE_AUDIO_OR_VIDEO_

/*打开计算播放性能的宏*/
//#define SKY_MM_CALC_FPS
#define __SKYMM__DISPLAY_IN_MMI__

#if (MTK_BIG_VERSION <= 0) 
#error please define macro MTK_BIG_VERSION as mtk big verison
#endif

#endif
