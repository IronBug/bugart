#include <stdio.h>
#include "bugart.h"

Bogart {
        UseRedis;
        //Model(User, "name", "email");

//       get("/hello")
	nextRoute("/hello", EVHTTP_REQ_GET, bugart)->handler = LAMBDA(void _(Request * request, Response * response)
        {
                body("<h1>Hello, World!</h1>");
        });

#if 0
        get("/create")
        {
                redisCommand(_redisFd, "HSET User:%s %s %s", params("id"), "name", params("name"));
                body("User created.");
        };

        get("/show")
        {
                redisReply * reply = redisCommand(_redisFd, "HGET User:%s %s", params("id"), "name");
                view("index.cml", map("name", reply->reply));
        };
#endif
        Start(11000)
}
