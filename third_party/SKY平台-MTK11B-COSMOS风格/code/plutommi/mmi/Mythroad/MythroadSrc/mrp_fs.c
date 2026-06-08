#ifdef __MMI_DSM_NEW__

#include "MMI_Include.h"

#include "mrp_include.h"

#if(MTK_VERSION<= 0x0812)
#include "FileManagerGprot.h"
#endif

static char dsmWorkPath[DSM_MAX_FILE_LEN+1] =__MR_CFG_VAR_ROOT_PATH__;             /*路径都是gb 编码*/
static char dsmLaunchPath[DSM_MAX_FILE_LEN+1] = __MR_CFG_VAR_ROOT_PATH__;  /*路径都是gb 编码*/
static U8 dsmLaunchDrv = 'c';   /*每个应用默认的盘符都可能是不一样的*/
static U8 dsmWorkDrv = 'c';
static int32 ferrno;
/*filenamebuf 放的是GB格式的路径*/
char  filenamebuf[(DSM_MAX_FILE_LEN+1)*2] = {0};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL mr_fs_check_hide_mode(void)
{
#ifdef __MMI_DSM_HIDE_FOLDER__	
	return TRUE;
#else
	return FALSE;
#endif
}


#ifdef __MMI_DSM_HIDE_FOLDER__
BOOL mr_fs_check_is_vm_path(U16* filepath)
{
	S8 *p= NULL;
	U16 *tmp = OslMalloc(DSM_MAX_FILE_LEN*2);

	if(!tmp)
		return FALSE;
	
	memset(tmp,0,DSM_MAX_FILE_LEN*2);
	mmi_ucs2cat((S8*)tmp,(S8*)filepath);
	mmi_tolower((S8*)tmp);
	p = mmi_ucs2str((S8 *)tmp,(S8*)L"mythroad");
	OslMfree(tmp);
	
	if(p)
		return TRUE;
	else
		return FALSE;
}
#endif


char* mr_fs_get_filename(char * outputbuf,const char *filename) 
{
	memset(filenamebuf, 0, sizeof(filenamebuf));
	
	if(strlen((char *)mr_fs_get_work_path()) == 0)
		sprintf( filenamebuf, "%c:\\", (S8)mr_fs_get_work_drv());
	else
		sprintf( filenamebuf, "%c:\\%s\\", (S8)mr_fs_get_work_drv(),mr_fs_get_work_path());
	
	if(filename[0] !='\0')
	{
	     strcat(filenamebuf, filename);
	}

	kal_prompt_trace(MOD_MMI,"filenamebuf =%s",filenamebuf);
	
       mr_fs_separator_vm_to_local((U8 *)filenamebuf);	 
	mr_str_gb_to_ucs2( filenamebuf,outputbuf);
	return outputbuf;
}


void mr_fs_separator_local_to_vm(U8 * path)
{
      int len = 0 ,i = 0;

      len = strlen((char *)path);

	  for(i = 0;i<len;i++)
	  	if(path[i] == 0x5c)
			path[i] = 0x2f;
}


void mr_fs_separator_vm_to_local(U8 * path)
{
      int len = 0 ,i = 0;

      len = strlen((char *)path);

	  for(i = 0;i<len;i++)
	  	if(path[i] == 0x2f)
			path[i] = 0x5c;
}


void mr_fs_create_vm_dir(void)
{
       int ret;
	   
	S16 DSMWORK_PATH[DSM_MAX_FILE_LEN+1] ={0};
	S8  fileBuf[DSM_MAX_NAME_LEN+1] ={0};
	
	sprintf(fileBuf, "%c:\\%s", mr_fs_get_work_drv(), mr_fs_get_work_path());
	
	mr_fs_separator_vm_to_local((U8 *)fileBuf);
	mr_str_gb_to_ucs2((char*)fileBuf, (char *)DSMWORK_PATH);

	ret = FS_GetAttributes((WCHAR *)DSMWORK_PATH);

	if((ret < 0) ||(!(ret&FS_ATTR_DIR)))
	{
		FS_CreateDir((WCHAR *)DSMWORK_PATH);
#ifdef __MMI_DSM_HIDE_FOLDER__
		FS_SetAttributes((WCHAR *)DSMWORK_PATH, FS_GetAttributes((WCHAR *)DSMWORK_PATH)|FS_ATTR_HIDDEN);	
#endif
	}

}


void mr_fs_set_work_drv(U8 drv)
{
	dsmWorkDrv = drv;
}


U8 mr_fs_get_work_drv(void)
{
	return dsmWorkDrv;
}


void mr_fs_set_work_path(char *path)
{
	memcpy(dsmWorkPath,path,strlen(path)+1);
}


char *mr_fs_get_work_path(void)
{
	return dsmWorkPath;
}


void mr_fs_switch_to_root_dir(void)
{
	dsmWorkDrv = __MR_CFG_VAR_ROOT_DRV__;
	memcpy(dsmWorkPath,__MR_CFG_VAR_ROOT_PATH__,strlen(__MR_CFG_VAR_ROOT_PATH__)+1);
}


/****************************************************************************
函数名:static void mr_fs_switch_to_launch_dir(void)
描  述:将操作路径返回到刚启动时候的路径
参  数:无
返  回:无
****************************************************************************/
static void mr_fs_switch_to_launch_dir(void)
{
	dsmWorkDrv = dsmLaunchDrv;
	memcpy(dsmWorkPath,dsmLaunchPath,strlen(dsmLaunchPath)+1);
}

void mr_fs_create_root_dir(void)
{
	int ret;
	char temp[DSM_MAX_FILE_LEN * ENCODING_LENGTH] = {0};

	memset(filenamebuf,0,sizeof(filenamebuf));
	
	sprintf(filenamebuf,"%c:\\%s", __MR_CFG_VAR_ROOT_DRV__, __MR_CFG_VAR_ROOT_PATH__);
	mr_fs_separator_vm_to_local((U8 *)filenamebuf);
	mr_str_gb_to_ucs2(filenamebuf,temp);

	ret = FS_GetAttributes((WCHAR *)temp);
	
	if(ret < 0||(!(ret&FS_ATTR_DIR)))
	{	
		FS_CreateDir((WCHAR *)temp);
#ifdef __MMI_DSM_HIDE_FOLDER__
		FS_SetAttributes((WCHAR *)temp,FS_GetAttributes((WCHAR *)temp)|FS_ATTR_HIDDEN);
#endif		
	}
	
}


void mr_fs_set_launch_dir(U8 drv,char *path)
{
	dsmWorkDrv = dsmLaunchDrv = drv;
	memcpy(dsmWorkPath,path,strlen(path)+1);
	memcpy(dsmLaunchPath,path,strlen(path)+1);
}


int32 mr_fs_switch_path(uint8* input, int32 input_len, uint8** output, int32* output_len)
{
	U8 drv;
	int32 len = 0;

	if(input ==  NULL)
		return MR_FAILED;
	
	if(strlen((char *)input) > (DSM_MAX_FILE_LEN-3))
		return MR_FAILED;

	drv = input[0];

	switch(drv)
	{
		case 'Z':
		case 'z':
			mr_fs_switch_to_launch_dir();
			break;
		case 'Y':
		case 'y':
		{
			U8 tDrv = mr_fs_get_work_drv();
			U8 vDrv;
			
			if(tDrv == MMI_SYSTEM_DRV)
			{
				vDrv = 'A';
			}
			else if(tDrv == MMI_PUBLIC_DRV)
			{
				if(MMI_PUBLIC_DRV == MMI_CARD_DRV)
				{
					vDrv = 'C';
				}
				else
				{
					vDrv = 'B';
				}
			}
			else
			{
				vDrv = 'C';
			}
			
			memset(filenamebuf,0,sizeof(filenamebuf));
			
			if(strlen((char *)mr_fs_get_work_path()) == 0)
				sprintf(filenamebuf,"%c:/",vDrv);
			else	
				sprintf(filenamebuf,"%c:/%s/",vDrv,mr_fs_get_work_path());

			mr_fs_separator_local_to_vm((U8 *)filenamebuf);
			
			*output = (uint8 *)filenamebuf;
			*output_len = strlen(filenamebuf);
			break;	
		}
		case 'X':
		case 'x':
		{
			mr_fs_switch_to_root_dir();
			mr_fs_create_root_dir();
			break;
		}
		default:
		{
			U8 tDrv;
			char temp[DSM_MAX_FILE_LEN * ENCODING_LENGTH] = {0};
			int ret;

			if(drv == 'A'||drv == 'a')
			{
				tDrv = MMI_SYSTEM_DRV;
			}
			else if(drv == 'B'||drv == 'b')
			{
				tDrv = MMI_PUBLIC_DRV;
			}
			else
			{
				tDrv = MMI_CARD_DRV;
			}
			
			if (FS_GetDevStatus(tDrv, FS_MOUNT_STATE_ENUM) != FS_NO_ERROR)
			{
				mr_trace("device busy!");
				return MR_FAILED;
			}
			
			if(strlen((char *)input) <= 3)
			{
				mr_fs_set_work_drv(tDrv);
				mr_fs_set_work_path("");
				return MR_SUCCESS;
			}
			
			memcpy(filenamebuf,input,strlen((char *)input)+1);
			filenamebuf[0] = (char)tDrv;
			
			mr_fs_separator_vm_to_local((U8 *)filenamebuf);
			
			mr_str_gb_to_ucs2(filenamebuf, temp);

			ret = FS_GetAttributes((WCHAR *)temp);

			if((ret < 0) ||(!(ret&FS_ATTR_DIR)))
			{
				return MR_FAILED;
			}
			else
			{
				len = strlen(filenamebuf);

				if(filenamebuf[len-1] == '\\')
					filenamebuf[len-1] ='\0';
				
				mr_fs_set_work_drv(tDrv);
				mr_fs_set_work_path((char *)(filenamebuf+3));
			}
			break;
		}
	}
	
	return MR_SUCCESS;
}


/* 向后兼容 */
int32 dsmSwitchPath(uint8* input, int32 input_len, uint8** output, int32* output_len)
{
	return mr_fs_switch_path(input, input_len, output, output_len);
}


int32 mr_fs_check_msdc_status(int32 param)
{
#ifdef WIN32
	return MR_MSDC_OK;
#else
#ifdef __USB_IN_NORMAL_MODE__
#if (__MR_CFG_VAR_MTK_VERSION__ >= 0x10A1103)
	if(!srv_usb_is_in_mass_storage_mode())
#else	
	if(!mmi_usb_is_in_mass_storage_mode())
#endif	
	{
		if(srv_fmgr_drv_is_accessible(SRV_FMGR_CARD_DRV))
		{
			return MR_MSDC_OK;
		}
		else
		{
			return MR_MSDC_NOT_EXIST;
		}
	}
	else
	{
		return MR_MSDC_NOT_USEFULL;
	}
#else
	if(srv_fmgr_drv_is_accessible(SRV_FMGR_CARD_DRV))
	{
		return MR_MSDC_OK;
	}
	else
	{
		return MR_MSDC_NOT_EXIST;
	}
#endif
#endif
}


static T_DSM_DISK_INFO dsmDiskInfo;
int32 mr_fs_get_free_space(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	U64 disk_free_space, disk_total_space;
	FS_DiskInfo		disk_info;
	S32				fs_ret;
	U16 path[4];
	char aPath[4] ={0};
       U8 drv;
	 T_DSM_DISK_INFO *spaceInfo = &dsmDiskInfo;
	
	if(input == NULL)
		return MR_FAILED;

	switch(*input)
	{
		case 'A':
		case 'a':
			drv = MMI_SYSTEM_DRV;
			break;
		case 'B':
		case 'b':
			drv = MMI_PUBLIC_DRV;
			break;
		case 'C':
		case 'c':
			drv = MMI_CARD_DRV;
			break;
		default:
			return MR_FAILED;
	}

	sprintf(aPath,"%c:\\",drv);

	mmi_asc_n_to_ucs2((S8 *)path,aPath,3);
	
	fs_ret = FS_GetDiskInfo((PU16)path, &disk_info, FS_DI_BASIC_INFO|FS_DI_FREE_SPACE);

	disk_free_space	= 0;
	disk_total_space	= 0;
	
	if(fs_ret >= 0)
	{
		disk_free_space  = (U64)disk_info.FreeClusters*disk_info.SectorsPerCluster*disk_info.BytesPerSector;
		disk_total_space  = (U64)disk_info.TotalClusters*disk_info.SectorsPerCluster*disk_info.BytesPerSector;
		
		if((disk_total_space/10) >(U64)1024*1024*1024)
		{
			spaceInfo->tUnit = 1024*1024*1024;
		}
		else if((disk_total_space/10) > 1024*1024)
		{
			spaceInfo->tUnit = 1024*1024;
		}
		else if((disk_total_space/10) > 1024)
		{
			spaceInfo->tUnit = 1024;
		}
		else 
		{
			spaceInfo->tUnit = 1;
		}
		
		spaceInfo->total = disk_total_space/spaceInfo->tUnit;

		if((disk_free_space/10) >(U64)1024*1024*1024)
		{
			spaceInfo->unit = 1024*1024*1024;
		}
		else if((disk_free_space/10) > 1024*1024)
		{
			spaceInfo->unit = 1024*1024;
		}
		else if((disk_free_space/10) > 1024)
		{
			spaceInfo->unit = 1024;
		}
		else 
		{
			spaceInfo->unit = 1;
		}
		
		spaceInfo->account = disk_free_space/spaceInfo->unit;
		
		*output = (uint8 *)&dsmDiskInfo;
		*output_len = sizeof(dsmDiskInfo);
		return MR_SUCCESS;
		
	}
	else
	{
		return MR_FAILED;
	}
}


int32 mr_fs_get_position(int32 param)
{
	UINT pos = 0;
	int ret = FS_GetFilePosition(param,&pos);

	if(ret == FS_NO_ERROR)
		return (pos+MR_PLAT_VALUE_BASE);
	else
		return MR_FAILED;
}


static void mr_fs_datetime_local_to_vm(const FS_DOSDateTime* dt_lc, mr_datetime* dt_vm)
{
	dt_vm->year = dt_lc->Year1980 + 1980;
	dt_vm->month = dt_lc->Month;
	dt_vm->day = dt_lc->Day; 
	dt_vm->hour = dt_lc->Hour;
	dt_vm->minute = dt_lc->Minute;
	dt_vm->second = dt_lc->Second2;
	mr_trace("%d-%d-%d %d:%d:%d", dt_vm->year, dt_vm->month, dt_vm->day, dt_vm->hour, dt_vm->minute, dt_vm->second);
}


int32 mr_fs_get_create_time(uint8* input, int32 input_len, uint8** output, int32* output_len, MR_PLAT_EX_CB *cb)
{
	mr_datetime*  datetime = (mr_datetime*)output;
	FS_FileInfo  fileinfo;
	MR_FILE_HANDLE fd;
	int32 r = MR_FAILED;
	
	if(!input || !output || !output_len || *output_len != sizeof(mr_datetime))
	{
		return MR_FAILED;
	}

	fd = mr_open((char*)input, MR_FILE_RDONLY);
	if(fd > 0)
	{
		if(FS_NO_ERROR == FS_GetFileInfo((FS_HANDLE)fd, &fileinfo))
		{
			mr_fs_datetime_local_to_vm(&fileinfo.DirEntry->CreateDateTime, datetime);
			r = MR_SUCCESS;
		}

		mr_close(fd);
	}

	return r;
}


//////////////////////////////////////////////////////////////////////////////////////////////////

int32 mr_getLen(const char* filename)
{
	int filehandle = -1;
	UINT size;
	U16 fullpathname[DSM_MAX_FILE_LEN] = {0};

	filehandle = FS_Open((WCHAR *)mr_fs_get_filename((char *)fullpathname,filename), FS_READ_ONLY);

	if (filehandle <FS_NO_ERROR)
		return MR_FAILED;
	
	ferrno = FS_GetFileSize( filehandle, &size);
	
	if (ferrno == FS_NO_ERROR)
	{
		FS_Close(filehandle);
		return size;
	}
	else
	{
		FS_Close(filehandle);
		return MR_FAILED;
	}
}


MR_FILE_HANDLE mr_open(const char* filename,  uint32 mode)
{
	uint32 new_mode = 0;
	U16 fullpathname[DSM_MAX_FILE_LEN] = {0};
	

	if (mode & MR_FILE_RDONLY ) 
		new_mode |= FS_READ_ONLY;
	if (mode & MR_FILE_WRONLY ) 
		new_mode |= FS_READ_WRITE;
	if (mode & MR_FILE_RDWR ) 
		new_mode |= FS_READ_WRITE;
	if (mode & MR_FILE_CREATE ) 
		new_mode |= FS_CREATE;
	if(mode & MR_FILE_COMMITTED)
		new_mode |= FS_COMMITTED;
	if(mode & MR_FILE_SHARD)
		new_mode  |= FS_OPEN_SHARED;

	mr_fs_get_filename((char *)fullpathname,filename);
	
#ifdef __MMI_DSM_HIDE_FOLDER__
	if(mr_fs_check_is_vm_path(fullpathname))
		new_mode |= FS_ATTR_HIDDEN;
#endif
	
	ferrno = FS_Open((WCHAR *)fullpathname, new_mode);

	if (ferrno < 0) 
		return 0;
	else
		return ferrno;
}

#if 0

int32 mr_read(MR_FILE_HANDLE f,void *p,uint32 l)
{
	UINT readnum = 0;

	ferrno = FS_Read(f, p, l, &readnum);

	if  (ferrno == FS_NO_ERROR)
		return readnum;
	else 
		return MR_FAILED;
}

#else

#define READ_MAX_SECTOR (10*1024)

int32 mr_read(MR_FILE_HANDLE f,void *p,uint32 l)

{
UINT readnum = 0;

UINT left = l;

uint8 *pTmp = (uint8*)p;

UINT tryRead = READ_MAX_SECTOR;

while(left > 0)

{
     if(left < tryRead)

     tryRead = left;

     ferrno = FS_Read(f, (void *)pTmp, tryRead, &readnum);

     if(ferrno != FS_NO_ERROR)

     {
         break;
     }

     pTmp += readnum;

     left -= readnum;

     if (readnum != tryRead)
     {
         return l-left;
     }
}

if  (ferrno == FS_NO_ERROR)
     return l;
else 
     return MR_FAILED;
}

#endif

int32 mr_write(MR_FILE_HANDLE f,void *p,uint32 l)
{
	UINT writenum = 0;


	ferrno = FS_Write( f, p, l, &writenum);
	
	if  (ferrno == FS_NO_ERROR)
		return writenum;
	else 
		return MR_FAILED;
	
}


int32 mr_close(MR_FILE_HANDLE f)
{
	ferrno = FS_Close(f);

	if (ferrno == FS_NO_ERROR)
		return MR_SUCCESS;
	else
		return MR_FAILED;
}


int32 mr_info(const char* filename)
{
	U16 fullpathname[DSM_MAX_FILE_LEN] = {0};  


	ferrno = FS_GetAttributes((WCHAR *) mr_fs_get_filename((char *)fullpathname,filename));

	if (ferrno < 0) 
		return MR_IS_INVALID;
	
	if (ferrno & FS_ATTR_DIR) 
		return MR_IS_DIR;
	else
		return MR_IS_FILE;	 	 
}


int32 mr_seek(MR_FILE_HANDLE f, int32 pos, int method)
{

	ferrno = FS_Seek( f, (int) pos, method);

	if (ferrno < 0)
		return MR_FAILED;
	else 
		return MR_SUCCESS;
}


int32 mr_remove(const char* filename)
{
	U16 fullpathname[DSM_MAX_FILE_LEN] = {0};

	ferrno = FS_Delete(( WCHAR *) mr_fs_get_filename((char *)fullpathname,filename));  

	if (ferrno == FS_NO_ERROR) 
		return MR_SUCCESS;
	else
		return MR_FAILED;
}


int32 mr_rename(const char* oldname, const char* newname)
{
	U16 fullpathname_1[DSM_MAX_FILE_LEN] = {0};
	U16 fullpathname_2[DSM_MAX_FILE_LEN] = {0};


	ferrno = FS_Rename(( WCHAR *) mr_fs_get_filename((char *)fullpathname_1, oldname)
		,( WCHAR *)mr_fs_get_filename((char *)fullpathname_2, newname));

	if (ferrno == FS_NO_ERROR) 
		return MR_SUCCESS;
	else
		return MR_FAILED;
}


int32 mr_mkDir(const char* name)
{
	U16 fullpathname[DSM_MAX_FILE_LEN] = {0};

	ferrno = FS_CreateDir(( WCHAR *) mr_fs_get_filename((char *)fullpathname,name)) ;	 

	if (ferrno == FS_NO_ERROR) {
#ifdef __MMI_DSM_HIDE_FOLDER__
		if(mr_fs_check_is_vm_path(fullpathname))
			FS_SetAttributes((WCHAR*)fullpathname, FS_GetAttributes((WCHAR*)fullpathname) | FS_ATTR_HIDDEN);
#endif		
		return MR_SUCCESS;
	}else
		return MR_FAILED;
}


int32 mr_rmDir(const char* name)
{
	U16 fullpathname[DSM_MAX_FILE_LEN] = {0};

	ferrno = FS_RemoveDir(( WCHAR *) mr_fs_get_filename((char *)fullpathname,name)) ;

	if (ferrno == FS_NO_ERROR) 
		return MR_SUCCESS;
	else
		return MR_FAILED;
}


MR_FILE_HANDLE mr_findStart(const char* name, char* buffer, uint32 len)
{
	S8 NameBuffer[4] = {0};
	FS_DOSDirEntry file_info;
	U16 fullpathname[DSM_MAX_FILE_LEN] = {0};
	U16 dsmFindBuffer[DSM_MAX_FILE_LEN] ={0};
	
	memset(dsmFindBuffer,0,sizeof(dsmFindBuffer));
	memset(fullpathname,0,sizeof(fullpathname));
	memset(NameBuffer,0,sizeof(NameBuffer));
	
#if (MTK_VERSION < 0x0852)	
	FS_SetCurrentDir((const WCHAR*)mr_fs_get_filename((char *)fullpathname,name));

	mr_str_gb_to_ucs2("*",NameBuffer);
	 ferrno = FS_FindFirst((const WCHAR*)NameBuffer, 0, 0 ,&file_info, (WCHAR*)dsmFindBuffer, len);
#else
	mr_fs_get_filename((char *)fullpathname,name);
	mmi_ucs2cat((S8 *)fullpathname, (S8 *)L"\\*.*"); 	
       ferrno = FS_FindFirst((const WCHAR*)fullpathname, 0, 0 ,&file_info, (WCHAR*)dsmFindBuffer, len);
#endif


	if( ferrno < 0 )
		return MR_FAILED;
	   
	mr_str_ucs2_to_gb((char *)dsmFindBuffer,buffer,FALSE);
	return ferrno;
}


int32 mr_findGetNext(MR_FILE_HANDLE search_handle, char* buffer, uint32 len)
{
	FS_DOSDirEntry file_info;
	U16 dsmFindBuffer[DSM_MAX_FILE_LEN] ={0};
	
	memset(dsmFindBuffer,0,sizeof(dsmFindBuffer));
	
	ferrno = FS_FindNext( search_handle, &file_info, (WCHAR*)dsmFindBuffer, len );

	mr_str_ucs2_to_gb((char *)dsmFindBuffer,buffer,FALSE);

	if ( ferrno == FS_NO_ERROR )
		return MR_SUCCESS;
	else
		return MR_FAILED;
}


int32 mr_findStop(MR_SEARCH_HANDLE search_handle)
{
	ferrno = FS_FindClose(search_handle);

	if (ferrno == FS_NO_ERROR)
		return MR_SUCCESS;
	else
		return MR_FAILED;
}


int32 mr_ferrno(void)
{
	return ferrno;
}
#ifdef __MMI_DSM_NEW_JSKY__

/****************************************************************************
函数名:int32 mr_attributes(const char* filename)
描  述:得到一个文件信息
参  数:filename
返  回: 
无效:  MR_IS_INVALID
****************************************************************************/
int32 mr_attributes(const char* filename)
{
	U16 fullpathname[DSM_MAX_FILE_LEN] = {0};  


	ferrno = FS_GetAttributes((WCHAR *) mr_fs_get_filename((char *)fullpathname,filename));

	if (ferrno < 0) 
		return MR_FAILED;
	
	return ferrno; 	 
}

/****************************************************************************
函数名:int32 mr_attributes(const char* filename)
描  述:设置一个文件信息
参  数:filename ,attr属性
返  回: 
无效:  MR_IS_INVALID
****************************************************************************/
int32 mr_setattributes(const char* filename,uint8  attr)
{
	U16 fullpathname[DSM_MAX_FILE_LEN] = {0};  

	
	ferrno = FS_SetAttributes((WCHAR *) mr_fs_get_filename((char *)fullpathname,filename),attr);

	if (ferrno != 0) 
		return MR_FAILED;
	
	return MR_SUCCESS; 	 
}


/****************************************************************************
函数名:int32 mr_getfoldersize(const char* filename)
描  述:得到一个目录的总文件尺寸大小
参  数:dir
返  回: 
无效:  MR_IS_INVALID
****************************************************************************/
int32 mr_getfoldersize(const char* dir)
{
    int32 folderSize;
	
    U16 fullpathname[DSM_MAX_FILE_LEN] = {0}; 
	
    folderSize = FS_GetFolderSize((WCHAR *) mr_fs_get_filename((char *)fullpathname,dir),FS_COUNT_IN_BYTE, NULL, 0);

	if (folderSize<0)
	{
		return MR_FAILED;
	}else
		return folderSize;
}

#endif /*__MMI_DSM_NEW_JSKY__*/
#endif

