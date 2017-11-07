#include <event.h>
#include <evhttp.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <hiredis.h>
#include "trie.h"

typedef struct
{
	const char * uri;
	struct evkeyvalq * params;
	struct evhttp_request * ev_req;
} request_s;

typedef struct
{
	int code;
	struct evbuffer * buffer;
} response_s;

typedef void (*handler_func_t)(request_s *, response_s *);

typedef struct _route_s
{
	enum evhttp_cmd_type type;
	char * pattern;
	handler_func_t handler;
	struct _route_s * next;
} route_s;

typedef struct
{
	uint16_t port;
	void (*init_func)();
	handler_func_t not_found;
	route_s * route;
	redisContext *rc;
	struct evhttp * http;
} bugart_context_s;

#define BUGART_CONTEXT_DEFINE					\
	bugart_context_s bugart_global_context;			\
	void int_handler_func(int unused)			\
	{							\
		event_loopbreak();				\
		if(bugart_global_context.http)			\
			evhttp_free(bugart_global_context.http);\
		FREE_REDIS;					\
		printf("\nUser break received. Exiting.\n");	\
	}


#define BUGART 	\
	void setuphandler_func_ts(bugart_context_s * bugart)

#define START(_port)						\
	finalizeroute_ss(bugart->route);			\
	}							\
								\
	int main() {						\
		/* set processing for Ctrl-C */			\
		signal(SIGINT, int_handler_func);		\
		start_bugart(_port, &bugart_global_context);	\
		return 0;

#define GET(_pattern,_function)		\
	nextroute_s(_pattern, EVHTTP_REQ_GET, bugart)->handler = _function;

#define POST(_pattern,_function)	\
	nextroute_s(_pattern, EVHTTP_REQ_POST, bugart)->handler = _function;

#define STATUS(_status)			\
	response->code = _status;

#define BODY(_pattern, ...)		\
	set_body(response, _pattern, ##__VA_ARGS__)

#define PARAMS(_key) get_param(request, _key)

const char * get_param(request_s *, const char *);
void set_body(response_s *, const char *, ...);
void start_bugart(uint16_t, bugart_context_s *);
route_s * nextroute_s(char *, enum evhttp_cmd_type, bugart_context_s *);
void finalizeroute_ss(route_s *);

typedef struct
{
	char * key;
	char * value;
} char_tuple_s;

typedef trie_s * Map;

#define map(...) makeMap(NULL, __VA_ARGS__, NULL)
Map makeMap(void *, ...);

#define render(_template, _map) render_text(response, _template, _map)
void render_text(response_s *, char *, Map);

#define view(_filename, _map) render_template(response, _filename, _map)
void render_template(response_s *, char *, Map);

/*
typedef struct _ModelField
{
	char * fieldName;
	struct _ModelField * next;
} ModelField;

typedef struct
{
	char * name;
	int redisFd;
	ModelField * field;
} redis_model_s;

void model_create(redis_model_s, char **, char *, Map);
Map modelGet(redis_model_s, char **, char *);

#define MODEL(_name, ...)			\
	char * _name##_fields[] = __VA_ARGS__;	\
	redis_model_s _name##_model = { #_name, _redisFd, _name##_fields, sizeof(_name##_fields) }
*/

#define USE_REDIS	\
	bugart_global_context.rc = redisConnect("127.0.0.1", 6379)

#define FREE_REDIS	\
	redisFree(bugart_global_context.rc)

extern bugart_context_s bugart_global_context;
