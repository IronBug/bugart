#include <stdio.h>
#include "bugart.h"

Bugart {
        UseRedis;
        //Model(User, "name", "email");

        get("/hello") {
                body("<h1>Hello, World!</h1>");
        });

        get("/create") {
                redisCommand(_redisFd, "HSET User:%s %s %s", params("id"), "name", params("name"));
                body("User created.");
        });

#if 0
        get("/show") {
                redisReply *reply = redisCommand(_redisFd, "HGET User:%s %s", params("id"), "name");
                view("index.cml", map("name", reply->str));
        });
#endif // 0
        Start(11000);
        //FreeRedis;
}
