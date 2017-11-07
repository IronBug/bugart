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
		body("User created.");
		freeReplyObject(reply);
	} else {
		body("Error: Params \"id\" and \"name\" required.");
	}
}

void handler_show(Request * request, Response * response)
{
	const char *id = params("id");
	if(id) {
		redisReply *reply = redisCommand(globalContext.rc, "HGET User:%s %s", id, "name");
		view("index.cml", map("name", reply->str));
		freeReplyObject(reply);
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

        //FreeRedis; // todo: put FreeRedis on closing everything.
}
