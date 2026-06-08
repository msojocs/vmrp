#ifdef __MMI_DSM_NEW__
extern unsigned char dsmLoopPCM;
extern long int dsmReadAddr;
extern long int dsmWriteAddr;
kal_int32 dsmAudGetBytes(MHdl *hdl, STFSAL *pstFSAL, kal_uint32 uLen, kal_uint8 *pbBuf);
static void dsmWavDecodeTask( void *data );
static void wavUpdateDataLen( wavMediaHdl *ihdl, kal_uint32 *uMBufLen);

kal_int32 dsmAudGetBytes(MHdl *hdl, STFSAL *pstFSAL, kal_uint32 uLen, kal_uint8 *pbBuf)
{
	kal_int32 actLen =0;
	kal_int32 leftLen = 0;
	kal_int32 temp =0;

	
	
	if(dsmReadAddr == dsmWriteAddr)
		return 0;

	if(dsmWriteAddr > dsmReadAddr)
	{
		leftLen= dsmWriteAddr-dsmReadAddr;
		actLen = leftLen >uLen?uLen:leftLen;
		memcpy(pbBuf,(void *)dsmReadAddr,actLen);
		dsmReadAddr+=actLen;
		return actLen;
	}
	else if(dsmWriteAddr < dsmReadAddr)
	{
		leftLen = (kal_uint32)pstFSAL->uRamFileSize -(dsmReadAddr-(kal_uint32)pstFSAL->pbFile);

		if(leftLen >= uLen)
		{
			memcpy(pbBuf,(void *)dsmReadAddr,uLen);
			dsmReadAddr+=uLen;
			return uLen;
		}
		else
		{
			memcpy(pbBuf,(void *)dsmReadAddr,leftLen);
			actLen = leftLen;
			dsmReadAddr = (kal_uint32)pstFSAL->pbFile;
			leftLen = uLen-actLen; /*目标buf 中还有那么大的空间可以继续放数据*/
			temp = dsmWriteAddr-dsmReadAddr;

			if(temp <= 0)
			{
				return actLen;
			}
			else
			{
				leftLen = temp>leftLen?leftLen:temp;
				memcpy(pbBuf+actLen,(void *)dsmReadAddr,leftLen);
				dsmReadAddr += leftLen;
				return (actLen+leftLen);
			}
		}
	}
	return 0;
}

static void dsmWavDecodeTask( void *data )
{
	MHdl *hdl = (MHdl *)data;
	wavMediaHdl *ihdl = (wavMediaHdl *) hdl;

	{
		kal_uint8  *pBuf;
		kal_uint32 uMBufLen;
		
		while(1)
		{
			hdl->GetWriteBuffer(hdl, &pBuf, &uMBufLen);
			
			if(uMBufLen == 0 || hdl->eof) 
				break;
			
			uMBufLen = dsmAudGetBytes(hdl,ihdl->pstFSAL,uMBufLen, pBuf);
			
			if(uMBufLen == 0 ) 
				break;
			
			wavUpdateDataLen( ihdl, &uMBufLen);
			hdl->WriteDataDone( hdl, uMBufLen );

		}
	}
	hdl->waiting = KAL_FALSE;
}

#endif

