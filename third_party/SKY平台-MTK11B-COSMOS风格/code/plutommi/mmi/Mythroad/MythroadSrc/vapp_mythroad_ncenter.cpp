#ifdef __MMI_DSM_NEW__

/***************************************************************************** 
 * Include
 *****************************************************************************/
#include "MMI_features.h"
#include "MMIDataType.h"
#ifdef __cplusplus
extern "C"
#endif
{
#include "mrp_include.h"
}

#if (__MR_CFG_VAR_MTK_VERSION__ < 0x11B1224)
#include "vapp_ncenter_base_cell.h"
#include "vapp_mythroad_ncenter.h"

void vapp_mythraod_goto_background(MMI_BOOL is_goto_background, S32 app_id, S32 img_id, S32 str_id)
{
    vapp_mythroad_goto_background_evt_struct evt;
    
    MMI_FRM_INIT_EVENT(&evt, EVT_ID_VAPP_MYTHROAD_GOTO_BACKGROUND);
    evt.is_goto_backgroud = is_goto_background;
    evt.app_id = app_id;
    evt.img_id = img_id;
    evt.str_id = str_id;
    MMI_FRM_CB_EMIT_EVENT(&evt);
}



VFX_IMPLEMENT_CLASS("VappMyThroadNcenterCell", VappMyThroadNcenterCell, VappNCenterOngoingDefaultCell);

VappMyThroadNcenterCell::VappMyThroadNcenterCell() :
    m_appId(0), m_imgId(0), m_strId(0)
{
}

void VappMyThroadNcenterCell::onInit()
{
    VappNCenterOngoingDefaultCell::onInit();
    
    mmi_frm_cb_reg_event(EVT_ID_VAPP_MYTHROAD_GOTO_BACKGROUND, &VappMyThroadNcenterCell::onMyThroadEvtCb, this);
}

void VappMyThroadNcenterCell::onDeinit()
{
    mmi_frm_cb_dereg_event(EVT_ID_VAPP_MYTHROAD_GOTO_BACKGROUND, &VappMyThroadNcenterCell::onMyThroadEvtCb, this);
    
    VappNCenterOngoingDefaultCell::onDeinit();
}

void VappMyThroadNcenterCell::createView()
{
    VappNCenterOngoingDefaultCell::createView();

    ShowContent();
}

void VappMyThroadNcenterCell::releaseView()
{
    VappNCenterOngoingDefaultCell::releaseView();
}

void VappMyThroadNcenterCell::onNCenterLeave()
{
    mr_backstage_activate_app(m_appId);
}

void VappMyThroadNcenterCell::onTap(VfxPoint pt)
{
    requestNCenterLeave();
}

void VappMyThroadNcenterCell::ShowOrHideApp(VfxBool is_goto_backgroud)
{
    if (is_goto_backgroud)
    {
        m_signalStateChanged.postEmit(this, VAPP_NCENTER_STATE_SHOW);
    }
    else
    {
        m_signalStateChanged.postEmit(this, VAPP_NCENTER_STATE_HIDE);
    }
}

void VappMyThroadNcenterCell::ShowContent()
{
    VfxS32 height = m_mainText->getSize().height;
    VfxS32 y = (VAPP_NCENTER_ONGOING_CELL_HEIGHT - height)/2;
    m_mainText->setPos(VAPP_NCENTER_ONGOING_CELL_MAIN_TEXT_X, y);

    setIcon(VfxImageSrc(m_imgId));
    setMainText(VFX_WSTR_RES(m_strId));
    /* We don't need close button */
    VFX_OBJ_CLOSE(m_closeButton);
}

mmi_ret VappMyThroadNcenterCell::onMyThroadEvtCb(mmi_event_struct * param)
{
    VappMyThroadNcenterCell *self = (VappMyThroadNcenterCell *)param->user_data;

    switch (param->evt_id)
    {
        case EVT_ID_VAPP_MYTHROAD_GOTO_BACKGROUND:
        {
            vapp_mythroad_goto_background_evt_struct *evt = (vapp_mythroad_goto_background_evt_struct *)param;
            self->m_appId = evt->app_id;
            self->m_imgId = evt->img_id;
            self->m_strId = evt->str_id;
            self->ShowOrHideApp((VfxBool)evt->is_goto_backgroud);
            break;
        }

        default:
            break;
    }

    return MMI_RET_OK;
}

#ifdef __PME_SUPPORT__
#ifdef __cplusplus
extern "C"
#endif
{
extern void pme_main_entry(void);
}

void vapp_mythraod_goto_pme_msg_email(MMI_BOOL is_show,S32 img_id, S32 str_id)
{
    vapp_mythroad_goto_background_evt_struct evt;
    
    MMI_FRM_INIT_EVENT(&evt, EVT_ID_VAPP_MYTHROAD_PME_MSG_EMAIL);
    evt.is_goto_backgroud = is_show;
    evt.app_id = 0;
    evt.img_id = img_id;
    evt.str_id = str_id;
    MMI_FRM_CB_EMIT_EVENT(&evt);
}


VFX_IMPLEMENT_CLASS("VappMyThroadPmeEmailNcenterCell", VappMyThroadPmeEmailNcenterCell, VappNCenterOngoingDefaultCell);

VappMyThroadPmeEmailNcenterCell::VappMyThroadPmeEmailNcenterCell() :
    m_appId(0), m_imgId(0), m_strId(0)
{
}

void VappMyThroadPmeEmailNcenterCell::onInit()
{
    VappNCenterOngoingDefaultCell::onInit();
    mmi_frm_cb_reg_event(EVT_ID_VAPP_MYTHROAD_PME_MSG_EMAIL, &VappMyThroadPmeEmailNcenterCell::onMyThroadEvtCb, this);
}

void VappMyThroadPmeEmailNcenterCell::onDeinit()
{
    mmi_frm_cb_dereg_event(EVT_ID_VAPP_MYTHROAD_PME_MSG_EMAIL, &VappMyThroadPmeEmailNcenterCell::onMyThroadEvtCb, this);
    VappNCenterOngoingDefaultCell::onDeinit();
}

void VappMyThroadPmeEmailNcenterCell::createView()
{
    VappNCenterOngoingDefaultCell::createView();

    ShowContent();
}

void VappMyThroadPmeEmailNcenterCell::releaseView()
{
    VappNCenterOngoingDefaultCell::releaseView();
}

void VappMyThroadPmeEmailNcenterCell::onNCenterLeave()
{
	pme_main_entry();
}

void VappMyThroadPmeEmailNcenterCell::onTap(VfxPoint pt)
{
    requestNCenterLeave();
}

void VappMyThroadPmeEmailNcenterCell::ShowOrHideApp(VfxBool is_goto_backgroud)
{
    if (is_goto_backgroud)
    {
        m_signalStateChanged.postEmit(this, VAPP_NCENTER_STATE_SHOW);
    }
    else
    {
        m_signalStateChanged.postEmit(this, VAPP_NCENTER_STATE_HIDE);
    }
}

void VappMyThroadPmeEmailNcenterCell::ShowContent()
{
    VfxS32 height = m_mainText->getSize().height;
    VfxS32 y = (VAPP_NCENTER_ONGOING_CELL_HEIGHT - height)/2;
    m_mainText->setPos(VAPP_NCENTER_ONGOING_CELL_MAIN_TEXT_X, y);

    setIcon(VfxImageSrc(m_imgId));
    setMainText(VFX_WSTR_RES(m_strId));
    /* We don't need close button */
    VFX_OBJ_CLOSE(m_closeButton);
}

mmi_ret VappMyThroadPmeEmailNcenterCell::onMyThroadEvtCb(mmi_event_struct * param)
{
    VappMyThroadPmeEmailNcenterCell *self = (VappMyThroadPmeEmailNcenterCell *)param->user_data;

    switch (param->evt_id)
    {
	case EVT_ID_VAPP_MYTHROAD_PME_MSG_EMAIL:
        {
            vapp_mythroad_goto_background_evt_struct *evt = (vapp_mythroad_goto_background_evt_struct *)param;
            self->m_appId = evt->app_id;
            self->m_imgId = evt->img_id;
            self->m_strId = evt->str_id;
            self->ShowOrHideApp((VfxBool)evt->is_goto_backgroud);
            break;
        }

        default:
            break;
    }

    return MMI_RET_OK;
}

void vapp_mythraod_goto_pme_msg_im(MMI_BOOL is_show,S32 img_id, S32 str_id)
{
    vapp_mythroad_goto_background_evt_struct evt;
    
    MMI_FRM_INIT_EVENT(&evt, EVT_ID_VAPP_MYTHROAD_PME_MSG_IM);
    evt.is_goto_backgroud = is_show;
    evt.app_id = 0;
    evt.img_id = img_id;
    evt.str_id = str_id;
    MMI_FRM_CB_EMIT_EVENT(&evt);
}


VFX_IMPLEMENT_CLASS("VappMyThroadPmeImNcenterCell", VappMyThroadPmeImNcenterCell, VappNCenterOngoingDefaultCell);

VappMyThroadPmeImNcenterCell::VappMyThroadPmeImNcenterCell() :
    m_appId(0), m_imgId(0), m_strId(0)
{
}

void VappMyThroadPmeImNcenterCell::onInit()
{
    VappNCenterOngoingDefaultCell::onInit();
    mmi_frm_cb_reg_event(EVT_ID_VAPP_MYTHROAD_PME_MSG_IM, &VappMyThroadPmeImNcenterCell::onMyThroadEvtCb, this);
}

void VappMyThroadPmeImNcenterCell::onDeinit()
{
    mmi_frm_cb_dereg_event(EVT_ID_VAPP_MYTHROAD_PME_MSG_IM, &VappMyThroadPmeImNcenterCell::onMyThroadEvtCb, this);
    VappNCenterOngoingDefaultCell::onDeinit();
}

void VappMyThroadPmeImNcenterCell::createView()
{
    VappNCenterOngoingDefaultCell::createView();

    ShowContent();
}

void VappMyThroadPmeImNcenterCell::releaseView()
{
    VappNCenterOngoingDefaultCell::releaseView();
}

void VappMyThroadPmeImNcenterCell::onNCenterLeave()
{
	pme_main_entry();
}

void VappMyThroadPmeImNcenterCell::onTap(VfxPoint pt)
{
    requestNCenterLeave();
}

void VappMyThroadPmeImNcenterCell::ShowOrHideApp(VfxBool is_goto_backgroud)
{
    if (is_goto_backgroud)
    {
        m_signalStateChanged.postEmit(this, VAPP_NCENTER_STATE_SHOW);
    }
    else
    {
        m_signalStateChanged.postEmit(this, VAPP_NCENTER_STATE_HIDE);
    }
}

void VappMyThroadPmeImNcenterCell::ShowContent()
{
    VfxS32 height = m_mainText->getSize().height;
    VfxS32 y = (VAPP_NCENTER_ONGOING_CELL_HEIGHT - height)/2;
    m_mainText->setPos(VAPP_NCENTER_ONGOING_CELL_MAIN_TEXT_X, y);

    setIcon(VfxImageSrc(m_imgId));
    setMainText(VFX_WSTR_RES(m_strId));
    /* We don't need close button */
    VFX_OBJ_CLOSE(m_closeButton);
}

mmi_ret VappMyThroadPmeImNcenterCell::onMyThroadEvtCb(mmi_event_struct * param)
{
    VappMyThroadPmeImNcenterCell *self = (VappMyThroadPmeImNcenterCell *)param->user_data;

    switch (param->evt_id)
    {
	case EVT_ID_VAPP_MYTHROAD_PME_MSG_IM:
        {
            vapp_mythroad_goto_background_evt_struct *evt = (vapp_mythroad_goto_background_evt_struct *)param;
            self->m_appId = evt->app_id;
            self->m_imgId = evt->img_id;
            self->m_strId = evt->str_id;
            self->ShowOrHideApp((VfxBool)evt->is_goto_backgroud);
            break;
        }

        default:
            break;
    }

    return MMI_RET_OK;
}

void vapp_mythraod_goto_pme_msg_system(MMI_BOOL is_show,S32 img_id, S32 str_id)
{
    vapp_mythroad_goto_background_evt_struct evt;
    
    MMI_FRM_INIT_EVENT(&evt, EVT_ID_VAPP_MYTHROAD_PME_MSG_SYSTEM);
    evt.is_goto_backgroud = is_show;
    evt.app_id = 0;
    evt.img_id = img_id;
    evt.str_id = str_id;
    MMI_FRM_CB_EMIT_EVENT(&evt);
}


VFX_IMPLEMENT_CLASS("VappMyThroadPmeSystemNcenterCell", VappMyThroadPmeSystemNcenterCell, VappNCenterOngoingDefaultCell);

VappMyThroadPmeSystemNcenterCell::VappMyThroadPmeSystemNcenterCell() :
    m_appId(0), m_imgId(0), m_strId(0)
{
}

void VappMyThroadPmeSystemNcenterCell::onInit()
{
    VappNCenterOngoingDefaultCell::onInit();
    mmi_frm_cb_reg_event(EVT_ID_VAPP_MYTHROAD_PME_MSG_SYSTEM, &VappMyThroadPmeSystemNcenterCell::onMyThroadEvtCb, this);
}

void VappMyThroadPmeSystemNcenterCell::onDeinit()
{
    mmi_frm_cb_dereg_event(EVT_ID_VAPP_MYTHROAD_PME_MSG_SYSTEM, &VappMyThroadPmeSystemNcenterCell::onMyThroadEvtCb, this);
    VappNCenterOngoingDefaultCell::onDeinit();
}

void VappMyThroadPmeSystemNcenterCell::createView()
{
    VappNCenterOngoingDefaultCell::createView();

    ShowContent();
}

void VappMyThroadPmeSystemNcenterCell::releaseView()
{
    VappNCenterOngoingDefaultCell::releaseView();
}

void VappMyThroadPmeSystemNcenterCell::onNCenterLeave()
{
	pme_main_entry();
}

void VappMyThroadPmeSystemNcenterCell::onTap(VfxPoint pt)
{
    requestNCenterLeave();
}

void VappMyThroadPmeSystemNcenterCell::ShowOrHideApp(VfxBool is_goto_backgroud)
{
    if (is_goto_backgroud)
    {
        m_signalStateChanged.postEmit(this, VAPP_NCENTER_STATE_SHOW);
    }
    else
    {
        m_signalStateChanged.postEmit(this, VAPP_NCENTER_STATE_HIDE);
    }
}

void VappMyThroadPmeSystemNcenterCell::ShowContent()
{
    VfxS32 height = m_mainText->getSize().height;
    VfxS32 y = (VAPP_NCENTER_ONGOING_CELL_HEIGHT - height)/2;
    m_mainText->setPos(VAPP_NCENTER_ONGOING_CELL_MAIN_TEXT_X, y);

    setIcon(VfxImageSrc(m_imgId));
    setMainText(VFX_WSTR_RES(m_strId));
    /* We don't need close button */
    VFX_OBJ_CLOSE(m_closeButton);
}

mmi_ret VappMyThroadPmeSystemNcenterCell::onMyThroadEvtCb(mmi_event_struct * param)
{
    VappMyThroadPmeSystemNcenterCell *self = (VappMyThroadPmeSystemNcenterCell *)param->user_data;

    switch (param->evt_id)
    {
	case EVT_ID_VAPP_MYTHROAD_PME_MSG_SYSTEM:
        {
            vapp_mythroad_goto_background_evt_struct *evt = (vapp_mythroad_goto_background_evt_struct *)param;
            self->m_appId = evt->app_id;
            self->m_imgId = evt->img_id;
            self->m_strId = evt->str_id;
            self->ShowOrHideApp((VfxBool)evt->is_goto_backgroud);
            break;
        }

        default:
            break;
    }

    return MMI_RET_OK;
}
#endif

#else

#include "vsrv_ncenter.h"

static void vapp_mythraod_Intent_callback(VsrvNotification* noti, VsrvNIntent intent, void* userData, VfxU32 userDataSize)
{
	if (intent.type == VSRV_NINTENT_TYPE_TERMINATE)
	{
		mr_app_exit();
	}	
	else if (intent.type == VSRV_NINTENT_TYPE_LAUNCH)
	{
		mr_backstage_activate_app(*(int32*)userData);
		VSRV_NOTIFICATION_CLOSE_EX((VsrvNGroupId)VAPP_MYTHROAD, 0);
	}
}

void vapp_mythraod_goto_background(MMI_BOOL is_goto_background, S32 app_id, S32 img_id, S32 str_id)
{
	if (!is_goto_background)
	{
		VSRV_NOTIFICATION_CLOSE_EX((VsrvNGroupId)VAPP_MYTHROAD, 0);
		return;
	}

	VsrvNGroupSingleTitle* group;
	VSRV_NGROUP_CREATE_EX(group, VsrvNGroupSingleTitle, (VAPP_MYTHROAD));

	VsrvNotificationOngoing *notify;
	VSRV_NOTIFICATION_CREATE_EX(notify, VsrvNotificationOngoing, ((VsrvNGroup*)group, 0));

	notify->setIcon(VfxImageSrc((VfxResId)img_id));
	notify->setQuestionText(VFX_WSTR_RES(STR_DSM_EXITBACKAPP));

	notify->setMainText(VFX_WSTR_RES(str_id));

	notify->setIntentCallback(vapp_mythraod_Intent_callback, (void*)&app_id, 4);
	notify->notify();
}

#endif
#endif/*__MMI_DSM_NEW__*/
