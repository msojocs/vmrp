/*****************************************************************************
** File Name:      skyqq_mmi_image.h                                         *
** Author:                                                                   *
** Date:           02/2010                                                   *
** Copyright:      sky-mobi.                                                 *
** Description:    This file is used to describe skyqq image resource        *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** Create by Spreadtrum Resource Editor tool                                 *
******************************************************************************/

#ifndef _SKYQQ_MMI_IMAGE_H_
#define _SKYQQ_MMI_IMAGE_H_

#include "sci_types.h"
#include "mmi_imagefun.h"
#include "mmi_module.h"

#ifdef __cplusplus
extern   "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// Image Resource ID
///////////////////////////////////////////////////////////////////////////////

#define MACRO_MDU_TYPE  MACRO_MDU_IMAGE
#include "macro_mdu_def.h"

typedef enum SKYQQ_MMI_IMAGE_ID_E
{
	SKYQQ_IMAGE_NULL = (MMI_MODULE_SKYQQ << 16) | MMI_SYS_RES_FLAG,
#include "skyqq_mdu_def.h"
	SKYQQ_IMAGE_MAX_ID
} SKYQQ_MMI_IMAGE_ID_E;

#undef MACRO_MDU_TYPE

#ifdef __cplusplus
}
#endif

#endif //_SKYQQ_MMI_IMAGE_H_
