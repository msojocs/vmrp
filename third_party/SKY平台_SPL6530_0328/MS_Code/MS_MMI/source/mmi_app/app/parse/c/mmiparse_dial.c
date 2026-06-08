/****************************************************************************
** File Name:      mmi_parse.c                                             *
** Author:         bruce.chi                                               *
** Date:           14/01/2007                                              *
** Copyright:      2004 Spreadtrum, Incorporated. All Rights Reserved.     *
** Description:    This file is used to describe the parse moudle.         *
****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                             *
** 01/2007        Bruce.chi        modify
** 
****************************************************************************/




/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/
#include "mmi_app_parse_trc.h"
#ifdef WIN32
#include "std_header.h"
#endif
#include <string.h>
#include "mmi_signal.h"
#include "mmi_signal_ext.h"
//#include "mmi_osbridge.h"
#include "mmiparse_export.h"
#include "mmk_type.h"
#include "mmk_app.h"
#include "mn.h"
#include "mmidisplay_data.h"
//#include "mmieng.h"
#include "mmieng_export.h"
#include "window_parse.h"
#include "mmiphone_export.h"
#include "mmiussd_export.h"
//#include "mmiset.h"
#include "mmiset_export.h"
#include "mmiidle_export.h"
#include "mmicc_export.h"
#include "mmi_textfun.h"
//#include "mmiebook.h"
#include "mmiebook_export.h"
#include "mmipub.h"
#ifdef VT_SUPPORT
#include "mmivt_export.h"
#endif
#include "mmiset_call_export.h"
#ifdef MRAPP_SUPPORT
#include "mmimrapp_export.h"
#endif

/**---------------------------------------------------------------------------*
 **                         Macro Declaration    							  *
 **---------------------------------------------------------------------------*/
#define MIN_PIN_STRING_LENGTH       20
#define MIN_PIN2_STRING_LENGTH      21

/*---------------------------------------------------------------------------*/
/*                          TYPE AND CONSTANT                                */
/*---------------------------------------------------------------------------*/
/**---------------------------------------------------------------------------*
 **                         Constant Variables                                *
 **---------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------*
 **                         STATIC DEFINITION                                *
 **--------------------------------------------------------------------------*/

#define ker_ScenParamS void

/*----------------------------------------------------------------------*/
/*                         VARIABLES                                    */
/*----------------------------------------------------------------------*/

/*****************************************************************************/
//  Description : This function is used to parse input SIM string
//                to change PIN/PIN2 information. 
//  Global resource dependence : none
//  Author: Jassmine
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN parsSimCtrlStringProc( 
                                    MN_DUAL_SYS_E dual_sys,
                                    ker_ScenParamS    *param,
                                    parsStrInfoS      *parsStrInfo 
                                    );

/*****************************************************************************/
// 	Description : if permit dsp play ring
//	Global resource dependence : 
//  Author: jassmine.meng
//	Note: 
/*****************************************************************************/
LOCAL BOOLEAN parsManufactureStringProc(
                                  ker_ScenParamS    *param,
                                  parsStrInfoS      *parsStrInfo 
                                  );
/*****************************************************************************/
//  Description :to parse redial string
//  Global resource dependence : none
//  Author:bin.ji
//  Note: 2004.11.11
/*****************************************************************************/
LOCAL BOOLEAN ParseRedialString(uint8* str_ptr, uint8 str_len);

/*****************************************************************************/
//  Description : This function is used to parse input SIM string
//                to change PIN/PIN2 information. 
//  Global resource dependence : none
//  Author: Jassmine
//  Note: 
/*****************************************************************************/
LOCAL BOOLEAN parsSimCtrlStringProc( 
                                    MN_DUAL_SYS_E dual_sys,
                                    ker_ScenParamS    *param,
                                    parsStrInfoS      *parsStrInfo 
                                    )
{
    MMI_PIN_INFO_T pin_info = {0};    
    MMI_STRING_T wait_text = {0};
    parsErrorCodeE pars_result = PARS_RESULT_SUCCESS;
    
    if(strncmp(parsStrInfo->inputStr,"**05*",5) == 0 )
    {
        pars_result = MMIAPIPARSE_ParsePukAndPinStr(parsStrInfo->inputStr,&pin_info);

        //enter **05*PUK*NEW_PIN*NEW_PIN# to change pin//
        if (PARS_RESULT_SUCCESS == pars_result)
        {
            MMIAPIPHONE_SetCurHandlingPinSimID(dual_sys);

            MNPHONE_OperatePinEx(dual_sys, MNSIM_PIN_UNBLOCK,MNSIM_PIN1,&pin_info.ext_pin_value,&pin_info.pin_value);
            //进入等待界面
            MMI_GetLabelTextByLang(TXT_COMMON_WAITING, &wait_text);
            MMIPUB_OpenWaitWin(1,&wait_text,PNULL,PNULL,IDLE_SPECIAL_WAIT_WIN_ID,IMAGE_NULL,
                ANIM_PUBWIN_WAIT,WIN_ONE_LEVEL,MMIPUB_SOFTKEY_NONE,PNULL);
        }
        else if (PARS_RESULT_PIN_CONTRADICT == pars_result)
        {
            #ifdef MMI_PDA_SUPPORT
            MMIPUB_OpenAlertWarningWin(TXT_INPUT_CONFLICT);
            #else
            MMIPUB_OpenAlertWarningWin(TXT_MSGBOX_PIN_CONFLICT);
            #endif
        }
        else
        {
            MMIPUB_OpenAlertWarningWin(TXT_INPUT_ERROR);
        }
        return TRUE;
    }
    else if(strncmp(parsStrInfo->inputStr,"**04*",5) == 0 )
    {
        if (MNSIM_IsPin1EnableEx(dual_sys))//PIN1码启动
        {
            pars_result = MMIAPIPARSE_ParsePukAndPinStr(parsStrInfo->inputStr,&pin_info);

            //enter **04*PIN*NEW_PIN*NEW_PIN# to change pin//
            if (PARS_RESULT_SUCCESS == pars_result)
            {
                MMIAPIPHONE_SetCurHandlingPinSimID(dual_sys);

                MNPHONE_OperatePinEx(dual_sys, MNSIM_PIN_CHANGE,MNSIM_PIN1,&pin_info.ext_pin_value,&pin_info.pin_value);
                //进入等待界面
                MMI_GetLabelTextByLang(TXT_COMMON_WAITING, &wait_text);
                
                MMIPUB_OpenWaitWin(1,&wait_text,PNULL,PNULL,IDLE_SPECIAL_WAIT_WIN_ID,IMAGE_NULL,
                    ANIM_PUBWIN_WAIT,WIN_ONE_LEVEL,MMIPUB_SOFTKEY_NONE,PNULL);
            }
            else if (PARS_RESULT_PIN_CONTRADICT == pars_result)
            {
                #ifdef MMI_PDA_SUPPORT
                MMIPUB_OpenAlertWarningWin(TXT_INPUT_CONFLICT);
                #else
                MMIPUB_OpenAlertWarningWin(TXT_MSGBOX_PIN_CONFLICT);
                #endif
            }
            else
            {
                MMIPUB_OpenAlertWarningWin(TXT_INPUT_ERROR);
            }
        }
        else
        {
            MMIPUB_OpenAlertWarningWin(TXT_ENABLE_PIN1);
        }

        return TRUE;
    }
    else if(strncmp(parsStrInfo->inputStr,"**052*",6) == 0 )
    {
        pars_result = MMIAPIPARSE_ParsePukAndPinStr(parsStrInfo->inputStr,&pin_info);

        //enter **052*PUK2*NEW_PIN2*NEW_PIN2# to change pin2//
        if (PARS_RESULT_SUCCESS == pars_result)
        {
            MMIAPIPHONE_SetCurHandlingPinSimID(dual_sys);

            MNPHONE_OperatePinEx(dual_sys,MNSIM_PIN_UNBLOCK,MNSIM_PIN2,&pin_info.ext_pin_value,&pin_info.pin_value);
            //进入等待界面
            MMI_GetLabelTextByLang(TXT_COMMON_WAITING, &wait_text);
            
            MMIPUB_OpenWaitWin(1,&wait_text,PNULL,PNULL,IDLE_SPECIAL_WAIT_WIN_ID,IMAGE_NULL,
                ANIM_PUBWIN_WAIT,WIN_ONE_LEVEL,MMIPUB_SOFTKEY_NONE,PNULL);
        }
        else if (PARS_RESULT_PIN_CONTRADICT == pars_result)
        {
            #ifdef MMI_PDA_SUPPORT
            MMIPUB_OpenAlertWarningWin(TXT_INPUT_CONFLICT);
            #else
            MMIPUB_OpenAlertWarningWin(TXT_MSGBOX_PIN2_CONFLICT);
            #endif
        }
        else
        {
            MMIPUB_OpenAlertWarningWin(TXT_INPUT_ERROR);
        }
        return TRUE;
    }
    else if(strncmp(parsStrInfo->inputStr,"**042*",6) == 0 )
    {
        pars_result = MMIAPIPARSE_ParsePukAndPinStr(parsStrInfo->inputStr,&pin_info);

        //enter **042*PIN2*NEW_PIN2*NEW_PIN2# to change pin2//
        if (PARS_RESULT_SUCCESS == pars_result)
        {
            MMIAPIPHONE_SetCurHandlingPinSimID(dual_sys);

            MNPHONE_OperatePinEx(dual_sys, MNSIM_PIN_CHANGE,MNSIM_PIN2,&pin_info.ext_pin_value,&pin_info.pin_value);
            //进入等待界面
            MMI_GetLabelTextByLang(TXT_COMMON_WAITING, &wait_text);
            
            MMIPUB_OpenWaitWin(1,&wait_text,PNULL,PNULL,IDLE_SPECIAL_WAIT_WIN_ID,IMAGE_NULL,
                ANIM_PUBWIN_WAIT,WIN_ONE_LEVEL,MMIPUB_SOFTKEY_NONE,PNULL);
        }
        else if (PARS_RESULT_PIN_CONTRADICT == pars_result)
        {
            #ifdef MMI_PDA_SUPPORT
            MMIPUB_OpenAlertWarningWin(TXT_INPUT_CONFLICT);
            #else
            MMIPUB_OpenAlertWarningWin(TXT_MSGBOX_PIN2_CONFLICT);
            #endif
        }
        else
        {
            MMIPUB_OpenAlertWarningWin(TXT_INPUT_ERROR);
        }
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************/
// 	Description : this function is used to parse user input string,
//  put user input string value to struct MMI_PUK_PIN_INFO_T 
//	Global resource dependence : 
//  Author: jassmine.meng
//	Note: when use puk change pin, extpin mean puk, pin mean new pin;
//        when use pin change pin, extpin mean old pin, pin mean new pin.
/*****************************************************************************/
parsErrorCodeE MMIAPIPARSE_ParsePukAndPinStr(
                              char              *str,
                              MMI_PIN_INFO_T    *pin_info
                              )
{
    parsErrorCodeE result = PARS_RESULT_FORMAT_ERROR;
    uint8       i = 0,j = 0;

    //judge input string length if is valid
    if ((str[4] == '*') && (strlen(str) < MIN_PIN_STRING_LENGTH))//pin
    {
        return (result);
    }
    else if ((str[4] != '*') && (strlen(str) < MIN_PIN2_STRING_LENGTH))//pin2
    {
        return (result);
    }

    //decide puk or old pin start point
    if (str[4] == '*')//pin
    {
        i = 5;
    }
    else//pin2
    {
        i = 6;
    }

    //get puk or old pin length
	while ((str[i+j] != '*') && (j < 9))
	{
		j++;
	}

    //judge the length of puk or old pin if is valid
	if ((j < 4) || (j > 8))
	{
		return (result);
	}
	pin_info->ext_pin_value.blocklen = j;
    MMI_MEMCPY((char*)pin_info->ext_pin_value.blockbuf,MN_MAX_BLOCK_LEN,
               (char*)(str + i),pin_info->ext_pin_value.blocklen,
               pin_info->ext_pin_value.blocklen);
	
    //decide new pin start point
    i = i +j + 1;
    j = 0;

    //get new pin length
    while ((str[i+j] != '*') && (j < 9))
	{
		j++;
	}

    //judge the length of new pin if is valid
    if ((j < 4) || (j > 8))
	{
		return (result);
	}
    pin_info->pin_value.blocklen = j;
    MMI_MEMCPY((char*)pin_info->pin_value.blockbuf,MN_MAX_BLOCK_LEN,
               (char*)(str + i),pin_info->pin_value.blocklen,
               pin_info->pin_value.blocklen);

    //decide new pin confirm start point
    i = i +j + 1;
    j = 0;

    //get new pin confirm length
	while ((str[i+j] != '#') && (j < 9))
	{
		j++;
	}

    //judge the length of new pin confirm if is valid
	if ((j < 4) || (j > 8))
	{
		return (result);
	}

    //judge new pin and new pin confirm if is equal
    if (j == pin_info->pin_value.blocklen)//length
    {
        if (0 == strncmp((char*)pin_info->pin_value.blockbuf,(char*)(str + i),pin_info->pin_value.blocklen))
        {
            result = PARS_RESULT_SUCCESS;
            return (result);
        }
        else
        {
            result = PARS_RESULT_PIN_CONTRADICT;

            return (result);
        }
    }
    else
    {
        result = PARS_RESULT_PIN_CONTRADICT;

        return (result);
    }
}

/*****************************************************************************/
// 	Description : if permit dsp play ring
//	Global resource dependence : 
//  Author: jassmine.meng
//	Note: 
/*****************************************************************************/
LOCAL BOOLEAN parsManufactureStringProc(
                                  ker_ScenParamS    *param,
                                  parsStrInfoS      *parsStrInfo 
                                  )
{
    BOOLEAN     result = FALSE;

    //dsp
    if(strncmp(parsStrInfo->inputStr,"#*8378*377*0#",0x0d) == 0 )
    {
        MMIAPISET_SetDspPlayRing(FALSE);
        result = TRUE;
    }
    else if (strncmp(parsStrInfo->inputStr,"#*8378*377*1#",0x0d) == 0 )
    {
        MMIAPISET_SetDspPlayRing(TRUE);
        result = TRUE;
    }
    //minhint
    else if (strncmp(parsStrInfo->inputStr,"#*8378*6464468*0#",strlen("#*8378*6464468*0#")) == 0 )
    {
        MMIAPISET_SetPermitMinhintRingIncall(FALSE, CC_CALL_REMINDER_TIME_OFF);
        result = TRUE;
    }
    else if (strncmp(parsStrInfo->inputStr,"#*8378*6464468*1#",strlen("#*8378*6464468*1#")) == 0)
    {
        MMIAPISET_SetPermitMinhintRingIncall(TRUE, CC_CALL_REMINDER_TIME_50S);
        result = TRUE;
    }
    //callincall
    else if (strncmp(parsStrInfo->inputStr,"#*8378*2255462255*0#",strlen("#*8378*2255462255*0#")) == 0 )
    {
        MMIAPISET_SetPermitCallRingIncall(FALSE);
        result = TRUE;
    }
    else if (strncmp(parsStrInfo->inputStr,"#*8378*2255462255*1#",strlen("#*8378*2255462255*1#")) == 0 )
    {
        MMIAPISET_SetPermitCallRingIncall(TRUE);
        result = TRUE;
    }
    //msgincall
    else if (strncmp(parsStrInfo->inputStr,"#*8378*674462255*0#",strlen("#*8378*674462255*0#")) == 0 )
    {
        MMIAPISET_SetPermitMsgRingIncall(FALSE);
        result = TRUE;
    }
    else if (strncmp(parsStrInfo->inputStr,"#*8378*674462255*1#",strlen("#*8378*674462255*1#")) == 0 )
    {
        MMIAPISET_SetPermitMsgRingIncall(TRUE);
        result = TRUE;
    }

    return (result);
}


/*****************************************************************************/
// 	Description : init pars string
//	Global resource dependence : none
//  Author: bruce.chi
//	Note:
/*****************************************************************************/
void MMIAPIPARSE_ParsInitialise( parsStrInfoS* parsStrInfo, BOOLEAN strInitialised , const uint8 *str)
{
    
    //Initialize storage
    memset( parsStrInfo, 0, sizeof(parsStrInfoS));
    parsStrInfo->optCode = PARS_OPERATION_CODE_UNDETERMINED;
    parsStrInfo->ctrlCode= PARS_CTRL_CODE_UNDETERMINED;
    parsStrInfo->numOfMandatorySI = 0;

    //Get and store the input string
    if (strInitialised)
    {
       strncpy((char*)parsStrInfo->inputStr,(char*)str,PARS_MAX_STR_LEN);
       parsStrInfo->inputStr[PARS_MAX_STR_LEN-1] = '\0';
       parsStrInfo->strLen    = strlen((char*)parsStrInfo->inputStr);       
    }
    else
    {
       // pInputStr = ker_GetInputStr();
       //strncpy(parsStrInfo->inputStr,pInputStr,PARS_MAX_STR_LEN);
       parsStrInfo->inputStr[PARS_MAX_STR_LEN-1] = '\0';
       //parsStrInfo->strLen    = strlen(pInputStr);
    }
}

/*****************************************************************************/
// 	Description : parse dialing number string
//	Global resource dependence : none
//  Author: bruce.chi
//	Note:解析跟sim卡无关的操作
/*****************************************************************************/
BOOLEAN MMIAPIPARSE_ParseString(uint8 *str, uint8 str_len)
{
    BOOLEAN isStrParsered = FALSE;
    parsStrInfoS parsStrInfo = {0};
    void* param = PNULL;

    isStrParsered = FALSE;

    MMIAPIPARSE_ParsInitialise(&parsStrInfo, TRUE, str);

    if('*' == parsStrInfo.inputStr[0] || '#' == parsStrInfo.inputStr[0])
    {

        MMIENG_IDLE_DIAL_NUM_TYPE_E dial_num_type = MMIENG_IDLE_DIAL_NUM_MIN;
        parsCtrlCodeE ctrl_code = PARS_CHANGE_PIN1;

	    //解析是否是工程模式暗码
        isStrParsered = MMIAPIENG_ParseIdleDialNumString(
                                                                                        (uint8*)parsStrInfo.inputStr,
                                                                                        (uint8)parsStrInfo.strLen, 
                                                                                        &dial_num_type, 
                                                                                        &ctrl_code
                                                                                        );
        if (isStrParsered)
        {
            parsStrInfo.ctrlCode = ctrl_code;

            MMIAPIENG_DoIdleDialNumOpt(dial_num_type);
            MMIAPIIDLE_CloseDialWin();
            MMIAPIIDLE_CloseDialMenuWin();

            return TRUE;
        }

        //解析是否是切换多国语言暗码,start
        {
            MMISET_LANGUAGE_TYPE_E language_type = MMISET_LANGUAGE_ENGLISH;	    	
            isStrParsered = MMIAPIENG_ParseIdleSetLanguageTypeNumString(
                                                                    (uint8*)parsStrInfo.inputStr,
                                                                    (uint8)parsStrInfo.strLen, 
                                                                    &language_type
                                                                    );
            if (isStrParsered)
            {
                //MMIAPIIDLE_CloseDialWin();
                //MMIAPIIDLE_CloseDialMenuWin();
                MMK_ReturnIdleWin();
                MMIAPIENG_DoIdleSetLanguageTypeNumOpt(language_type);

                return TRUE;
            }
        }
        //解析是否是切换多国语言暗码,end

        isStrParsered = MMIAPISET_ParseCleanDataString((uint8*)parsStrInfo.inputStr, (uint8)parsStrInfo.strLen);
        if (isStrParsered)
        {
            MMIAPISET_OpenInputResetPwdWin(TRUE);
            MMIAPIIDLE_CloseDialWin();

            return TRUE;
        }

        isStrParsered = MMIAPIPARSE_ProcessImeiStr(
                                                                            (uint8*)parsStrInfo.inputStr, 
                                                                            parsStrInfo.strLen);

        if (isStrParsered)
        {
            MMIAPIIDLE_CloseDialWin();

            MMIAPIPHONE_OpenIMEIDisplayWin();

            return TRUE;
        }

#ifdef MRAPP_SUPPORT
	if (MMIMRAPP_ProcessEntrStr((uint8*)parsStrInfo.inputStr, parsStrInfo.strLen)) /*lint !e718 !e18*/
	{
		return TRUE;
	}
#endif

        //Input String is MS manufacture defined procedure
        //handle "*#8378*377*1#" permit dsp play ring in call,"*#8378*377*0#" not
        isStrParsered = parsManufactureStringProc( param, &parsStrInfo);

        if (isStrParsered)
        {
            MMIPUB_OpenAlertSuccessWin(TXT_SUCCESS);
            MMIAPIIDLE_CloseDialWin();

            return TRUE;
        }


        //"redial"  自动重拨
        //Input String is MS manufacture defined procedure
        //handle "*#8378*733425*1#" permit redial, "*#8378*733425*0#" not
        isStrParsered = ParseRedialString((uint8*)parsStrInfo.inputStr, (uint8)parsStrInfo.strLen);

        if (isStrParsered)
        {
            MMIPUB_OpenAlertSuccessWin(TXT_SUCCESS);
            MMIAPIIDLE_CloseDialWin();

            return TRUE;
        }
#ifdef MMI_SIM_LOCK_SUPPORT
        //解析是否是SIM Lock开关菜单暗码,end
        //handle "*####8888#" 
        isStrParsered = MMIAPISET_ParseSIMLockSwitchString((uint8*)parsStrInfo.inputStr, (uint8)parsStrInfo.strLen);
        if (isStrParsered)
        {
            MMIAPISET_OpenSIMLockSwitchWin();
            MMIAPIIDLE_CloseDialWin();

            return TRUE;
        }
#endif        
 
    }

    return FALSE;
}

/*****************************************************************************/
// 	Description : parse telephone number
//	Global resource dependence : none
//  Author: bruce.chi
//	Note: 解析跟sim相关的操作
/*****************************************************************************/
MMI_PARSE_TYPE_E MMIAPIPARSE_ParseTeleNum(MN_DUAL_SYS_E dual_sys, uint8 *str, uint8 str_len, BOOLEAN is_video_call )
{
    BOOLEAN isStrParsered = FALSE;
    parsStrInfoS parsStrInfo = {0};
    void* param = PNULL;
    char emergency_num[] = {"08"};
    BOOLEAN sim_ok = MMIAPIPHONE_IsSimAvailable(dual_sys);

    isStrParsered = FALSE;

    MMIAPIPARSE_ParsInitialise(&parsStrInfo, TRUE, str);

    // 通过parsInitialise函数计算出来的长度有问题
    parsStrInfo.strLen = str_len;
    if((parsStrInfo.strLen > 4) &&
        (parsStrInfo.inputStr[0]=='*') &&
        (parsStrInfo.inputStr[1]=='3') &&
        (parsStrInfo.inputStr[2]=='1') &&
        (parsStrInfo.inputStr[3]=='#') &&
        (parsStrInfo.inputStr[parsStrInfo.strLen - 1] != '#'))
    {
        return MMI_PARSE_CALL_CLIR_SUPPRESSION_E;
    }

    if((parsStrInfo.strLen > 4) &&
        (parsStrInfo.inputStr[0]=='#') &&
        (parsStrInfo.inputStr[1]=='3') &&
        (parsStrInfo.inputStr[2]=='1') &&
        (parsStrInfo.inputStr[3]=='#') &&
        (parsStrInfo.inputStr[parsStrInfo.strLen - 1] != '#'))
    {
        return MMI_PARSE_CALL_CLIR_INVOCATION_E;
    }
    
    if('*' == parsStrInfo.inputStr[0] || '#' == parsStrInfo.inputStr[0])
    {
         /* Input string is standard GSM SS procedure support by MS, NULL procedure, return FALSE*/
        isStrParsered = MMIAPISET_ParsSsStringProc(dual_sys, &parsStrInfo);

        if (isStrParsered)
        {
            return MMI_PARSE_SS_TYPE_E;
        }

        /* Input string is standard GSM SS procedure support by MS, NULL procedure, return FALSE*/
        /* Input string is SIM control procedure, PIN procedure, return FALSE if parse failure*/
        isStrParsered = parsSimCtrlStringProc(dual_sys, param, &parsStrInfo);

        if (isStrParsered)
        {
            return MMI_PARSE_SIM_CTRL_TYPE_E;
        }        

        //ussd
        isStrParsered = MMIAPIUSSD_ParseUssdString(dual_sys, (uint8 *)parsStrInfo.inputStr, (uint8)parsStrInfo.strLen);
        if (isStrParsered)
        {
            MMIAPIIDLE_CloseDialWin();

            return MMI_PARSE_USSD_TYPE_E;
        }
    }

    if((parsStrInfo.strLen > 1)
        && ('#' == parsStrInfo.inputStr[parsStrInfo.strLen - 1]))
    {
        //ussd
        isStrParsered = MMIAPIUSSD_ParseUssdString(dual_sys, (uint8 *)parsStrInfo.inputStr, (uint8)parsStrInfo.strLen);
        if (isStrParsered)
        {
            MMIAPIIDLE_CloseDialWin();

            return MMI_PARSE_USSD_TYPE_E;
        }
    }

    //when input 1/2 digit.should just if is USSD
    if (1 == parsStrInfo.strLen || 2 == parsStrInfo.strLen)
    {
        if((0 == MMIAPICOM_Stricmp((uint8 *)parsStrInfo.inputStr, (uint8 *)emergency_num)) || !sim_ok)
        {
            //SCI_TRACE_LOW:"MMIAPIPARSE_ParseTeleNum: input num is 08"
            SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPARSE_DIAL_651_112_2_18_2_42_56_0,(uint8*)"");
        }
        else
        {
            isStrParsered = MMIAPIUSSD_ParseUssdString(dual_sys, (uint8 *)parsStrInfo.inputStr, (uint8)parsStrInfo.strLen);
            if (isStrParsered)
            {
                MMIAPIIDLE_CloseDialWin();

                return MMI_PARSE_USSD_TYPE_E;
            }
        }
    }

    // 以下的处理是屏蔽#键为结尾的号码
    /* Input string is call set-up string */
    // 解析出CALL的相关
    if (is_video_call)
    {
#ifdef VT_SUPPORT
        MMIAPIVT_ReqVideoModeCall(dual_sys, str, str_len );

        return MMI_PARSE_VIDEO_TYPE_E;
#else
        //MMIAPPCC_ProcessPhoneNum( str, str_len );
        //SCI_TRACE_LOW:"MMIAPIPARSE_ParseTeleNum error: is_video_call=1 but VT_SUPPORT is not defined"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPARSE_DIAL_676_112_2_18_2_42_56_1,(uint8*)"");
#endif
    }

    return MMI_PARSE_NORMAL_CALL_TYPE_E;
}

/*****************************************************************************/
// 	Description : process the "*#06#" imei
//	Global resource dependence : 
//  Author: Great.Tian
//	Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMIAPIPARSE_ProcessImeiStr(
                                   uint8* str_ptr, //[IN] input string pointer
                                   uint16 str_len  //[IN] input string length
                                   )
{
    BOOLEAN      recode = FALSE;

    if(str_ptr == PNULL)
    {
        //SCI_TRACE_LOW:"MMIAPIPARSE_ProcessImeiStr str_ptr is NULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPARSE_DIAL_696_112_2_18_2_42_57_2,(uint8*)"");
        return FALSE;
    }
    if (str_len != 5)
    {
        //SCI_TRACE_LOW:"MMIAPIPARSE_ProcessImeiStr str_len is not 5"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPARSE_DIAL_701_112_2_18_2_42_57_3,(uint8*)"");
        return (FALSE);
    }   

    if (strncmp((char*)str_ptr,"*#06#",str_len) == 0)
    {
        recode = TRUE;
    }

    return (recode);
}
/*****************************************************************************/
//  Description :to parse redial string
//  Global resource dependence : none
//  Author:bin.ji
//  Note: 2004.11.11
/*****************************************************************************/
LOCAL BOOLEAN ParseRedialString(uint8* str_ptr, uint8 str_len)
{
    uint len = strlen((char*)"*#8378*733425*1#");
    
    if(str_ptr == PNULL)
    {
        //SCI_TRACE_LOW:"ParseRedialString str_ptr is NULL"
        SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIPARSE_DIAL_722_112_2_18_2_42_57_4,(uint8*)"");
        return FALSE;
    }
    
    if (str_len != len)
    {
        return  FALSE;
    }
    else if (0 == strncmp((char*)str_ptr, "*#8378*733425*1#", len))
    {
        MMIAPISET_SetIsAutoRedial(TRUE);
        return TRUE;
    }
    else if (0 == strncmp((char*)str_ptr, "*#8378*733425*0#", len))
    {
        MMIAPISET_SetIsAutoRedial(FALSE);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
