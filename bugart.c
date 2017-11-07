#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>

#include "bugart.h"

// hax!  libevent doesn't export evkeyvalq definition properly
#include <sys/queue.h>

Map makeMap(void * dummy, ...)
{
	va_list ap;
	char * key, * value;
	Map newMap = trie_new();

	va_start(ap, dummy);
	while((key = va_arg(ap, char *)) && (value = va_arg(ap, char *))) {
		trie_add(newMap, key, strlen(key), value);
	}

	return newMap;
}

response_s * response_s_new(struct evbuffer * buffer)
{
	response_s * r = (response_s *) malloc(sizeof(response_s));
	r->code = 200;
	r->buffer = buffer;
	return r;
}

void response_s_free(response_s * r)
{
	free(r);
}

request_s * request_s_new(struct evhttp_request * ev_req)
{
	request_s * r = (request_s *) malloc(sizeof(request_s));
	r->ev_req = ev_req;
	r->uri = evhttp_request_uri(ev_req);
	r->params = (struct evkeyvalq *) malloc(sizeof(struct evkeyvalq));
	evhttp_parse_query(r->uri, r->params);
	return r;
}

void request_s_free(request_s * r)
{
	if(r->params) {
		evhttp_clear_headers(r->params);
		free(r->params);
	}
	free(r);
}


bool match_uri(const char * pattern, const char * uri)
{
	while(*pattern && *uri) {
		if(*pattern == *uri) {
			pattern++;
			uri++;
		} else if(*pattern == '*') {
			if(*uri == '/' || !*(uri+1)) {
				pattern++;
			} else {
				uri++;
			}
		} else {
			return false;
		}
	}
	return (!*pattern && !*uri) || (!*pattern && *uri);
}

route_s * match_route(route_s * route, request_s * req)
{
	while(route) {
		if(req->ev_req->type == route->type && match_uri(route->pattern, req->ev_req->uri))
			return route;
		route = route->next;
	}

	return NULL;
}

route_s * nextroute_s(char * pattern, enum evhttp_cmd_type type, bugart_context_s * bugart)
{
	route_s * new_route = (route_s *) malloc(sizeof(route_s));
	new_route->pattern = pattern;
	new_route->next = NULL;
	new_route->type = type;
	if(bugart->route) {
		route_s * cursor = bugart->route;
		while(cursor->next)
			cursor = cursor->next;
		cursor->next = new_route;
	} else {
		bugart->route = new_route;
	}
	return new_route;
}

const char * get_param(request_s * request, const char * key)
{
	return evhttp_find_header(request->params, key);
}

void render_text(response_s * response, char * template, Map args)
{
	char anchor[] = "%{";
	char * cursor;
	char * val;
	while((cursor = strstr(template, anchor))) {
		evbuffer_add(response->buffer, template, cursor - template);
		template = cursor + sizeof(anchor) - 1;
		cursor = strchr(template, '}');
		val = trie_get(args, template, cursor - template);
		evbuffer_add(response->buffer, val, strlen(val));
		template = cursor + 1;
	}
	evbuffer_add(response->buffer, template, strlen(template));
}

char * loadTemplate(char * filename)
{
	char * result;
	int size = 0;
	FILE *f = fopen(filename, "r");
	if(!f)
		return NULL;

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);

	result = (char *) malloc(size + 1);
	fread(result, sizeof(char), size, f);
	fclose(f);

	result[size] = 0;

	return result;
}

void render_template(response_s * response, char * filename, Map args)
{
	char * template = loadTemplate(filename);
	render_text(response, template, args);
	free(template);
}

void set_body(response_s * response, const char * pattern, ...)
{
	va_list ap;
	va_start(ap, pattern);
	evbuffer_add_vprintf(response->buffer, pattern, ap);
}

void finalizeroute_ss(route_s * route)
{
	while(route) {
		// todo: add finializing routes here
		//route->handler = //Block_copy(route->handler);
		printf("Finalizing route %p\n",route);
		route = route->next;
	}
}

void setuphandler_func_ts(bugart_context_s *);

void request_handler(struct evhttp_request * ev_req, void * context)
{
	struct timeval t0, t1, tr;

	bugart_context_s * bugart = (bugart_context_s *) context;

	gettimeofday(&t0, NULL);

	request_s * request = request_s_new(ev_req);
	response_s * response = response_s_new(evbuffer_new());

	route_s * matched_route = match_route(bugart->route, request);

	if(matched_route) {
		matched_route->handler(request, response);
		evhttp_send_reply(ev_req, response->code, "OK", response->buffer);
	} else {
		bugart->not_found(request, response);
		evhttp_send_reply(ev_req, 404, "Not Found", response->buffer);
	}

	request_s_free(request);
	if(response->buffer)
		evbuffer_free(response->buffer);
	response_s_free(response);

	gettimeofday(&t1, NULL);
	timersub(&t1, &t0, &tr);
	printf("request_s processed in: %lu secs, %lu usecs\n", tr.tv_sec, tr.tv_usec);
}

void handler_not_found(request_s * request, response_s * response)
{
	printf("handler_func_t: Not found\n");
}

void setupBUGART(bugart_context_s * bugart)
{
	bugart->not_found = handler_not_found;
	setuphandler_func_ts(bugart);
}

void start_bugart(uint16_t port, bugart_context_s * bugart)
{
	bugart->port = port;

	setupBUGART(bugart);

	struct event_base * base = event_init();
	bugart_global_context.http = evhttp_new(base);
	evhttp_bind_socket(bugart_global_context.http, "127.0.0.1", bugart->port);
	evhttp_set_gencb(bugart_global_context.http, request_handler, bugart);

	printf("Showtime! Bugart's ready on camera %u...\n", bugart->port);

	event_loop(0);
}
/*
void model_create(redis_model_s model, char * fields[], char * key, Map attrs) {
	int i;
	for(i = 0; i < model.fieldCount ; i++) {
		char * field = fields[i];
		char * attr = trie_get(attrs, field, strlen(field));
	if(attr)
		redisCommand(model.redisFd, "HSET %s:%s %s %s", model.name, key, field, attr);
    }
}

Map modelGet(redis_model_s model, char * fields[], char * key) {
	int i;
	Map record = trie_new();
	for(i = 0; i < model.fieldCount ; i++) {
		char * field = fields[i];
		redisReply * reply = redisCommand(model.redisFd, "HGET %s:%s %s", model.name, key, field);
	if(reply->type == REDIS_REPLY_STRING)
		trie_add(record, field, strlen(field), reply->reply);
    }
}
*/
