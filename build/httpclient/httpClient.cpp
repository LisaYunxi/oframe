#include <iostream>
#include "mongoose.h"
#include "version.h"

static int s_exit_flag = 0;

static void ev_handler(struct mg_connection* nc, int ev, void* ev_data)
{
	struct http_message* hm = (struct http_message*) ev_data;
	int connect_status;

	switch (ev)
	{
	case MG_EV_CONNECT:
	{
		connect_status = *(int*)ev_data;
		if (connect_status != 0) {
			printf("Error connecting: %s\n", strerror(connect_status));
			s_exit_flag = 1;
		}
		break;
	}
	case MG_EV_HTTP_REPLY:
	{
		printf("Got reply:\n%.*s\n", (int)hm->body.len, hm->body.p);
		nc->flags |= MG_F_SEND_AND_CLOSE;
		s_exit_flag = 1;
		break;
	}
	case MG_EV_CLOSE:
	{
		if (s_exit_flag == 0) {
			printf("Server closed connection\n");
			s_exit_flag = 1;
		};
		break;
	}
	default:
		break;
	}
}

int main(int argc, char* argv[])
{
	showVersion(argc, argv);

	while (true)
	{
		struct mg_mgr mgr;
		struct mg_connection* nc;

		time_t beginTime = time(NULL);
		mg_mgr_init(&mgr, NULL);
		nc = mg_connect_http(&mgr, ev_handler, "127.0.0.1", NULL, NULL);
		mg_set_protocol_http_websocket(nc);

		s_exit_flag = 0;
		while (s_exit_flag == 0)
		{
			mg_mgr_poll(&mgr, 1);
		}
		time_t endTime = time(NULL);
		size_t used = endTime - beginTime;

		mg_mgr_free(&mgr);
	}
}
