#include <stdio.h>
#include <stdint.h>
#include <libubus.h>
#include <syslog.h>
#include <uci.h>

static struct ubus_context *ctx = NULL;
static struct ubus_request_data req_data;
static struct blob_buf bb;

enum
{
	UBUS_RND_ID,
	UBUS_RND_DATA,
	UBUS_RND_MSG,
	UBUS_RND_MAX,
};
enum
{
	L_CRIT,
	L_WARNING,
	L_NOTICE,
	L_INFO,
	L_DEBUG,
	L_MAX,
};

static const int log_class[] = {
	[L_CRIT] = LOG_CRIT,
	[L_WARNING] = LOG_WARNING,
	[L_NOTICE] = LOG_NOTICE,
	[L_INFO] = LOG_INFO,
	[L_DEBUG] = LOG_DEBUG};

//Add notification
static struct ubus_event_handler interface_event_handler;

enum {
	EVT_INTF,
	EVT_ACTN,
};

static const struct blobmsg_policy event_policy[] = {
	[EVT_INTF] = {.name = "interface", .type = BLOBMSG_TYPE_STRING},
	[EVT_ACTN] = {.name = "action", .type = BLOBMSG_TYPE_STRING},
};

static void ubus_receive_interface_event(
	__attribute__((unused)) struct ubus_context* ctx,
	__attribute__((unused)) struct ubus_event_handler* ev,
	__attribute__((unused)) const char* type, struct blob_attr* msg)
{
	const char* interface = NULL;
	const char* action = NULL;
	struct blob_attr* tb[ARRAY_SIZE(event_policy)];

	blobmsg_parse(event_policy, ARRAY_SIZE(event_policy), tb, blob_data(msg),
				  blob_len(msg));

	if (tb[EVT_INTF] && tb[EVT_ACTN]) {
		interface = blobmsg_get_string(tb[EVT_INTF]);
		action = blobmsg_get_string(tb[EVT_ACTN]);
		notify_interface(interface, action);
	}
}

bool ubus_listen_network_events(void)
{
	/* ubus event listener */
	memset(&interface_event_handler, 0, sizeof(interface_event_handler));
	interface_event_handler.cb = ubus_receive_interface_event;
	int ret = ubus_register_event_handler(ctx, &interface_event_handler,
										  "network.interface");
	if (ret < 0) {
		return false;
	}

	ubus_add_uloop(ctx);
	return true;
}
//End notification

static int ubus_rnd_handler(struct ubus_context *ctx, struct ubus_object *obj,
							struct ubus_request_data *req,
							const char *method, struct blob_attr *msg);

struct blobmsg_policy ubus_rnd_policy[] = {
	[UBUS_RND_ID] = {.name = "id", .type = BLOBMSG_TYPE_INT32},
	[UBUS_RND_DATA] = {.name = "data", .type = BLOBMSG_TYPE_INT32},
	[UBUS_RND_MSG] = {.name = "msg", .type = BLOBMSG_TYPE_STRING},
};

struct ubus_method ubus_rnd_methods[] =
	{
		UBUS_METHOD("ubus_methd", ubus_rnd_handler, ubus_rnd_policy),
};

static struct ubus_object_type main_object_type =
	UBUS_OBJECT_TYPE("ubus_rnd", ubus_rnd_methods);

static struct ubus_object main_object =
	{
		.name = "RAHUL_OBJ",
		.type = &main_object_type,
		.methods = ubus_rnd_methods,
		.n_methods = ARRAY_SIZE(ubus_rnd_methods),
};

void my_log_message(int priority, const char *format, ...)
{
	va_list vl;

	va_start(vl, format);
	vsyslog(log_class[priority], format, vl);

	va_end(vl);
}

void ubus_rnd_init()
{
	char *ubus_socket = NULL;

	//initialize the uloop context
	uloop_init();
	ctx = ubus_connect(ubus_socket);
	if (ctx == NULL)
		printf("Failed to create the context\n");

	ubus_add_uloop(ctx);
	int ret = ubus_add_object(ctx, &main_object);
	if (ret != 0)
	{
		printf("Failed to register an object to ubus\n");
		goto UBUS_FAIL;
	}
	else
	{
		printf("Register object with ubus\n");
	}

	uloop_run();
	uloop_done();
	printf("TEST HELLO WORLD\n");

UBUS_FAIL:
	ubus_free(ctx);
}

int main()
{
	printf("HELLO WORLD\n");
	my_log_message(log_class[LOG_DEBUG], "UBUS_ID Hello world");
	struct uci_package *my_config_pkg = NULL;
	struct uci_context *context = uci_alloc_context();

	my_config_pkg = calloc(1,sizeof(struct uci_package));

	if(uci_load(context , "/etc/config/alm",&my_config_pkg)){
		system("echo uci_load failed >> /tmp/xyz");
	}
	struct uci_element *e =NULL;
	struct uci_element *s = NULL;
	struct uci_ptr ptr;

	uci_foreach_element(&my_config_pkg->sections, e) {

		s = uci_to_section(e);
		fprintf(stderr,"get section\n");
		{
			char val[64] = {'\0'};
			memset(val,0,sizeof(val));
			sprintf(val,"alm.comm.A_list=%s",'x');
			if(uci_lookup_ptr(context,&ptr,val,true) == UCI_OK) {
				fprintf(stderr,"get pointer\n");
				if(uci_set(context,&ptr) == UCI_OK){
					uci_commit(context,&my_config_pkg,true);
					fprintf(stderr,"commit the uci file\n");
				}


				}
		}

	}


	ubus_rnd_init();

	return 0;
}

static int ubus_rnd_handler(struct ubus_context *ctx, struct ubus_object *obj,
							struct ubus_request_data *req, const char *method,
							struct blob_attr *msg)
{

	struct blob_attr *tb[UBUS_RND_MAX]; /* for parsed attr */

	/* Parse blob_msg from the caller to request policy */
	blobmsg_parse(ubus_rnd_policy, ARRAY_SIZE(ubus_rnd_policy), tb, blob_data(msg), blob_len(msg));
	/* print request msg */
	my_log_message(log_class[LOG_DEBUG], "Receive msg from caller: ");

	if (tb[UBUS_RND_ID])
	{
		printf("UBUS_ID=%u  ", blobmsg_get_u32(tb[UBUS_RND_ID]));
		my_log_message(log_class[LOG_DEBUG], "UBUS_ID =%u", blobmsg_get_u32(tb[UBUS_RND_ID]));
	}
	if (tb[UBUS_RND_DATA])
	{
		printf("DATA=%u  ", blobmsg_get_u32(tb[UBUS_RND_DATA]));
		my_log_message(log_class[LOG_DEBUG], "UBUS_ID =%u", blobmsg_get_u32(tb[UBUS_RND_ID]));
	}
	if (tb[UBUS_RND_MSG])
	{
		printf("MSG='%s' \n", (char *)blobmsg_data(tb[UBUS_RND_MSG]));
		my_log_message(log_class[LOG_DEBUG], "UBUS_ID =%u", blobmsg_get_u32(tb[UBUS_RND_ID]));
	}
	/* Do some job here according to caller's request */

	/* send a reply msg to the caller for information */
	blob_buf_init(&bb, 0);
	blobmsg_add_string(&bb, "Rahul reply", "Request is being proceeded!");
	ubus_send_reply(ctx, req, bb.head);

	/* 	-----  reply results to the caller -----
	 * NOTE: we may put proceeding job in a timeout task, just to speed up service response.
	 */
	//	ubus_defer_request(ctx, req, &req_data);

	//	ubus_complete_deferred_request(ctx, req, UBUS_STATUS_OK);
}
