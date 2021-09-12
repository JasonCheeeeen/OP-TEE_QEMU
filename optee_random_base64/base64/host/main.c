#include<err.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<sys/sysinfo.h>
#include<time.h>

/* TEE Client API */
#include<tee_client_api.h>

/* UUID */
#include<base64_ta.h>

int main(void)
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op = {0};
	TEEC_UUID uuid = TA_BASE64_UUID;
	unsigned char encode_in[100];
	unsigned char decode_in[100];
	char check_input[10];
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
	printf("Choose encode or decode......\n");
	scanf("%s",check_input);
	if(strcmp(check_input,"encode") == 0){
	
		printf("input...... \n");
		scanf("%s",encode_in);
		
		op.params[0].tmpref.buffer = encode_in;
		op.params[0].tmpref.size = sizeof(encode_in);
		
		res = TEEC_InvokeCommand(&sess, TA_ENCODE,
					 &op, &err_origin);
		if (res != TEEC_SUCCESS)
			errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
				res, err_origin);
		
		printf("endode by TA......");
		printf("\n");
		printf("%s",encode_in);
		printf("\n");
	}
	else{
		printf("input...... \n");
		scanf("%s",decode_in);
		
		op.params[0].tmpref.buffer = decode_in;
		op.params[0].tmpref.size = sizeof(decode_in);
		
		res = TEEC_InvokeCommand(&sess, TA_DECODE,
					 &op, &err_origin);
		if (res != TEEC_SUCCESS)
			errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
				res, err_origin);
		
		printf("decode by TA......");
		printf("\n");
		printf("%s",decode_in);
		printf("\n");
	}
	
	TEEC_CloseSession(&sess);
	
	TEEC_FinalizeContext(&ctx);

	return 0;
}
