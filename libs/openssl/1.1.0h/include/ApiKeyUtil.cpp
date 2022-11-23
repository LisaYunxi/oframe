#include "ApiKeyUtil.h"
#include <openssl/hmac.h>

string GenerateSign(string SecretKey, string RequestTimestamp, string RequestMethod, string RequestPath, string RequestBody)
{
	string data = RequestTimestamp + RequestMethod + RequestPath + RequestBody;
	return HmacShaEncode(SecretKey.c_str(), data.c_str());
}

string HmacShaEncode(const char *Key, const char *Input)
{
	unsigned char *mac = NULL;
	unsigned int macLen = 0;
	const EVP_MD *engine = EVP_sha256();

	mac = (unsigned char*)malloc(EVP_MAX_MD_SIZE);

	HMAC_CTX *ctx = HMAC_CTX_new();
	HMAC_Init_ex(ctx, Key, (int)strlen(Key), engine, NULL);
	HMAC_Update(ctx, (unsigned char*)Input, strlen(Input));

	HMAC_Final(ctx, mac, &macLen);
	HMAC_CTX_free(ctx);

	char signature[128];
	EVP_EncodeBlock((unsigned char *)signature, mac, macLen);
	return signature;
}
