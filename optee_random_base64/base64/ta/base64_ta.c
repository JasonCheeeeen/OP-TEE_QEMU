#include<tee_internal_api.h>
#include<tee_internal_api.h>
#include<stdio.h>
#include<stdlib.h>
#include<base64_ta.h>
/*
 * Called when the instance of the TA is created.
 * First
 */
TEE_Result TA_CreateEntryPoint(void)
{
	DMSG("has been called");

	return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed.
 * last
 */
void TA_DestroyEntryPoint(void)
{
	DMSG("has been called");
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
		TEE_Param __maybe_unused params[4],
		void __maybe_unused **sess_ctx)
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	/* Unused parameters */
	(void)&params;
	(void)&sess_ctx;

	/*
	 * The DMSG() macro is non-standard, TEE Internal API doesn't
	 * specify any means to logging from a TA.
	 */
	IMSG("Hello World!\n");

	/* If return value != TEE_SUCCESS the session will not be created. */
	return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
	(void)&sess_ctx; /* Unused parameter */
	IMSG("Goodbye!\n");
}

static TEE_Result base64_en(uint32_t param_types,
	TEE_Param params[4])
{
	uint32_t exp_param_types =
				TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_OUTPUT,
						TEE_PARAM_TYPE_NONE,
						TEE_PARAM_TYPE_NONE,
						TEE_PARAM_TYPE_NONE);
	unsigned char 		*base64_table="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	unsigned char *res = NULL;
	void *buf = NULL;
	long str_len = 0;
	long res_len;
	int k = 0;
	int i,j;

	DMSG("has been called");
	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	buf = TEE_Malloc(params[0].memref.size, 0);
	TEE_MemMove(buf, params[0].memref.buffer, params[0].memref.size);
	unsigned char *str = (unsigned char*) buf;
	
	while(str[k] != 0){
		str_len++;
		k++;
	}
	if(str_len%3 == 0){
		res_len = (str_len/3)*4;
	}
	else{
		res_len = (str_len/3+1)*4;
	}
	res = malloc(sizeof(unsigned char)*res_len+1);
	res[res_len] = '\0';
	
	for(i=0,j=0;i<res_len-2;i+=4,j+=3){
		res[i] = base64_table[str[j]>>2];
		res[i+1] = base64_table[(str[j]&0x3)<<4 | (str[j+1]>>4)];
		res[i+2] = base64_table[(str[j+1]&0xf)<<2 | (str[j+2]>>6)];
		res[i+3] = base64_table[str[j+2]&0x3f];
	}
	
	switch(str_len%3){
		case 1:
			res[res_len-1] = '=';
			res[res_len-2] = '=';
			break;
		case 2:
			res[res_len-1] = '=';
			break;
	}
	
	void *tmp = res;
	TEE_MemMove(params[0].memref.buffer, tmp, res_len+1);
	IMSG("Test :%s %ld %ld %s", str, str_len, res_len, res);
	if (!buf)
		return TEE_ERROR_OUT_OF_MEMORY;
	free(res);
	TEE_Free(buf);

	return TEE_SUCCESS;
}

static TEE_Result base64_de(uint32_t param_types,
	TEE_Param params[4])
{
	uint32_t exp_param_types =
				TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_OUTPUT,
						TEE_PARAM_TYPE_NONE,
						TEE_PARAM_TYPE_NONE,
						TEE_PARAM_TYPE_NONE);
	unsigned char* res = NULL;
	unsigned char* buf = NULL;
	long str_len = 0;
	long res_len;
	int k = 0;
	int i,j;
	int table[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,62,0,0,0,
             	     63,52,53,54,55,56,57,58,59,60,61,0,0,0,0,0,0,0,0,
             	     1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
		     22,23,24,25,0,0,0,0,0,0,26,27,28,29,30,31,32,33,34,35,
             	     36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};

	DMSG("has been called");
	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	buf = TEE_Malloc(params[0].memref.size, 0);
	TEE_MemMove(buf, params[0].memref.buffer, params[0].memref.size);
	unsigned char *str = (unsigned char*) buf;
	
	while(str[k] != 0){
		str_len++;
		k++;
	}
	if(str[str_len-1] == '=' && str[str_len-2] == '='){
		res_len = str_len/4*3-2;
	}
	else if(str[str_len-1] == '=' && str[str_len-2] != '='){
		res_len = str_len/4*3-1;
	}
	else{
		res_len = str_len/4*3;
	}
	
	res = malloc(sizeof(unsigned char)*res_len+1);
	res[res_len] = '\0';
	for(i=0,j=0;j<res_len-1;i+=4,j+=3){
		res[j] = ((unsigned char)table[str[i]])<<2 | (((unsigned char)table[str[i+1]])>>4);
		res[j+1] = (((unsigned char)table[str[i+1]])<<4) | (((unsigned char)table[str[i+2]])>>2);
		res[j+2] = (((unsigned char)table[str[i+2]])<<6) | ((unsigned char)table[str[i+3]]);
	}
	void *tmp = res;
	TEE_MemMove(params[0].memref.buffer, tmp, res_len+1);
	IMSG("Test :%s %ld %ld %s", str, str_len, res_len, res);
	if (!buf)
		return TEE_ERROR_OUT_OF_MEMORY;
	free(res);
	TEE_Free(buf);
	
	return TEE_SUCCESS;
}
/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
			uint32_t cmd_id,
			uint32_t param_types, TEE_Param params[4])
{
	(void)&sess_ctx;

	switch (cmd_id) {
	case TA_ENCODE:
		return base64_en(param_types, params);
	case TA_DECODE:
		return base64_de(param_types, params);
	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}
