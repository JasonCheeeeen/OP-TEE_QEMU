#include<err.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<sys/sysinfo.h>
#include<time.h>

/* TEE Client API */
#include<tee_client_api.h>

/* UUID */
#include<random_ta.h>

int main(void)
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op = {0};
	TEEC_UUID uuid = TA_RANDOM_UUID;
	uint8_t random_test[16] = {0};
	uint32_t err_origin;
	int i;
	
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
	
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT,
					 TEEC_NONE, TEEC_NONE, TEEC_NONE);
	op.params[0].tmpref.buffer = random_test;
	op.params[0].tmpref.size = sizeof(random_test);
	
	printf("original random array...... \n");
	srand(time(0));
	for(i=0;i<16;i++){
		random_test[i] = rand();
		printf("%d ",random_test[i]);
	}
	
	res = TEEC_InvokeCommand(&sess, TA_RANDOM_SORT,
				 &op, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);
	printf("\n");
	printf("sort by TA......");
	printf("\n");
	for (i = 0; i < 16; i++)
		printf("%d ", random_test[i]);
	printf("\n");
	
	TEEC_CloseSession(&sess);
	
	TEEC_FinalizeContext(&ctx);

	return 0;
}
