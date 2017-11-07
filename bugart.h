#include <event.h>
#include <evhttp.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <hiredis.h>
#include "trie.h"

typedef struct {
	const char * uri;
	struct evkeyvalq * params;
	struct evhttp_request * ev_req;
} Request;

typedef struct {
	int code;
	struct evbuffer * buffer;
} Response;

typedef void (*Handler)(Request *, Response *);

typedef struct _Route {
	enum evhttp_cmd_type type;
	char * pattern;
	Handler handler;
	struct _Route * next;
} Route;

typedef struct {
	uint16_t port;
	void (*init_func)();
	Handler not_found;
	Route * route;
	redisContext *rc;
	struct evhttp * http;
} BugartContext;

#define BugartContextDefine					\
	BugartContext globalContext;				\
	void int_handler_func(int unused)			\
	{							\
		event_loopbreak();				\
		if(globalContext.http)				\
			evhttp_free(globalContext.http);	\
		FreeRedis;					\
		printf("\nUser break received. Exiting.\n");	\
	}


#define Bugart 	\
	void setupHandlers(BugartContext * bugart)

#define Start(_port)					\
	finalizeRoutes(bugart->route);			\
	}						\
	int main() {					\
		/* set processing for Ctrl-C */		\
		signal(SIGINT, int_handler_func);	\
		startBugart(_port, &globalContext);	\
		return 0;

#define get_end() );

#define get(_pattern,_function)	\
	nextRoute(_pattern, EVHTTP_REQ_GET, bugart)->handler = _function;

#define post(_pattern)	\
	nextRoute(_pattern, EVHTTP_REQ_POST, bugart)->handler = ^ void (Request * request, Response * response)

#define status(_status)	\
	response->code = _status;

#define body(_pattern, ...)	\
	setBody(response, _pattern, ##__VA_ARGS__)

#define params(_key) getParam(request, _key)

const char * getParam(Request *, const char *);
void setBody(Response *, const char *, ...);
void startBugart(uint16_t, BugartContext *);
Route * nextRoute(char *, enum evhttp_cmd_type, BugartContext *);
void finalizeRoutes(Route *);

typedef struct {
	char * key;
	char * value;
} CharTuple;

typedef trie * Map;

#define map(...) makeMap(NULL, __VA_ARGS__, NULL)
Map makeMap(void *, ...);

#define render(_template, _map) renderText(response, _template, _map)
void renderText(Response *, char *, Map);

#define view(_filename, _map) renderTemplate(response, _filename, _map)
void renderTemplate(Response *, char *, Map);

/*
typedef struct _ModelField {
	char * fieldName;
	struct _ModelField * next;
} ModelField;

typedef struct {
	char * name;
	int redisFd;
	ModelField * field;
} RedisModel;

void modelCreate(RedisModel, char **, char *, Map);
Map modelGet(RedisModel, char **, char *);

#define Model(_name, ...)			\
	char * _name##_fields[] = __VA_ARGS__;	\
	RedisModel _name##Model = { #_name, _redisFd, _name##_fields, sizeof(_name##_fields) }
*/

#define UseRedis	\
	globalContext.rc = redisConnect("127.0.0.1", 6379)

#define FreeRedis	\
	redisFree(globalContext.rc)

extern BugartContext globalContext;
