#if defined(__MMI_DSM_NEW__)  && defined( __HC_CALL_NEW__) 

#ifndef __HCCALL_FILE_ADAPTER_H_20090803__
#define __HCCALL_FILE_ADAPTER_H_20090803__

#include "HcDataType.h"


#define PO_RDONLY				0//Open for read only, FS_READ_ONLY
#define PO_WRONLY				1//Opens an empty file for writing. If the given file exists, its contents are destroyed.
#define PO_RDWR					2//Opens for writing and Reading; creates the file first if it doesn’t exist.    

#define PSEEK_SET				0// offset from begining of file
#define PSEEK_CUR				1// offset from current file pointer
#define PSEEK_END				2// offset from end of file

//macro defined for IMSI record file
#define RecordsPlitReturn			'\r'
#define RecordsPlitLine			'\n'

#define HCPIM_MAX_FILE_LEN		(128+1)


extern int32 HcFile_CreateHccallDir(void);
extern int32 HcFile_Open(int32* FileHandle,int8* name,uint32 flag,uint16 mode);
extern int32 HcFile_Close(int32 FileHandle, uint32 mode);
extern int32 HcFile_GetSize(int8* name,uint32* FileSize);
extern int32 HcFile_Read(int32 FileHandle,void* buf,uint32 count,uint32* ReadCount, uint32 mode);
extern int32 HcFile_Write(int32 FileHandle,void* buf,uint32 count,uint32* WriteCount);
extern int32 HcFile_Seek(int32 FileHandle,int32 offset,int16 origin, uint16 mode);
extern int32 HcFile_Delete(int8* name);
extern int32 HcFile_IsExist(int8* name);
extern int32 HcFile_Rename(int8* poldname, int8* pnewname);
#endif //__HCCALL_FILE_ADAPTER_H_20090803__
#endif // __HC_CALL_NEW__
