#include "main.h"

Aoid aoid = AOID_INITIALIZER;
Aoid aoid_id = AOID_INITIALIZER;
Aoid aoid_device_kind = AOID_INITIALIZER;
Aoid aoid_rtc = AOID_INITIALIZER;
Aoid aoid_rtc_date = AOID_INITIALIZER;
Aoid aoid_rtc_time = AOID_INITIALIZER;
#ifdef USE_NOIDS
Aoid aoid_noid_first = AOID_INITIALIZER;
Aoid aoid_noid_extern_next_id = AOID_INITIALIZER;
#endif

extern int app_id;
extern dk_t app_device_kind;

static void app_serveAoidRequestSelf(void *vself, Aoid *oid, void *vserver, int command){
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_RESET:
			app_reset();
			acpls_reset(server);
			return;
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(R)
	}
	acpls_reset(server);
}

static void app_serveAoidRequestId(void *vself, Aoid *oid, void *vserver, int command){
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_GET_RAM_VALUE:
			aoidServer_sendII(oid, server, app_id, YES);
			return;
		case CMD_AOID_GET_NVRAM_VALUE:{
				yn_t success = NO;
				AppParam param; 
				param.id = 0;
				if(pmem_getAppParam(&param)){
					success = YES;
				}
				aoidServer_sendII(oid, server, param.id, success);
			}
			return;
		case CMD_AOID_SET_NVRAM_VALUE:{
				int v;
				if(acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){
					AppParam param;
					if(pmem_getAppParam(&param)){
						param.id = v;
						if(appParam_check(&param) == ERROR_NO){
							pmem_saveAppParam(&param);
						}
					}
				}
			}
			acpls_reset(server);
			return;
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(GGS)
	}
	
	acpls_reset(server);
}

static void app_serveAoidRequestDeviceKind(void *vself, Aoid *oid, void *vserver, int command){
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_GET_RAM_VALUE:
			aoidServer_sendII(oid, server, app_device_kind, YES);
			return;
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(Gr)
	}
	acpls_reset(server);
}

static void app_serveAoidRequestRtc(void *vself, Aoid *oid, void *vserver, int command){
	Acpls *server = (Acpls *) vserver;
	switch(command){
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(None)
	}
	acpls_reset(server);
}

static void app_serveAoidRequestRtcDate(void *vself, Aoid *oid, void *vserver, int command){
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_GET_NVRAM_VALUE:{
				unsigned long out = 0;
				yn_t success = NO;
				extern RTC rtc;
				DateTime *ndt = rtc.now();
				if(ndt != NULL) {
					out = dt_dtToIDate(ndt);
					success = YES;
				}
				aoidServer_sendII(oid, server, out, success);
			}
			return;
		case CMD_AOID_SET_NVRAM_VALUE:{
				unsigned long v;
				if(acp_packGetCellUl(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){
					rtc.setDate(v);
				}
			}
			acpls_reset(server);
			return;
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(GnS)
	}
	acpls_reset(server);
}

static void app_serveAoidRequestRtcTime(void *vself, Aoid *oid, void *vserver, int command){
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_GET_NVRAM_VALUE:{
				unsigned long out = 0;
				yn_t success = NO;
				extern RTC rtc;
				DateTime *ndt = rtc.now();
				if(ndt != NULL) {
					out = dt_dtToITime(ndt);
					success = YES;
				}
				aoidServer_sendII(oid, server, out, success);
			}
			return;
		case CMD_AOID_SET_NVRAM_VALUE:{
				unsigned long v;
				if(acp_packGetCellUl(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){
					rtc.setTime(v);
				}
			}
			acpls_reset(server);
			return;
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(GnS)
	}
	acpls_reset(server);
}

#ifdef USE_NOIDS
extern Noid noid;
extern Noid noid_extern;

#define AOID_NOID_GET_NVRAM_INT(ITEM, DEFVAL)	{yn_t success = NO;\
												AppParam param; \
												param.noids.ITEM = DEFVAL;\
												if(pmem_getAppParam(&param)){\
													success = YES;\
												}\
												aoidServer_sendII(oid, server, param.noids.ITEM, success);}
#define AOID_NOID_SET_NVRAM_INT(ITEM, TOTYPE)	{int v;\
										if(acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){\
											AppParam param;\
											if(pmem_getAppParam(&param)){\
												param.noids.ITEM = (TOTYPE) v;\
												if(appParam_check(&param) == ERROR_NO){\
													pmem_saveAppParam(&param);\
												}\
											}\
										}}\
										acpls_reset(server);
static void app_serveAoidRequestNoidFirst(void *vself, Aoid *oid, void *vserver, int command){
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_GET_RAM_VALUE:{
				int is_first = NO;
				if(noid.kind == NOID_KIND_FIRST){
					is_first = YES;
				}
				aoidServer_sendII(oid, server, is_first, YES);
			}
			return;
		case CMD_AOID_GET_NVRAM_VALUE:
			AOID_NOID_GET_NVRAM_INT(is_first, NO)
			return;
		case CMD_AOID_SET_NVRAM_VALUE:
			AOID_NOID_SET_NVRAM_INT(is_first, yn_t)
			return;
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(GGS)
	}
	acpls_reset(server);
}

static void app_serveAoidRequestNoidNext(void *vself, Aoid *oid, void *vserver, int command){
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_GET_RAM_VALUE:
			aoidServer_sendII(oid, server, *noid_extern.id, YES);
			return;
		case CMD_AOID_GET_NVRAM_VALUE:
			AOID_NOID_GET_NVRAM_INT(next_external_noid_id, NOID_ID_UNKNOWN)
			return;
		case CMD_AOID_SET_NVRAM_VALUE:
			AOID_NOID_SET_NVRAM_INT(next_external_noid_id, int)
			return;
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(GGS)
	}
	acpls_reset(server);
}
#endif

int aoids_begin(){
	size_t id = AOID_ID_UNKNOWN;
	//AOID_SET_PAR(OID, 						NEXT,							PARENT,			KIND,							DESCR,								FUNCTION, 							DATA,	ID)
	AOID_SET_PARAM(&aoid,						&aoid_id,						NULL,			AOID_KIND_COMPLEX,				AOID_DESCRIPTION_APP,				app_serveAoidRequestSelf,			NULL,	id)
	AOID_SET_PARAM(&aoid_id,					&aoid_device_kind,				&aoid,			AOID_KIND_ID_PARAM,				AOID_DESCRIPTION_ID,				app_serveAoidRequestId,				NULL,	id)
	AOID_SET_PARAM(&aoid_device_kind,			&aoid_rtc,						&aoid,			AOID_KIND_DEVICE_KIND_PARAM,	AOID_DESCRIPTION_DEVICE_KIND,		app_serveAoidRequestDeviceKind,		NULL,	id)
	AOID_SET_PARAM(&aoid_rtc,					&aoid_rtc_date,					&aoid,			AOID_KIND_COMPLEX,				AOID_DESCRIPTION_RTC,				app_serveAoidRequestRtc,			NULL,	id)
	AOID_SET_PARAM(&aoid_rtc_date,				&aoid_rtc_time,					&aoid_rtc,		AOID_KIND_DATED_PARAM,			AOID_DESCRIPTION_DATE,				app_serveAoidRequestRtcDate,		NULL,	id)
#ifdef USE_NOIDS
	AOID_SET_PARAM(&aoid_rtc_time,				&aoid_noid_first,				&aoid_rtc,		AOID_KIND_TODS_PARAM,			AOID_DESCRIPTION_TIME,				app_serveAoidRequestRtcTime,		NULL,	id)
	AOID_SET_PARAM(&aoid_noid_first,			&aoid_noid_extern_next_id,		&aoid,			AOID_KIND_YN_PARAM,				AOID_DESCRIPTION_NOID_FIRST,		app_serveAoidRequestNoidFirst,		NULL,	id)
	AOID_SET_PARAM(&aoid_noid_extern_next_id,	&serials.aoid,					&aoid,			AOID_KIND_ID_PARAM,				AOID_DESCRIPTION_NOID_NEXT,			app_serveAoidRequestNoidNext,		NULL,	id)
#else
	AOID_SET_PARAM(&aoid_rtc_time,				&serials.aoid,					&aoid_rtc,		AOID_KIND_TODS_PARAM,			AOID_DESCRIPTION_TIME,				app_serveAoidRequestRtcTime,		NULL,	id)
#endif
	serials_buildAoids(							&channels.aoid,					&aoid,																															&id);
	channels_buildAoids(						NULL,							&aoid,																															&id);
	return 1;
}
