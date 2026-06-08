/*****************************************************************************
** File Name:      mrapp_mmi_text.h                                         *
** Author:                                                                   *
** Date:           11/2009                                                   *
** Copyright:      sky-mobi.                                                 *
** Description:    This file is used to describe mrapp image resource        *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** Create by Spreadtrum Resource Editor tool                                 *
******************************************************************************/

#ifndef _MRAPP_MMI_TEXT_H_
#define _MRAPP_MMI_TEXT_H_

#include "sci_types.h"
#include "mmi_module.h"

#ifdef __cplusplus
extern   "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// ÎÄ×Ö×ÊÔ´ID
///////////////////////////////////////////////////////////////////////////////

#define MACRO_MDU_TYPE  MACRO_MDU_TEXT
#include "macro_mdu_def.h"

typedef enum MRAPP_MMI_TEXT_ID_E
{
	MRAPP_TEXT_NULL = ( MMI_MODULE_MRAPP << 16 ) | MMI_SYS_RES_FLAG,
#include "mrapp_mdu_def.h"
	MRAPP_TXT_MAX
} MRAPP_MMI_TEXT_ID_E;

#undef MACRO_MDU_TYPE

#ifdef __cplusplus
}
#endif
#endif // _MRAPP_MMI_TEXT_H_
