/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2010
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *  hccall.res
 *
 * Project:
 * --------
 *  hccall
 *
 * Description:
 * ------------
 *  This file defines the resource ID for the idle screen application.
 *
 * Author:
 * -------
 * 
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * 
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#include "MMI_features.h"
#include "CustResDef.h"
#include "mrp_features.h"

#define DSM_RES_PATH            "..\\\\..\\\\Customer\\\\Images\\\\DSM"                       

#ifdef __MMI_MAINLCD_240X320__
#define DSM_WEATHER_RES_PATH            "..\\\\..\\\\Customer\\\\Images\\\\DSM\\\\Weather\\\\240"   
#elif defined(__MMI_MAINLCD_176X220__)
#define DSM_WEATHER_RES_PATH            "..\\\\..\\\\Customer\\\\Images\\\\DSM\\\\Weather\\\\176" 
#else
#define DSM_WEATHER_RES_PATH            "..\\\\..\\\\Customer\\\\Images\\\\DSM\\\\Weather\\\\240" 
#endif

#ifndef __MMI_MAINLCD_480X800__
#define DSM_LIST_RES_ICONS_PATH             DSM_RES_PATH"\\\\list_icon\\\\48"
#else
#define DSM_LIST_RES_ICONS_PATH             DSM_RES_PATH"\\\\list_icon\\\\72"
#endif

<?xml version="1.0" encoding="UTF-8"?>
<APP id="VAPP_HCCALL" package_name="native.mtk.hccall" name="STR_ID_HCCALL" img="IMG_ID_HCCALL_MAIN" launch="vapp_hccall_launch" type="venus">
#ifdef __HC_CALL_NEW__
    <!----- Include Area ------------------------------------------------------>
    <INCLUDE file="GlobalResDef.h"/>
		<INCLUDE file="GlobalMenuItems.h"/>
    		
    <!----- String Resource Area ---------------------------------------------->
    <STRING id="STR_ID_HCCALL"/>
    <STRING id="STR_ID_HCCALL_PHB"/>
    <STRING id="STR_ID_HCCALL_HISTORYCALL"/>
    <STRING id="STR_ID_HCCALL_LOC_SEARCH"/>
    <STRING id="STR_ID_HCCALL_SECTION_NUM"/>
    <STRING id="STR_ID_HCCALL_INCOMMON_NUM"/>
    <STRING id="STR_ID_HCCALL_FUN_SET"/>
    <STRING id="STR_ID_HCCALL_SETTING"/>
    <STRING id="STR_ID_HCCALL_NETWORK_SETTING"/>
    <STRING id="STR_ID_HCCALL_NETWORK_SETTING_CMNET"/>
    <STRING id="STR_ID_HCCALL_NETWORK_SETTING_CMWAP"/>
    <STRING id="STR_ID_HCCALL_NETWORK_SETTING_WIFI"/>
    <STRING id="STR_ID_HCCALL_SIM_SETTING"/>
    <STRING id="STR_ID_HCCALL_SIM_SETTING_CARD1"/>
    <STRING id="STR_ID_HCCALL_SIM_SETTING_CARD2"/>
    <STRING id="STR_ID_HCCALL_SIM_SETTING_CARD3"/>
    <STRING id="STR_ID_HCCALL_SIM_SETTING_CARD4"/>
    <STRING id="STR_ID_HCCALL_FUNCTION_SETTING"/>
    <STRING id="STR_ID_HCCALL_FUNCTION_SETTING_ON"/>
    <STRING id="STR_ID_HCCALL_FUNCTION_SETTING_OFF"/>
    <STRING id="STR_ID_HCCALL_ABOUT"/>         
    <STRING id="STR_ID_HCCALL_ABOUT_INFOR"/>
    <STRING id="STR_ID_HCCALL_FUN_SET_NOTIFY"/>
    <STRING id="STR_ID_HCCALL_FUN_SET_CATIP"/>
    <STRING id="STR_ID_HCCALL_FUN_SET_INCOMING"/>
    <STRING id="STR_ID_HCCALL_FUN_SET_OUTGOING"/>
    <STRING id="STR_ID_HCCALL_FUN_SET_HISTORYSHOW"/>
    <STRING id="STR_ID_HCCALL_FUN_SET_UPDATE"/>
    <STRING id="STR_ID_HCCALL_FUN_SET_ON"/>         
    <STRING id="STR_ID_HCCALL_FUN_SET_OFF"/>       
    <STRING id="STR_ID_HCCALL_LOC_SEARCH_TITLE"/>
    <STRING id="STR_ID_HCCALL_LOC_SEARCHERROR"/>
    <STRING id="STR_ID_HCCALL_DOMESTIC_SECTION"/>
    <STRING id="STR_ID_HCCALL_INCOMMON_NUM_INFOR"/>
    <STRING id="STR_ID_HCCALL_SECOND_INCOMMON_NUM_INFOR"/>
    <STRING id="STR_ID_HCCALL_INCOMMON_NUM_POLICE"/>
    <STRING id="STR_ID_HCCALL_HELP"/>  
    <STRING id="STR_ID_HCCALL_BILL"/>
    <STRING id="STR_ID_HCCALL_ALERT"/>
    <STRING id="STR_ID_HCCALL_MANUAL"/>  
    <STRING id="STR_ID_HCCALL_PAY"/>
    <STRING id="STR_ID_HCCALL_ALERT_INFOR"/>
    <STRING id="STR_ID_HCCALL_MANUAL_INFOR"/>
    <STRING id="STR_ID_HCCALL_PAY_INFOR"/>        
    

    <STRING id="STR_ID_HCCALL_CC_IML"/>
    <STRING id="STR_ID_HCCALL_CC_IRP"/>
    <STRING id="STR_ID_HCCALL_CC_FJ"/>
    <STRING id="STR_ID_HCCALL_CC_DHF"/>
    <STRING id="STR_ID_HCCALL_CC_HMBD"/>
    <STRING id="STR_ID_HCCALL_CC_HJ"/>
    <STRING id="STR_ID_HCCALL_CC_JJC"/>
    <STRING id="STR_ID_HCCALL_CC_TQYB"/>
    <STRING id="STR_ID_HCCALL_CC_JTJ"/>
    <STRING id="STR_ID_HCCALL_CC_YZCX"/>
    <STRING id="STR_ID_HCCALL_CC_HSZZ"/>
    <STRING id="STR_ID_HCCALL_CC_DXS"/>
    <STRING id="STR_ID_HCCALL_CC_LTS"/>
    <STRING id="STR_ID_HCCALL_CC_LTF"/>
    <STRING id="STR_ID_HCCALL_CC_TTS"/>
    <STRING id="STR_ID_HCCALL_CC_WTS"/>
    <STRING id="STR_ID_HCCALL_CC_YDS"/>
    <STRING id="STR_ID_HCCALL_CC_YZF"/>         
    <STRING id="STR_ID_HCCALL_CC_XFTS"/> 
    <STRING id="STR_ID_HCCALL_CC_WHJ"/>   
    <STRING id="STR_ID_HCCALL_CC_JGJD"/>  
    <STRING id="STR_ID_HCCALL_CC_ZLJD"/>                                                
    <STRING id="STR_ID_HCCALL_CC_SWZY"/>
    <STRING id="STR_ID_HCCALL_CC_WSOS"/>
    <STRING id="STR_ID_HCCALL_CC_DXIP"/>
    <STRING id="STR_ID_HCCALL_CC_LTIP"/>         
    <STRING id="STR_ID_HCCALL_CC_YDIP"/>
    <STRING id="STR_ID_HCCALL_CC_SLHJ"/>
    <STRING id="STR_ID_HCCALL_CC_GDJZ"/>
    <STRING id="STR_ID_HCCALL_CC_HMBL"/>

    <STRING id="STR_ID_HCCALL_DATAUPDATING"/>
    <STRING id="STR_ID_HCCALL_PAYALERTINFO"/>
    <STRING id="STR_ID_HCCALL_PAYALERTSUCCESS"/>
    <STRING id="STR_ID_HCCALL_PAYALERTUPDATE"/>
    <STRING id="STR_ID_HCCALL_PAYALERTBAOYUE"/>         
    <STRING id="STR_ID_HCCALL_FC_BEIJING"/>
    <STRING id="STR_ID_HCCALL_FC_SHANGHAI"/>
    <STRING id="STR_ID_HCCALL_FC_TIANJIN"/>
    <STRING id="STR_ID_HCCALL_FC_CHONGQING"/>
    <STRING id="STR_ID_HCCALL_GUISHUDI"/>
    <STRING id="STR_ID_HCCALL_GETCODEFAIL"/>
    <STRING id="STR_ID_HCCALL_PROVINCE"/>
    <STRING id="STR_ID_HCCALL_MUNICIPALITES"/> 
    <STRING id="STR_ID_HCCALL_GUISHUDINOPAY"/>   
    <STRING id="STR_ID_HCCALL_FUN_SET_SIM"/>  
    <STRING id="STR_ID_HCCALL_FUN_SET_SIMF"/>       
    <STRING id="STR_ID_HCCALL_FUN_SET_SIMS"/>       
    <STRING id="STR_ID_HCCALL_MONALERTINFO"/>
    <STRING id="STR_ID_HCCALL_HALL"/> 
    <STRING id="STR_ID_HCCALL_INSERTSD"/>   
    <STRING id="STR_ID_HCCALL_SEARCH"/>  
    <STRING id="STR_ID_HCCALL_NOMEMORY"/>       
    <STRING id="STR_ID_HCCALL_TIP"/>  
    <STRING id="STR_ID_HCCALL_DATABASEUPDATE"/>
    <STRING id="STR_ID_HCCALL_NODATABASE"/>
    <STRING id="STR_ID_HCCALL_CHECKUPDATE"/>
    <STRING id="STR_ID_HCCALL_DOWNLOADING"/>
    <STRING id="STR_ID_HCCALL_NETERROR"/>
    <STRING id="STR_ID_HCCALL_ISNEW"/>
    <STRING id="STR_ID_HCCALL_SERVERERROR"/>
    <STRING id="STR_ID_HCCALL_TIMEOUT"/>
    <STRING id="STR_ID_HCCALL_INSERTSIM"/>
    <STRING id="STR_ID_HCCALL_UPDATEMANUAL"/>
    <STRING id="STR_ID_HCCALL_UPDATEAUTO"/>
    <STRING id="STR_ID_HCCALL_UPDATECLOSE"/>
    <STRING id="STR_ID_HCCALL_UPDATE1"/>
    <STRING id="STR_ID_HCCALL_UPDATE2"/>
    <STRING id="STR_ID_HCCALL_UPDATE3"/>
    <STRING id="STR_ID_HCCALL_NEWVERSION"/>
    
                                      
    <!----- Image Resource Area ----------------------------------------------->

    <IMAGE id="IMG_ID_HCCALL_MAIN">DSM_LIST_RES_ICONS_PATH"\\\\hccall.png"</IMAGE>    

    <!----- Screen Resource Area ---------------------------------------------->
    <SCREEN id="SCR_ID_HCCALL"/>
    <SCREEN id="SCR_ID_HCCALL_LOC_SEARCH"/>
    <SCREEN id="SCR_ID_HCCALL_SHOWRESULT_LOCSEARCH"/>
    <SCREEN id="SCR_ID_HCCALL_SECTION_NUM"/>
    <SCREEN id="SCR_ID_HCCALL_SECOND_SECTION_NUM"/>
    <SCREEN id="SCR_ID_HCCALL_ABOUT"/>
    <SCREEN id="SCR_ID_HCCALL_SETTING"/>
    <SCREEN id="SCR_ID_HCCALL_NETWORK_SETTING"/>
    <SCREEN id="SCR_ID_HCCALL_SIM_SETTING"/>
    <SCREEN id="SCR_ID_HCCALL_FUNC_SETTING"/>
    <SCREEN id="SCR_ID_HCCALL_SECOND_INCOMMON_NUM"/>
    <SCREEN id="SCR_ID_HCCALL_THIRD_SECTION_NUM"/>
    <SCREEN id="SCR_ID_HCCALL_HELP"/>
    <SCREEN id="SCR_ID_HCCALL_MANUAL"/>
    <SCREEN id="SCR_ID_HCCALL_PAY"/>
    <SCREEN id="SCR_ID_HCCALL_UPDATE_MENU"/>
    <SCREEN id="SCR_ID_HCCALL_UPDATE_SETTING"/>
    <SCREEN id="SCR_ID_HCCALL_UPDATE_NEWVERSION"/>
    <SCREEN id="SCR_ID_HCCALL_UPDATE_GPRSALERT"/>
    <SCREEN id="SCR_ID_HCCALL_UPDATE_CONNECTING"/>
    <SCREEN id="SCR_ID_HCCALL_UPDATE_DOWNLOADING"/>

	
    <!----- Menu Resource Area ------------------------------------------------>
    <MENU id="MENU_ID_HCCALL" str="STR_ID_HCCALL">
#ifdef __SUPPORT_PBCALL__
        <MENUITEM_ID>@OID:MENU_ID_HCCALL_PHB</MENUITEM_ID>
				<MENUITEM_ID>@OID:MENU_ID_HCCALL_HISTORYCALL</MENUITEM_ID>
#endif
				<MENUITEM_ID>@OID:MENU_ID_HCCALL_LOC_SEARCH</MENUITEM_ID>
				<MENUITEM_ID>@OID:MENU_ID_HCCALL_SECTION_NUM</MENUITEM_ID>
        <MENUITEM_ID>@OID:MENU_ID_HCCALL_INCOMMON_NUM</MENUITEM_ID>
				<MENUITEM_ID>@OID:MENU_ID_HCCALL_DATABASEUPDATE</MENUITEM_ID>
				<MENUITEM_ID>@OID:MENU_ID_HCCALL_SETTING</MENUITEM_ID>
				<MENUITEM_ID>@OID:MENU_ID_HCCALL_FUNNY</MENUITEM_ID>
				<MENUITEM_ID>@OID:MENU_ID_HCCALL_HELP</MENUITEM_ID>

    </MENU>

#ifdef __SUPPORT_PBCALL__
  <MENU id="MENU_ID_HCCALL_PHB" parent="MENU_ID_HCCALL" str="STR_ID_HCCALL_PHB">  </MENU>
  <MENU id="MENU_ID_HCCALL_HISTORYCALL" parent="MENU_ID_HCCALL" str="STR_ID_HCCALL_HISTORYCALL">  </MENU>
#endif
  <MENU id="MENU_ID_HCCALL_LOC_SEARCH" parent="MENU_ID_HCCALL" str="STR_ID_HCCALL_LOC_SEARCH">  </MENU>
	<MENU id="MENU_ID_HCCALL_SECTION_NUM" parent="MENU_ID_HCCALL" str="STR_ID_HCCALL_SECTION_NUM">  </MENU>
	<MENU id="MENU_ID_HCCALL_INCOMMON_NUM" parent="MENU_ID_HCCALL" str="STR_ID_HCCALL_INCOMMON_NUM">  </MENU>  
	<MENU id="MENU_ID_HCCALL_DATABASEUPDATE" parent="MENU_ID_HCCALL" str="STR_ID_HCCALL_DATABASEUPDATE">  </MENU>
	<MENU id="MENU_ID_HCCALL_SETTING" parent="MENU_ID_HCCALL" str="STR_ID_HCCALL_SETTING">  </MENU>
	<MENU id="MENU_ID_HCCALL_FUNNY" parent="MENU_ID_HCCALL" str="STR_ID_HCCALL_HALL">  </MENU>  
	<MENU id="MENU_ID_HCCALL_HELP" parent="MENU_ID_HCCALL" str="STR_ID_HCCALL_HELP">  </MENU> 
#endif /*__HC_CALL_NEW__*/
</APP>