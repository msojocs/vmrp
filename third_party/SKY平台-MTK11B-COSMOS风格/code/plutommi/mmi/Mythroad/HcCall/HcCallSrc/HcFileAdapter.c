#if defined(__MMI_DSM_NEW__)  && defined( __HC_CALL_NEW__) 
/*-------------------------------------------------------------------------------*
杭州斯凯版权所有
File: HcCallIn_UI.c
Create: 2009.09.23
--------------------------------------------------------------------------------*/
#include "HcFileAdapter.h"
#include "fat_fs.h"
#include "GlobalMenuItems.h"
//#include "FrameworkStruct.h"
#include "GlobalDefs.h"
#include "FileMgrSrvGProt.h"
//#include "mmidsmmain.h"
#if defined(WIN32)
#define MMI_SYSTEM_DRV SRV_FMGR_SYSTEM_DRV
#define MMI_PUBLIC_DRV SRV_FMGR_PUBLIC_DRV
#define MMI_CARD_DRV SRV_FMGR_CARD_DRV
#else
#include "mrp_features.h"
#endif

static uint8 *g_bytestream_buf_ptr = 0;
static uint32 g_bytestream_size = 0;
static int32 g_bytestream_offset = 0;

int8 g_filename[HCPIM_MAX_FILE_LEN*2]={0};


int HC_get_driver_char(int type)
{
    switch(type)
    {
        case HC_DRIVER_SYSTEM:
            return MMI_SYSTEM_DRV; //FS_GetDrive(FS_DRIVE_I_SYSTEM, 1, FS_NO_ALT_DRIVE);
            break;
        case HC_DRIVER_PUBLIC:
            return MMI_PUBLIC_DRV; //FS_GetDrive(FS_DRIVE_V_NORMAL, 1, FS_NO_ALT_DRIVE);
            break;
        case HC_DRIVER_CARD:
            return MMI_CARD_DRV; //FS_GetDrive(FS_DRIVE_V_REMOVABLE, 1, FS_NO_ALT_DRIVE);
            break;
    }

	return MMI_SYSTEM_DRV;
}


/********************************************************************
Function:		HcFile_CreateHccallDir
Description: 	create the hccall's root dir
Input:		void
Output:		void
Return:		void
Notice:		this function is called when the phone is powered on
*********************************************************************/
int32 HcFile_CreateHccallDir(void)
{
	int ret;

	memset(g_filename, 0, sizeof(g_filename));
	kal_wsprintf((uint16 *)g_filename, "%c:\\%w", HC_get_driver_char(HC_DRIVER_CARD), L"newldzs");

	ret = FS_GetAttributes((uint16*)g_filename);
	if((ret < 0) ||(!(ret&FS_ATTR_DIR)))
	{
		FS_CreateDir((uint16*)g_filename);
	}
	
	return TRUE;
}


/********************************************************************
Function:		HcFile_Makeup_FileName
Description: 	makeup the hccall's file name, add the default 
Input:		void
Output:		void
Return:		void
Notice:		this function is called when the phone is powered on
*********************************************************************/
int32 HcFile_Makeup_FileName(int8 *destPathname, int8* srcFilename)
{
	int8 fileName[50] = {0};

	if(strcmp((const char*)HCUSER_CONFIG_F, (const char*)srcFilename)!=0)
	{
		kal_wsprintf((uint16 *)destPathname, "%c:\\%w", HC_get_driver_char(HC_DRIVER_CARD), L"newldzs\\");
	}
	else
	{
		char buf[100] = {0};
		sprintf(buf, "%c:\\", HC_get_driver_char(HC_DRIVER_PUBLIC));
		mmi_asc_to_ucs2((S8*)destPathname, (S8*)buf);
	}

	mmi_asc_to_ucs2((S8*)fileName, (S8*)srcFilename);
	mmi_ucs2cat((S8*)destPathname, (const S8*)fileName);
	
	return TRUE;
}


/****************************************************************************
函数名:int32 HcFile_Open(int32* FileHandle,int8* name,uint32 flag,uint16 mode)
描  述:打开文件
参  数:FileHandle 文件句柄 name 文件名 flag 打开方式 mode 保留
返  回:成功返回TRUE 否则返回FALSE
****************************************************************************/
int32 HcFile_Open(int32* FileHandle, int8* name, uint32 flag, uint16 mode)
{
	int32 result;

	if (FileHandle==NULL || name==NULL)
	{
		return FALSE;
	}
	
	if (mode == 1)
	{
		*FileHandle = (int32)name;
		g_bytestream_buf_ptr = (uint8*)name;
		g_bytestream_size = (uint32)flag;
		g_bytestream_offset = 0;
		return TRUE;
	}
	
	memset(g_filename, 0, sizeof(g_filename));
	HcFile_Makeup_FileName(g_filename, name);
	
	switch(flag)
	{
		case 0://PO_RDONLY
			result = FS_Open((uint16*)g_filename,FS_READ_ONLY | FS_OPEN_NO_DIR);
			break;
		case 1://PO_WRONLY
			result = FS_Open((uint16*)g_filename,FS_READ_WRITE | FS_CREATE_ALWAYS | FS_OPEN_NO_DIR);
			break;
		case 2://PO_RDWR
			result = FS_Open((uint16*)g_filename,FS_READ_WRITE | FS_CREATE  | FS_OPEN_NO_DIR);
			break;
		default:
			return FALSE;
	}				
	
	if(result >= 0)
	{
		*FileHandle = result;
		  
		return TRUE;
	}
	else
	{        
		return FALSE;
	}
}


/****************************************************************************
函数名:int32 HcFile_Close(int32 FileHandle, uint32 mode)
描  述:关闭文件
参  数:FileHandle 文件句柄 
返  回:成功返回TRUE 否则返回FALSE
****************************************************************************/
int32 HcFile_Close(int32 FileHandle, uint32 mode)
{
	if (FileHandle == 0)
	{
		return FALSE;
	}

	if (mode == 0)
	{
		FS_Close(FileHandle);
	}
	else
	{
		g_bytestream_size = 0;
		g_bytestream_offset = 0;
		g_bytestream_buf_ptr = NULL;
	}

	return TRUE;
}


/****************************************************************************
函数名:int32 int32 HcFile_GetSize(int8* name,uint32* FileSize)
描  述:获得文件大小
参  数:name 文件名  FileSize文件大小
返  回:成功返回TRUE 否则返回FALSE
****************************************************************************/
int32 HcFile_GetSize(int8* name,uint32* FileSize)
{
	int fHandle;
	UINT size;

	if(name == NULL || FileSize == NULL)
	{
		return FALSE;
	}
	
	memset(g_filename, 0, sizeof(g_filename));
	HcFile_Makeup_FileName(g_filename, name);

	fHandle = FS_Open((uint16*)g_filename,FS_READ_ONLY);
	if(fHandle < 0)
	{
		*FileSize = 0;
		return TRUE;
	}
	
	FS_GetFileSize(fHandle,&size);
	*FileSize = size;
	
	FS_Close(fHandle);
	return TRUE;
}


/****************************************************************************
函数名:int32 HcFile_Read(int32 FileHandle,void* buf,uint32 count,uint32* ReadCount, HC_UINT0)
描  述:读取文件
参  数:FileHandle 文件句柄  buf 存储区 count 存储区大小 readcount 读取计数
返  回:成功返回TRUE 否则返回FALSE
****************************************************************************/
int32 HcFile_Read(int32 FileHandle,void* buf,uint32 count,uint32* ReadCount, uint32 mode)
{
	int rc;
	UINT n;

	if (FileHandle==0 || buf==NULL || count<0)
	{
		return FALSE;
	}

	if (mode == 0)
	{
		rc = FS_Read(FileHandle,buf,count,&n);
		if(rc < 0)
		{
			return FALSE;	
		}

		if (ReadCount)
		{
			*ReadCount = n;
		}
	}
	else
	{
		if (g_bytestream_offset + count > g_bytestream_size)
		{
			return FALSE;
		}
		else
		{
			memcpy(buf, g_bytestream_buf_ptr + g_bytestream_offset, count);
			g_bytestream_offset += count;
			*ReadCount = count;
		}
	}
		
	return TRUE;
}


/****************************************************************************
函数名:int32 HcFile_Write(int32 FileHandle,void* buf,uint32 count,uint32* WriteCount)
描  述:写文件
参  数:FileHandle 文件句柄  buf 存储区 count 存储区大小 readcount 写计数
返  回:成功返回TRUE 否则返回FALSE
****************************************************************************/
int32 HcFile_Write(int32 FileHandle,void* buf,uint32 count,uint32* WriteCount)
{
	int rc;
	UINT  write;

	if (FileHandle==0 || buf==NULL || count<=0)
	{
		return FALSE;
	}
	
	rc = FS_Write(FileHandle,buf,count,&write);
	if(rc < 0)
	{
		return FALSE;
	}
	
	if(WriteCount != NULL)
	{
		*WriteCount = write;
	}
	
	return TRUE;
}


/****************************************************************************
函数名:int32 HcFile_Seek(int32 FileHandle,int32 offset,int16 origin, uint16 mode)
描  述:文件指针转移
参  数:FileHandle 文件句柄  offset 偏移量 origin 原点 SeekLen 长度
返  回:成功返回TRUE 否则返回FALSE
****************************************************************************/
int32 HcFile_Seek(int32 FileHandle, int32 offset,int16 origin, uint16 mode)
{
	int rc;

	if (FileHandle==0)
	{
		return FALSE;
	}

	if (mode == 0)
	{
		rc = FS_Seek(FileHandle, offset, origin);
		if(rc < 0)
		{
			return FALSE;
		}
	}
	else
	{
		uint32 offsetBk = g_bytestream_offset;
		
		switch(origin)
		{
		case PSEEK_SET:
			{
				g_bytestream_offset = offset;
			}
			break;
			
		case PSEEK_CUR:
			{
				g_bytestream_offset += offset;
			}
			break;
			
		case PSEEK_END:
			{
				g_bytestream_offset = g_bytestream_size-offset;
			}
			break;
		}

		if(g_bytestream_offset <0||g_bytestream_offset>= g_bytestream_size)
		{
			g_bytestream_offset= offsetBk;
			return FALSE;
		}
	}

	return TRUE;
}


/****************************************************************************
函数名:int32 HcFile_Delete(int8* name)
描  述:删除文件
参  数:name文件名
返  回:成功返回TRUE 否则返回FALSE
****************************************************************************/
int32 HcFile_Delete(int8* name)
{
	int rc;

	if (name==NULL)
	{
		return FALSE;
	}
	
	memset(g_filename, 0, sizeof(g_filename));
	HcFile_Makeup_FileName(g_filename, name);

	rc = FS_Delete((uint16*)g_filename);

	return TRUE;
}


int32 HcFile_Rename(int8* poldname, int8* pnewname)
{
	int8 *oldfilename = NULL;

	if (poldname==NULL || pnewname==NULL)
	{
		return FALSE;
	}

	oldfilename = (int8*)OslMalloc(HCPIM_MAX_FILE_LEN*2);
	if (!oldfilename)
	{
		return FALSE;
	}
	
	memset(oldfilename, 0, HCPIM_MAX_FILE_LEN*2);
	memset(g_filename, 0, sizeof(g_filename));
	HcFile_Makeup_FileName(oldfilename, poldname);
	HcFile_Makeup_FileName(g_filename, pnewname);
	
	if (FS_Rename((U16*)oldfilename,(U16*)g_filename) == FS_NO_ERROR)
	{
		OslMfree(oldfilename);
		return TRUE;
	}

	OslMfree(oldfilename);
	return FALSE;
}


/****************************************************************************
函数名:int32 HcFile_Delete(int8* name)
描  述:文件是否存在
参  数:name文件名
返  回:是返回TRUE 否则返回FALSE
****************************************************************************/
int32 HcFile_IsExist(int8* name)
{
	int fHandle;

	if (name==NULL)
	{
		return FALSE;
	}
	
	memset(g_filename, 0, sizeof(g_filename));
	HcFile_Makeup_FileName(g_filename, name);
	
	fHandle = FS_Open((uint16*)g_filename,FS_READ_ONLY);
	if(fHandle < 0)
	{
		return FALSE;
	}

	FS_Close(fHandle);
	return TRUE;
}



#ifdef DEBUG_FILE_TRACE
char g_hccall_tracebuffer[1024];

void FilePrintf(const char *pcFormat, ...)
{
	va_list MyList;
	int32 fhandle = 0;
	int32 result = 0;
	uint32 writecount = 0;

	memset(g_hccall_tracebuffer, 0, sizeof(g_hccall_tracebuffer));
	
	va_start(MyList, pcFormat);
	vsprintf((char*)g_hccall_tracebuffer, pcFormat, MyList);
	va_end(MyList);

	result = HcFile_Open(&fhandle, (int8*)HC_FILELOG ,PO_RDWR, 0);
	if (!result)
	{
		return;
	}

	HcFile_Seek(fhandle, 0, PSEEK_END, 0);
	HcFile_Write(fhandle, g_hccall_tracebuffer, strlen(g_hccall_tracebuffer), &writecount);
	HcFile_Write(fhandle, "\r\n", 2, &writecount);
	HcFile_Close(fhandle, 0);
}
#endif

#endif //__HC_CALL_NEW__
