#include "mongoose.h"
#include "version.h"

static int s_done = 0;
static int s_is_connected = 0;
char g_frontip[256];
vector<string> g_jsons;
bool g_bLoop = false;
int g_nCount = 0;
time_t g_nLastCountTime = time(NULL);
int g_nLatency = 0;
int g_nLatencyAll = 0;

void CmdLineArgError()
{
	printf("Usage: wsclient (ws://ip:port [-l] json [json] [json] ... ))/[-v] \n");
	printf("       wsclient ws://127.0.0.1:8000 {\"SendUserLogin\":{\"UserID\":\"100101\",\"Password\":\"111111\"}} {\"SendTopicAction\":{\"Action\":\"1\",\"LocalNo\":2,\"TopicID\":\"8\",\"FilterType\":\"1\",\"FilterValue\":\"OFEX_BTCUSDT\",\"ResumeNo\":-1}} :登陆并订阅私有主题\n");
	printf("       wsclient ws://47.103.71.106:8000 {\"SendTopicAction\":{\"Action\":\"1\",\"LocalNo\":2,\"TopicNo\":8,\"FilterType\":\"1\",\"FilterValue\":\"OFEX_BTCUSDT\",\"ResumeNo\":-1}}	:直接订阅公共主题\n");
	printf("       wsclient ws://47.103.71.106:8000 -l {\"SendOrderInsert\":{\"Direction\":0,\"Price\":20000,\"ExchangeID\":\"Test\",\"InstrumentID\":\"BTCUSDT\"}}	:重复发送报单命令\n");
	printf("       wsclient -v :显示版本信息\n");
	exit(0);
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
	(void)nc;
	switch (ev)
	{
	case MG_EV_CONNECT:
	{
		int status = *((int *)ev_data);
		if (status != 0)
			printf("-- Connection error: %d\n", status);
		else
			printf("-- Connected\n");
		break;
	}
	case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
	{
		struct http_message *hm = (struct http_message *) ev_data;
		if (hm->resp_code == 101)
		{
			s_is_connected = 1;
			for (int i = 0; i < g_jsons.size(); i++)
			{
				mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, g_jsons[i].c_str(), g_jsons[i].size());
				//mg_if_can_send_cb(nc);
				printf("%s \n", g_jsons[i].c_str());
			}
		}
		else
		{
			printf("-- Connection failed! HTTP code %d\n", hm->resp_code);
			/* Connection will be closed after this. */
		}
		break;
	}
	case MG_EV_WEBSOCKET_FRAME:
	{
		g_nCount++;
		struct websocket_message *wm = (struct websocket_message *) ev_data;
		if (g_bLoop)
		{
#ifdef WIN32	
			struct _timeb timeout;
			_ftime(&timeout);
			struct tm tt;
			LOCALTIME(&tt, &(timeout.time));
			int nLantency = tt.tm_sec * 1000 + timeout.millitm - g_nLatency;
			if (nLantency < 0)
				nLantency += 60 * 1000;
			g_nLatencyAll += nLantency;
#else
			struct timeval timeout;
			gettimeofday(&timeout, 0);
			struct tm tt;
			LOCALTIME(&tt, &(timeout.tv_sec));
			int nLantency = tt.tm_sec * 1000000 + timeout.tv_usec - g_nLatency;
			if (nLantency < 0)
				nLantency += 60 * 1000000;;
			g_nLatencyAll += nLantency;
#endif

			//printf("%.*s\n", (int)wm->size, wm->data);
			if (g_nCount > 50000)
			{
				time_t now = time(NULL);
				time_t nTimeGap = now - g_nLastCountTime;
				double nPackages = ((double)g_nCount) / nTimeGap;
				double nLantancy = ((double)g_nLatencyAll) / g_nCount;
				printf("-> %0.f packages/s of [%0.f] us(ms)/each \n", nPackages, nLantancy);
				g_nLastCountTime = now;
				g_nCount = 0;
				g_nLatencyAll = 0;
			}

			for (int i = 0; i < g_jsons.size(); i++)
			{
				mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, g_jsons[i].c_str(), g_jsons[i].size());
				//mg_if_can_send_cb(nc);
				//printf("%s \n", g_jsons[i].c_str());
			}

#ifdef WIN32	
			_ftime(&timeout);
			LOCALTIME(&tt, &(timeout.time));
			g_nLatency = tt.tm_sec * 1000 + timeout.millitm;
#else
			gettimeofday(&timeout, 0);
			LOCALTIME(&tt, &(timeout.tv_sec));
			g_nLatency = tt.tm_sec * 1000000 + timeout.tv_usec;
#endif
		}
		else
		{
			printf("%.*s\n", (int)wm->size, wm->data);
		}
		break;
	}
	case MG_EV_CLOSE:
	{
		if (s_is_connected) printf("-- Disconnected\n");
		s_done = 1;
		break;
	}
	}
}

int main(int argc, char* argv[])
{
	showVersion(argc, argv);

	if (argc < 3)
	{
		CmdLineArgError();
	}

	struct mg_mgr mgr;
	struct mg_connection *nc, *tmp;
	const char *chat_server_url = argv[1];

	if (!strcmp(argv[2], "-l"))
	{
		g_bLoop = true;
		for (int i = 3; i < argc; i++)
		{
			g_jsons.push_back(argv[i]);
		}

	}
	else
	{
		for (int i = 2; i < argc; i++)
		{
			g_jsons.push_back(argv[i]);
		}
	}

	mg_mgr_init(&mgr, NULL);

	nc = mg_connect_ws(&mgr, ev_handler, chat_server_url, "ws_chat", NULL);
	if (nc == NULL)
	{
		fprintf(stderr, "Invalid address\n");
		return 1;
	}

	while (!s_done)
	{
		mg_mgr_poll(&mgr, 1);

		//配合多线程改造修改
		for (nc = mgr.active_connections; nc != NULL; nc = tmp)
		{
			tmp = nc->next;
			mg_if_can_send_cb(nc);
		}
	}
	mg_mgr_free(&mgr);

	return 0;
}
