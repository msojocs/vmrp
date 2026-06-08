
#ifndef __VAPP_MYTHROAD_H__
#define __VAPP_MYTHROAD_H__


/***************************************************************************** 
 * Include
 *****************************************************************************/
#include "vfx_mc_include.h"
#include "vcp_include.h"


/***************************************************************************** 
 * Define
 *****************************************************************************/

/***************************************************************************** 
 * Typedef
 *****************************************************************************/

/***************************************************************************** 
 * Mythroad App 
 *****************************************************************************/
class VappMythroadApp : public VfxApp
{
    VFX_DECLARE_CLASS(VappMythroadApp);

// Override
public:
    virtual void onRun(void* args, VfxU32 argSize);
    virtual mmi_ret onProc(mmi_event_struct *evt);
    virtual void onGroupTopActive();

    static mmi_proc_func proc;
    static mmi_id cui_id;
    static mmi_ret cui_proc(mmi_event_struct *evt);
};

#endif

