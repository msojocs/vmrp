/*****************************************************************************
** File Name:      dmenu_mmi_text.h                                         *
** Author:                                                                   *
** Date:           11/2009                                                   *
** Copyright:      sky-mobi.                                                 *
** Description:    This file is used to describe mrapp image resource        *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** Create by Spreadtrum Resource Editor tool                                 *
******************************************************************************/

#ifndef _DMENU_MMI_TEXT_H_
#define _DMENU_MMI_TEXT_H_

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

typedef enum DMENU_MMI_TEXT_ID_E
{
	DMENU_TEXT_NULL = ( MMI_MODULE_DMENU << 16 ) | MMI_SYS_RES_FLAG,
#include "dmenu_mdu_def.h"
	DMENU_TXT_MAX
}DMENU_MMI_TEXT_ID_E;

#undef MACRO_MDU_TYPE

#ifdef __cplusplus
}
#endif
#endif // _DMENU_MMI_TEXT_H_

