#include "aoid.h"

static void channel_serveAoidRequestSelf(void *vself, Aoid *oid, void *vserver, int command){
	Channel *self = (Channel *) vself;
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_START:
			channel_start(self);
			acpls_reset(server);
			return;
		case CMD_AOID_STOP:
			channel_stop(self);
			acpls_reset(server);
			return;
		case CMD_AOID_RESET:
			channel_reset(self);
			acpls_reset(server);
			return;
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(SSR)
	}
	acpls_reset(server);
}

static void channel_serveRequestNone(void *vself, Aoid *oid, void *vserver, int command) {
	Acpls *server = (Acpls *) vserver;
	switch(command){
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(None)
	}
	acpls_reset(server);
}

static void channel_serveAoidRequestGGS(void *vself, Aoid *oid, void *vserver, int command, void (*sendRamParam)(Aoid *, Acpls *, Channel *), void (*sendNvramParam)(Aoid *, Acpls *, const ChannelParam *, yn_t), int (*setNvramParam)(Acpls *, ChannelParam *)){
	Channel *self = (Channel *) vself;
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_GET_RAM_VALUE:
			sendRamParam(oid, server, self);
			return;
		case CMD_AOID_GET_NVRAM_VALUE:{
				yn_t success = NO;
				ChannelParam param;
				memset(&param, 0, sizeof param);
				if(pmem_getChannelParam(&param, self->ind)){
					success = YES;
				}
				sendNvramParam(oid, server, &param, success);
			}
			return;
		case CMD_AOID_SET_NVRAM_VALUE:{
				ChannelParam param;
				if(!pmem_getChannelParam(&param, self->ind)){
					acpls_reset(server);
					return;
				}
				if(!setNvramParam(server, &param)){
					acpls_reset(server);
					return;
				}
				if(channelParam_check(&param) == ERROR_NO){
					pmem_saveChannelParam(&param, self->ind);
				}
			}
			acpls_reset(server);
			return;
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(GGS)
	}
	acpls_reset(server);
}

static void channel_sendRamParamId(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendII(oid, server, channel->id, YES);}
static void channel_sendNvramParamId(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendII(oid, server, param->id, success);}
static int channel_setNvramParamId(Acpls *server, ChannelParam *param){int v;	if(!acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){	return 0;	}	param->id = v;	return 1;}
static void channel_serveAoidRequestId(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamId, channel_sendNvramParamId, channel_setNvramParamId);}

static void channel_sendRamParamPin(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendII(oid, server, channel->pin, YES);}
static void channel_sendNvramParamPin(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendII(oid, server, param->pin, success);}
static int channel_setNvramParamPin(Acpls *server, ChannelParam *param){int v; if(!acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->pin = v; return 1;}
static void channel_serveAoidRequestPin(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamPin, channel_sendNvramParamPin, channel_setNvramParamPin);}

static void channel_sendRamParamInitialPinState(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendII(oid, server, channel->initial_pin_state, YES);}
static void channel_sendNvramParamInitialPinState(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendII(oid, server, param->initial_pin_state, success);}
static int channel_setNvramParamInitialPinState(Acpls *server, ChannelParam *param){int v;	if(!acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->initial_pin_state = v; return 1;}
static void channel_serveAoidRequestInitialPinState(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamInitialPinState, channel_sendNvramParamInitialPinState, channel_setNvramParamInitialPinState);}

static void channel_sendRamParamEnable(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendII(oid, server, channel->enable, YES);}
static void channel_sendNvramParamEnable(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendII(oid, server, param->enable, success);}
static int channel_setNvramParamEnable(Acpls *server, ChannelParam *param){int v; if(!acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->enable = (yn_t)v; return 1;}
static void channel_serveAoidRequestEnable(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamEnable, channel_sendNvramParamEnable, channel_setNvramParamEnable);}

static void channel_sendRamParamDTimerTimeOn(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendUlI(oid, server, channel->dtimer.time_on, YES);}
static void channel_sendNvramParamDTimerTimeOn(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendUlI(oid, server, param->dtimer.time_on_s, success);}
static int channel_setNvramParamDTimerTimeOn(Acpls *server, ChannelParam *param){unsigned long v; if(!acp_packGetCellUl(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->dtimer.time_on_s = v; return 1;}
static void channel_serveAoidRequestDTimerTimeOn(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamDTimerTimeOn, channel_sendNvramParamDTimerTimeOn, channel_setNvramParamDTimerTimeOn);}

static void channel_sendRamParamDTimerTimeOff(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendUlI(oid, server, channel->dtimer.time_off, YES);}
static void channel_sendNvramParamDTimerTimeOff(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendUlI(oid, server, param->dtimer.time_off_s, success);}
static int channel_setNvramParamDTimerTimeOff(Acpls *server, ChannelParam *param){unsigned long v; if(!acp_packGetCellUl(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->dtimer.time_off_s = v; return 1;}
static void channel_serveAoidRequestDTimerTimeOff(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamDTimerTimeOff, channel_sendNvramParamDTimerTimeOff, channel_setNvramParamDTimerTimeOff);}

static void channel_sendRamParamPTimerIntervalOn(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendUlI(oid, server, channel->ptimer.interval_on, YES);}
static void channel_sendNvramParamPTimerIntervalOn(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendUlI(oid, server, param->ptimer.interval_on_s, success);}
static int channel_setNvramParamPTimerIntervalOn(Acpls *server, ChannelParam *param){unsigned long v; if(!acp_packGetCellUl(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->ptimer.interval_on_s = v; return 1;}
static void channel_serveAoidRequestPTimerIntervalOn(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamPTimerIntervalOn, channel_sendNvramParamPTimerIntervalOn, channel_setNvramParamPTimerIntervalOn);}

static void channel_sendRamParamPTimerIntervalOff(Aoid *oid, Acpls *server, Channel *channel){aoidServer_sendUlI(oid, server, channel->ptimer.interval_off, YES);}
static void channel_sendNvramParamPTimerIntervalOff(Aoid *oid, Acpls *server, const ChannelParam *param, yn_t success){aoidServer_sendUlI(oid, server, param->ptimer.interval_off_s, success);}
static int channel_setNvramParamPTimerIntervalOff(Acpls *server, ChannelParam *param){unsigned long v; if(!acp_packGetCellUl(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->ptimer.interval_off_s = v; return 1;}
static void channel_serveAoidRequestPTimerIntervalOff(void *vself, Aoid *oid, void *vserver, int command){channel_serveAoidRequestGGS(vself, oid, vserver, command, channel_sendRamParamPTimerIntervalOff, channel_sendNvramParamPTimerIntervalOff, channel_setNvramParamPTimerIntervalOff);}

void dtimerAoid_build(DTimerAoid *self,  Aoid *next, Aoid *parent, void *vchannel, size_t *id){
	//OBJE_ID_SET_PARAM(OID, 					NEXT,						PARENT,			KIND,							DESCR,									FUNCTION, 									DATA,		ID)
	AOID_SET_PARAM(&self->main,					&self->time_on,				parent,			AOID_KIND_COMPLEX,				AOID_DESCRIPTION_DAY_TIMER,				channel_serveRequestNone,					vchannel,	*id)
	AOID_SET_PARAM(&self->time_on,				&self->time_off,			&self->main,	AOID_KIND_TODS_PARAM,			AOID_DESCRIPTION_ON_TIME,				channel_serveAoidRequestDTimerTimeOn,		vchannel,	*id)
	AOID_SET_PARAM(&self->time_off,				next,						&self->main,	AOID_KIND_TODS_PARAM,			AOID_DESCRIPTION_OFF_TIME,				channel_serveAoidRequestDTimerTimeOff,		vchannel,	*id)
}

void ptimerAoid_build(PTimerAoid *self,  Aoid *next, Aoid *parent, void *vchannel, size_t *id){
	//OBJE_ID_SET_PARAM(OID, 					NEXT,						PARENT,			KIND,							DESCR,									FUNCTION, 									DATA,		ID)
	AOID_SET_PARAM(&self->main,					&self->interval_on,			parent,			AOID_KIND_COMPLEX,				AOID_DESCRIPTION_2STATE_CYCLIC_TIMER,	channel_serveRequestNone,					vchannel,	*id)
	AOID_SET_PARAM(&self->interval_on,			&self->interval_off,		&self->main,	AOID_KIND_TIMES_PARAM,			AOID_DESCRIPTION_INTERVAL1_S,			channel_serveAoidRequestPTimerIntervalOn,	vchannel,	*id)
	AOID_SET_PARAM(&self->interval_off,			next,						&self->main,	AOID_KIND_TIMES_PARAM,			AOID_DESCRIPTION_INTERVAL2_S,			channel_serveAoidRequestPTimerIntervalOff,	vchannel,	*id)
}

void channelAoid_build(ChannelAoid *self, Aoid *next, Aoid *parent, void *vchannel, size_t *id){
	//OBJE_ID_SET_PARAM(OID, 					NEXT,						PARENT,			KIND,							DESCR,									FUNCTION, 									DATA,		ID)
	AOID_SET_PARAM(&self->main,					&self->id,					parent,			AOID_KIND_COMPLEX,				AOID_DESCRIPTION_CHANNEL,				channel_serveAoidRequestSelf,				vchannel,	*id)
	AOID_SET_PARAM(&self->id,					&self->pin,					&self->main,	AOID_KIND_ID_PARAM,				AOID_DESCRIPTION_ID,					channel_serveAoidRequestId,					vchannel,	*id)
	AOID_SET_PARAM(&self->pin,					&self->initial_pin_state,	&self->main,	AOID_KIND_UINT8_PARAM,			AOID_DESCRIPTION_PIN,					channel_serveAoidRequestPin,				vchannel,	*id)
	AOID_SET_PARAM(&self->initial_pin_state,	&self->enable,				&self->main,	AOID_KIND_PIN_STATE_PARAM,		AOID_DESCRIPTION_INITIAL_PIN_STATE,		channel_serveAoidRequestInitialPinState,	vchannel,	*id)
	AOID_SET_PARAM(&self->enable,				&self->dtimer.main,			&self->main,	AOID_KIND_YN_PARAM,				AOID_DESCRIPTION_ENABLE,				channel_serveAoidRequestEnable,				vchannel,	*id)
	dtimerAoid_build(&self->dtimer,				&self->ptimer.main, 		&self->main,																														vchannel,	id);
	ptimerAoid_build(&self->ptimer, 			next,						&self->main,																														vchannel,	id);
}
