
#ifdef __MMI_DSM_NEW__
#include "mrp_include.h"

gdi_handle dsm_getWallpaperHandle(void)
{
#if (MTK_VERSION >= 0x07a)
	return dm_get_wallpaper_layer();
#else
	return (dm_wallpaper_layer != GDI_ERROR_HANDLE) ? (dm_layers[dm_wallpaper_layer]) : (GDI_ERROR_HANDLE);
#endif
}
#endif


/*#end#*/
/*#auto end#*/