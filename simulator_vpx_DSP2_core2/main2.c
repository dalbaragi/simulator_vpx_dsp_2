/* BIOS6 include */
#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/heaps/HeapBuf.h>
#include <ti/sysbios/heaps/HeapMem.h>
#include <ti/csl/csl_ipcAux.h>
#include <ti/csl/csl_chipAux.h>
#include <ti/csl/csl_cacheAux.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Memory.h>

#include <ti/ipc/GateMP.h>
#include <ti/ipc/Ipc.h>
#include <ti/ipc/ListMP.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/MultiProc.h>

#include <ti/ipc/Ipc.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/HeapBufMP.h>
#include <ti/ipc/MultiProc.h>

#include <math.h>

#include <string.h>

#include "../CustomHeaderDsp2.h"
#include "Calculate.h"

#include <stdlib.h>
#include <ti/ipc/Notify.h>


//Will be used to send message
//MessageQ_QueueId 		QueueIdCore2ToCore34567;
MessageQ_QueueId 		QueueIdCore0ToCore2;
MessageQ_QueueId 		QueueIdCore2ToCore0;
MessageQ_QueueId 		QueueIdCore2ToCore1;
MessageQ_QueueId 		QueueIdCore2ToCore3;
MessageQ_QueueId 		QueueIdCore2ToCore4;
MessageQ_QueueId 		QueueIdCore2ToCore5;
MessageQ_QueueId 		QueueIdCore2ToCore6;
MessageQ_QueueId 		QueueIdCore2ToCore7;

MessageQ_Handle  		MessageQCore0ToCore2;

MsgCore0ToCore2*		Msg0To2Ptr;
MsgCore2ToCore1*		Msg2To34567Ptr;

MsgCore2ToCore0*		Msg2To0Ptr;
MsgCore2ToCore1*		Msg2To1Ptr;
MsgCore2ToCore34567* 	Msg2To3Ptr;
MsgCore2ToCore34567* 	Msg2To4Ptr;
MsgCore2ToCore34567* 	Msg2To5Ptr;
MsgCore2ToCore34567* 	Msg2To6Ptr;
MsgCore2ToCore34567* 	Msg2To7Ptr;

HeapBufMP_Handle 		HeapHandleCore2Core0;
HeapBufMP_Handle 		HeapHandleCore2Core1;
HeapBufMP_Handle 		HeapHandleCore2Core3;
HeapBufMP_Handle 		HeapHandleCore2Core4;
HeapBufMP_Handle 		HeapHandleCore2Core5;
HeapBufMP_Handle 		HeapHandleCore2Core6;
HeapBufMP_Handle 		HeapHandleCore2Core7;


/* Initialize IPC and MessageQ */
void IPC_init(void)
{
	Int status;

	//Ipc_start does not Ipc_attach, because 'Ipc.procSync' is set to 'Ipc.ProcSync_PAIR' in *.cfg
	//Ipc reserves some shared memory in SharedRegion zero for synchronization.
	status = Ipc_start();
	if (status < 0)
	{
		System_abort("Ipc_start failed\n");
	}

	//Wait IPC attach
	//!!!! Must attach to the share memory owner - CORE1 firstly
	while ((status = Ipc_attach(PROC_ID_CORE1)) < 0)
	{
		Task_sleep(1) ;
	}
	while ((status = Ipc_attach(PROC_ID_CORE0)) < 0)
	{
		Task_sleep(1) ;
	}
	while ((status = Ipc_attach(PROC_ID_CORE3)) < 0)
	{
		Task_sleep(1) ;
		System_printf("Ipc_attach(PROC_ID_CORE3)\n");
	}
	while ((status = Ipc_attach(PROC_ID_CORE4)) < 0)
	{
		Task_sleep(1) ;
	}
	while ((status = Ipc_attach(PROC_ID_CORE5)) < 0)
	{
		Task_sleep(1) ;
	}
	while ((status = Ipc_attach(PROC_ID_CORE6)) < 0)
	{
		Task_sleep(1) ;
	}
	while ((status = Ipc_attach(PROC_ID_CORE7)) < 0)
	{
		Task_sleep(1) ;
	}
	System_printf("Ipc_attach() finished.\n");


	/* Create the heap that will be used to allocate messages. */
	//---------------For CORE0--------------------------
	/* Create the local message queue */
	MessageQCore0ToCore2 = MessageQ_create(MSGQ_NAME_CORE0_CORE2, NULL);
	if (MessageQCore0ToCore2 == NULL)
	{
		System_abort("MessageQ_create failed\n");
	}
	System_printf("MessageQ_create(MSGQ_NAME_CORE0_CORE2, NULL); \n");

	Msg0To2Ptr = (MsgCore0ToCore2*)malloc(sizeof(MsgCore0ToCore2));
	System_printf("MsgCore0ToCore2 malloc size is %d.\n", sizeof(MsgCore0ToCore2));
	if(!Msg0To2Ptr)//如果malloc失败
	{
		System_printf("MsgCore0ToCore2 malloc failed./n");
	}

	/*------------------------- To CORE0 ----------------------*/
	do {
		status = HeapBufMP_open(HEAP_BUF_NAME_CORE2_CORE0, &HeapHandleCore2Core0);
		Task_sleep(1) ;
	} while (status < 0);

	do {
		status = MessageQ_open(MSGQ_NAME_CORE2_CORE0, &QueueIdCore2ToCore0);
		Task_sleep(1) ;
	} while (status < 0);

	MessageQ_registerHeap((IHeap_Handle)HeapHandleCore2Core0, HEAP_ID_CORE2_CORE0);

	Msg2To0Ptr= (MsgCore2ToCore0*)MessageQ_alloc(HEAP_ID_CORE2_CORE0, sizeof(MsgCore2ToCore34567));
	if (Msg2To0Ptr == NULL)
	{
	   System_abort("MessageQ_alloc failed\n" );
	}
	System_printf("To CORE0 \n");

	/*------------------------- To CORE1 ----------------------*/
	do {
		status = HeapBufMP_open(HEAP_BUF_NAME_CORE2_CORE1, &HeapHandleCore2Core1);
		Task_sleep(1) ;
	} while (status < 0);

	do {
		status = MessageQ_open(MSGQ_NAME_CORE2_CORE1, &QueueIdCore2ToCore1);
		Task_sleep(1) ;
	} while (status < 0);

	MessageQ_registerHeap((IHeap_Handle)HeapHandleCore2Core1, HEAP_ID_CORE2_CORE1);

	Msg2To1Ptr= (MsgCore2ToCore1*)MessageQ_alloc(HEAP_ID_CORE2_CORE1, sizeof(MsgCore2ToCore34567));
	if (Msg2To1Ptr == NULL)
	{
	   System_abort("MessageQ_alloc failed\n" );
	}
	System_printf("To CORE1 \n");

	/*------------------------- To CORE3 ----------------------*/
	do {
		status = HeapBufMP_open(HEAP_BUF_NAME_CORE2_CORE34567, &HeapHandleCore2Core3);
		Task_sleep(1) ;
	} while (status < 0);

	do {
		status = MessageQ_open(MSGQ_NAME_CORE2_CORE3, &QueueIdCore2ToCore3);
		Task_sleep(1) ;
	} while (status < 0);

	MessageQ_registerHeap((IHeap_Handle)HeapHandleCore2Core3, HEAP_ID_CORE2_CORE34567);

	Msg2To3Ptr= (MsgCore2ToCore34567*)MessageQ_alloc(HEAP_ID_CORE2_CORE34567, sizeof(MsgCore2ToCore34567));
	if (Msg2To3Ptr == NULL)
	{
	   System_abort("MessageQ_alloc failed\n" );
	}
	System_printf("To CORE3 \n");

	/*------------------------- To CORE4 ----------------------*/
	do {
		status = HeapBufMP_open(HEAP_BUF_NAME_CORE2_CORE34567, &HeapHandleCore2Core4);
		Task_sleep(1) ;
	} while (status < 0);

	do {
		status = MessageQ_open(MSGQ_NAME_CORE2_CORE4, &QueueIdCore2ToCore4);
		Task_sleep(1) ;
	} while (status < 0);

	MessageQ_registerHeap((IHeap_Handle)HeapHandleCore2Core4, HEAP_ID_CORE2_CORE34567);

	Msg2To4Ptr= (MsgCore2ToCore34567*)MessageQ_alloc(HEAP_ID_CORE2_CORE34567, sizeof(MsgCore2ToCore34567));
	if (Msg2To4Ptr == NULL)
	{
	   System_abort("MessageQ_alloc failed\n" );
	}
	System_printf("To CORE4 \n");

	/*------------------------- To CORE5 ----------------------*/
	do {
		status = HeapBufMP_open(HEAP_BUF_NAME_CORE2_CORE34567, &HeapHandleCore2Core5);
		Task_sleep(1) ;
	} while (status < 0);

	do {
		status = MessageQ_open(MSGQ_NAME_CORE2_CORE5, &QueueIdCore2ToCore5);
		Task_sleep(1) ;
	} while (status < 0);

	MessageQ_registerHeap((IHeap_Handle)HeapHandleCore2Core5, HEAP_ID_CORE2_CORE34567);

	Msg2To5Ptr= (MsgCore2ToCore34567*)MessageQ_alloc(HEAP_ID_CORE2_CORE34567, sizeof(MsgCore2ToCore34567));
	if (Msg2To5Ptr == NULL)
	{
	   System_abort("MessageQ_alloc failed\n" );
	}
	System_printf("To CORE5 \n");

	/*------------------------- To CORE6 ----------------------*/
	do {
		status = HeapBufMP_open(HEAP_BUF_NAME_CORE2_CORE34567, &HeapHandleCore2Core6);
		Task_sleep(1) ;
	} while (status < 0);

	do {
		status = MessageQ_open(MSGQ_NAME_CORE2_CORE6, &QueueIdCore2ToCore6);
		Task_sleep(1) ;
	} while (status < 0);

	MessageQ_registerHeap((IHeap_Handle)HeapHandleCore2Core6, HEAP_ID_CORE2_CORE34567);

	Msg2To6Ptr= (MsgCore2ToCore34567*)MessageQ_alloc(HEAP_ID_CORE2_CORE34567, sizeof(MsgCore2ToCore34567));
	if (Msg2To6Ptr == NULL)
	{
	   System_abort("MessageQ_alloc failed\n" );
	}
	System_printf("To CORE6 \n");

	/*------------------------- To CORE7 ----------------------*/
	do {
		status = HeapBufMP_open(HEAP_BUF_NAME_CORE2_CORE34567, &HeapHandleCore2Core7);
		Task_sleep(1) ;
	} while (status < 0);

	do {
		status = MessageQ_open(MSGQ_NAME_CORE2_CORE7, &QueueIdCore2ToCore7);
		Task_sleep(1) ;
	} while (status < 0);

	MessageQ_registerHeap((IHeap_Handle)HeapHandleCore2Core7, HEAP_ID_CORE2_CORE34567);

	Msg2To7Ptr= (MsgCore2ToCore34567*)MessageQ_alloc(HEAP_ID_CORE2_CORE34567, sizeof(MsgCore2ToCore34567));
	if (Msg2To7Ptr == NULL)
	{
	   System_abort("MessageQ_alloc failed\n" );
	}
	System_printf("To CORE7 \n");

	System_printf("Debug(Core 2): IPC_init() finished. \n");
}

void MainThread()
{
	int 	status;
	int		ManualMsgID = 0;



	/* Initialize IPC and MessageQ */
	IPC_init();

	while(1)
	{
		//Waite message from core0.
		while(MessageQ_get(MessageQCore0ToCore2, (MessageQ_Msg *)&Msg0To2Ptr, MessageQ_FOREVER) != 0);

		CACHE_invL1d(Msg0To2Ptr, sizeof(MsgCore0ToCore2), CACHE_WAIT);	//从cache中invalid

		ManualMsgID = Msg0To2Ptr->header.msgId;

		MessageQ_setMsgId(&(Msg2To1Ptr->header), ManualMsgID);
		MessageQ_setMsgId(&(Msg2To3Ptr->header), ManualMsgID);
		MessageQ_setMsgId(&(Msg2To4Ptr->header), ManualMsgID);
		MessageQ_setMsgId(&(Msg2To5Ptr->header), ManualMsgID);
		MessageQ_setMsgId(&(Msg2To6Ptr->header), ManualMsgID);
		MessageQ_setMsgId(&(Msg2To7Ptr->header), ManualMsgID);

		//ISRJ参数0
		if(Msg0To2Ptr->JammingFrameId == ISRJ_0)
		{
			IsrjParam0Cal(Msg0To2Ptr, Msg2To1Ptr, Msg2To3Ptr);
			Msg2To1Ptr->SampleWidth 	= Msg0To2Ptr->JammingParam.JammingIsrjParam0Hyplink.SampleWidth;
			Msg2To1Ptr->RepeatWidth 	= Msg0To2Ptr->JammingParam.JammingIsrjParam0Hyplink.RepeatWidth;
			Msg2To1Ptr->RepeatStartTime = Msg0To2Ptr->JammingParam.JammingIsrjParam0Hyplink.RepeatStartTime;
			Msg2To1Ptr->RepeatNum 		= Msg0To2Ptr->JammingParam.JammingIsrjParam0Hyplink.RepeatNum;
			if(((Msg0To2Ptr->TargetFrameId >> 16) == MANUAL_MODE) ||
					((Msg0To2Ptr->TargetFrameId >> 16) == AUTO_MODE_PASS_BACK))
			{
				//回传数据
				status = MessageQ_put(QueueIdCore2ToCore0, &(Msg2To0Ptr->header));
				if (status < 0)
				{
					System_abort("MessageQ_put had a failure/error\n");
				}
			}
		}
		else if(Msg0To2Ptr->JammingFrameId == ISRJ_1)
		{
			IsrjParam1Cal(Msg0To2Ptr, Msg2To1Ptr, Msg2To3Ptr);
			Msg2To1Ptr->SampleWidth 	= Msg0To2Ptr->JammingParam.JammingIsrjParam1Hyplink.SampleWidth;
			Msg2To1Ptr->RepeatWidth 	= Msg0To2Ptr->JammingParam.JammingIsrjParam1Hyplink.RepeatWidth;
			Msg2To1Ptr->RepeatStartTime = Msg0To2Ptr->JammingParam.JammingIsrjParam1Hyplink.RepeatStartTime;
			Msg2To1Ptr->RepeatNum 		= Msg0To2Ptr->JammingParam.JammingIsrjParam1Hyplink.RepeatNum;
			if(((Msg0To2Ptr->TargetFrameId >> 16) == MANUAL_MODE) ||
					((Msg0To2Ptr->TargetFrameId >> 16) == AUTO_MODE_PASS_BACK))
			{
				//回传数据
				status = MessageQ_put(QueueIdCore2ToCore0, &(Msg2To0Ptr->header));
				if (status < 0)
				{
					System_abort("MessageQ_put had a failure/error\n");
				}
			}
		}

		//给core4~7的msg和3相同
		memcpy(Msg2To4Ptr, Msg2To3Ptr, sizeof(MsgCore2ToCore34567));
		memcpy(Msg2To5Ptr, Msg2To3Ptr, sizeof(MsgCore2ToCore34567));
		memcpy(Msg2To6Ptr, Msg2To3Ptr, sizeof(MsgCore2ToCore34567));
		memcpy(Msg2To7Ptr, Msg2To3Ptr, sizeof(MsgCore2ToCore34567));

		/* 计算好的角度发送给核3~7. */
		status = MessageQ_put(QueueIdCore2ToCore3, &(Msg2To3Ptr->header));
		if (status < 0)
		{
			System_abort("MessageQ_put had a failure/error\n");
		}
		status = MessageQ_put(QueueIdCore2ToCore4, &(Msg2To4Ptr->header));
		if (status < 0)
		{
			System_abort("MessageQ_put had a failure/error\n");
		}
		status = MessageQ_put(QueueIdCore2ToCore5, &(Msg2To5Ptr->header));
		if (status < 0)
		{
			System_abort("MessageQ_put had a failure/error\n");
		}
		status = MessageQ_put(QueueIdCore2ToCore6, &(Msg2To6Ptr->header));
		if (status < 0)
		{
			System_abort("MessageQ_put had a failure/error\n");
		}
		status = MessageQ_put(QueueIdCore2ToCore7, &(Msg2To7Ptr->header));
		if (status < 0)
		{
			System_abort("MessageQ_put had a failure/error\n");
		}

		/* 发送算好的参数到核1 */
		status = MessageQ_put(QueueIdCore2ToCore1, &(Msg2To1Ptr->header));
		if (status < 0)
		{
			System_abort("MessageQ_put had a failure/error\n");
		}

		System_printf("MessageQ_put\n");
	}
}

//---------------------------------------------------------------------
// Main Entry Point
//---------------------------------------------------------------------
int main()
{
	/* Start the BIOS 6 Scheduler */
	BIOS_start ();
}
