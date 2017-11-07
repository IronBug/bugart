#include <stdio.h>
#include "bugart.h"

BugartContextDefine;

void handler_hello(Request * request, Response * response)
{
	body("<h1>Hello, World!</h1>");
}

void handler_create(Request * request, Response * response)
{
	const char *id = params("id");
	const char *name = params("name");
	if(id && name) {
		redisReply *reply = redisCommand(globalContext.rc, "HSET User:%s %s %s", id, "name", name);
		if(reply) {
			body("User created.");
			freeReplyObject(reply);
		}
	} else {
		body("Error: Params \"id\" and \"name\" required.");
	}
}

void handler_show(Request * request, Response * response)
{
	const char *id = params("id");
	if(id) {
		redisReply *reply = redisCommand(globalContext.rc, "HGET User:%s %s", id, "name");
		if(reply) {
			if(reply->str) {
				trie *m =  map("name", reply->str);
				view("index.cml",m);
				trie_free(m);
			} else {
				body("No such user2.");
			}
			freeReplyObject(reply);
		}
	} else {
		body("Error: Param \"id\" required.");
	}
}

Bugart {
	UseRedis;
	//Model(User, "name", "email");

	get("/hello",handler_hello);

	get("/create",handler_create);

	get("/show",handler_show);

	Start(11000);
}

