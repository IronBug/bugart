#include <stdio.h>
#include "bugart.h"

BUGART_CONTEXT_DEFINE;

void handler_hello(request_s * request, response_s * response)
{
	BODY("<h1>Hello, World!</h1>");
}

void handler_create(request_s * request, response_s * response)
{
	const char *id = PARAMS("id");
	const char *name = PARAMS("name");
	if(id && name) {
		redisReply *reply = redisCommand(bugart_global_context.rc, "HSET User:%s %s %s", id, "name", name);
		if(reply) {
			BODY("User created.");
			freeReplyObject(reply);
		}
	} else {
		BODY("Error: Params \"id\" and \"name\" required.");
	}
}

void handler_show(request_s * request, response_s * response)
{
	const char *id = PARAMS("id");
	if(id) {
		redisReply *reply = redisCommand(bugart_global_context.rc, "HGET User:%s %s", id, "name");
		if(reply) {
			if(reply->str) {
				trie_s *m =  map("name", reply->str);
				view("index.cml",m);
				trie_free(m);
			} else {
				BODY("No such user2.");
			}
			freeReplyObject(reply);
		}
	} else {
		BODY("Error: Param \"id\" required.");
	}
}

BUGART
{
	USE_REDIS;
	//MODEL(User, "name", "email");

	GET("/hello",handler_hello);

	GET("/create",handler_create);

	GET("/show",handler_show);

	START(11000);
}

