#include <stdio.h>
#include "bugart.h"

void handler_hello(Request * request, Response * response) {
                body("<h1>Hello, World!</h1>");
}

Bugart {
        UseRedis;
        //Model(User, "name", "email");

        get("/hello",handler_hello);

#if 0
        get("/create") {
                redisCommand(_redisFd, "HSET User:%s %s %s", params("id"), "name", params("name"));
                body("User created.");
        });

        get("/show") {
                redisReply *reply = redisCommand(_redisFd, "HGET User:%s %s", params("id"), "name");
                view("index.cml", map("name", reply->str));
        });
#endif // 0
        Start(11000);
        //FreeRedis;
}
