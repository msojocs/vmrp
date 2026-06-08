/***************************************************************************** 
 * Include
 *****************************************************************************/
#ifndef __VAPP_MYTHROAD_NCENTER_H__
#define __VAPP_MYTHROAD_NCENTER_H__


#include "MMIDataType.h"
#include "vapp_ncenter_base_cell.h"
#include "vapp_mythroad_gprot.h"


/*****************************************************************************
 * class VappMyThroadNcenterCell
 *****************************************************************************/
class VappMyThroadNcenterCell : public VappNCenterOngoingDefaultCell
{
    VFX_DECLARE_CLASS(VappMyThroadNcenterCell);

    
public:
    // Constructor / Destructor
    VappMyThroadNcenterCell();   

    VAPP_NCENTER_ONGOING_CELL_CREATE(VappMyThroadNcenterCell);
    
protected:
    virtual void onInit();
    virtual void onDeinit();

public:
    virtual void createView();
    virtual void releaseView();
    virtual void onNCenterLeave();
    virtual void onTap(VfxPoint pt);


private:
    void ShowOrHideApp(VfxBool is_goto_background);
    void ShowContent();
    static mmi_ret onMyThroadEvtCb(mmi_event_struct *param);

// implementation
private:
    VfxS32 m_appId;
    VfxS32 m_imgId;
    VfxS32 m_strId;
} ;

#ifdef __PME_SUPPORT__
class VappMyThroadPmeImNcenterCell : public VappNCenterOngoingDefaultCell
{
    VFX_DECLARE_CLASS(VappMyThroadPmeImNcenterCell);

    
public:
    // Constructor / Destructor
    VappMyThroadPmeImNcenterCell();   

    VAPP_NCENTER_ONGOING_CELL_CREATE(VappMyThroadPmeImNcenterCell);
    
protected:
    virtual void onInit();
    virtual void onDeinit();

public:
    virtual void createView();
    virtual void releaseView();
    virtual void onNCenterLeave();
    virtual void onTap(VfxPoint pt);


private:
    void ShowOrHideApp(VfxBool is_goto_background);
    void ShowContent();
    static mmi_ret onMyThroadEvtCb(mmi_event_struct *param);

// implementation
private:
    VfxS32 m_appId;
    VfxS32 m_imgId;
    VfxS32 m_strId;
} ;

class VappMyThroadPmeEmailNcenterCell : public VappNCenterOngoingDefaultCell
{
    VFX_DECLARE_CLASS(VappMyThroadPmeEmailNcenterCell);

    
public:
    // Constructor / Destructor
    VappMyThroadPmeEmailNcenterCell();   

    VAPP_NCENTER_ONGOING_CELL_CREATE(VappMyThroadPmeEmailNcenterCell);
    
protected:
    virtual void onInit();
    virtual void onDeinit();

public:
    virtual void createView();
    virtual void releaseView();
    virtual void onNCenterLeave();
    virtual void onTap(VfxPoint pt);


private:
    void ShowOrHideApp(VfxBool is_goto_background);
    void ShowContent();
    static mmi_ret onMyThroadEvtCb(mmi_event_struct *param);

// implementation
private:
    VfxS32 m_appId;
    VfxS32 m_imgId;
    VfxS32 m_strId;
} ;


class VappMyThroadPmeSystemNcenterCell : public VappNCenterOngoingDefaultCell
{
    VFX_DECLARE_CLASS(VappMyThroadPmeSystemNcenterCell);

    
public:
    // Constructor / Destructor
    VappMyThroadPmeSystemNcenterCell();   

    VAPP_NCENTER_ONGOING_CELL_CREATE(VappMyThroadPmeSystemNcenterCell);
    
protected:
    virtual void onInit();
    virtual void onDeinit();

public:
    virtual void createView();
    virtual void releaseView();
    virtual void onNCenterLeave();
    virtual void onTap(VfxPoint pt);


private:
    void ShowOrHideApp(VfxBool is_goto_background);
    void ShowContent();
    static mmi_ret onMyThroadEvtCb(mmi_event_struct *param);

// implementation
private:
    VfxS32 m_appId;
    VfxS32 m_imgId;
    VfxS32 m_strId;
} ;
#endif

#endif /* __VAPP_MYTHROAD_NCENTER_H__ */
