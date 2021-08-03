#include<err.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<sys/sysinfo.h>

/* TEE Client API */
#include<tee_client_api.h>

/* UUID */
#include<gettime_ta.h>

int main(void)
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_Operation op_time_REE;
	TEEC_Operation op_time_TEE;
	TEEC_UUID uuid = TA_GETTIME_UUID;
	uint32_t err_origin;
	
	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
		
	/*
	 * Open a session to TA, the TA will print "hello
	 * world!" in the log when the session is created.
	 */
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);
			
	/* Clear the TEEC_Operation struct */
	memset(&op, 0, sizeof(op));
	
	op_time_REE.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op_time_TEE.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	
	/* TEE_TIME */		 
	res = TEEC_InvokeCommand(&sess, TA_CMD_GET_SYSTEM_TIME, &op_time_TEE,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);
	printf("TIME_TEE seconds: %d\n", op_time_TEE.params[0].value.a);
	printf("TIME_TEE milliseconds: %d\n", op_time_TEE.params[0].value.b);
	
	/* REE_TIME */
	res = TEEC_InvokeCommand(&sess, TA_CMD_GET_REE_TIME, &op_time_REE,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);
	printf("TIME_REE seconds: %d\n", op_time_REE.params[0].value.a);
	printf("TIME_REE milliseconds: %d\n", op_time_REE.params[0].value.b);
	
	TEEC_CloseSession(&sess);
	
	TEEC_FinalizeContext(&ctx);

	return 0;
}
