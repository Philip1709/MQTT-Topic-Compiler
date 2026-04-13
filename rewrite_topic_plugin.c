#include <mosquitto_plugin.h>
#include <mosquitto.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static int rewrite_topic_callback(int event, void *event_data, void *userdata)
{
    struct mosquitto_evt_message *ed = event_data;

    if(!ed || !ed->client || !ed->topic) return MOSQ_ERR_SUCCESS;

    const char *clientid = mosquitto_client_id(ed->client);
    if(!clientid || strlen(clientid) == 0) clientid = "anon";

    const char *orig_topic = ed->topic;

    // Only rewrite if it matches your target topic
    if(strncmp(orig_topic, "v1/devices/me/telemetry", 23) == 0) {
        char new_topic[256];
        snprintf(new_topic, sizeof(new_topic), "iSTEP/%s/telemetry", clientid);

        mosquitto_property *properties = NULL;

        // Replace topic
        ed->topic = strdup(new_topic);
    }

    return MOSQ_ERR_SUCCESS;
}

int mosquitto_plugin_version(int supported_version_count, const int *supported_versions)
{
    for(int i = 0; i < supported_version_count; i++){
        if(supported_versions[i] == 5){
            return 5;
        }
    }
    return -1; // Unsupported
}

int mosquitto_plugin_init(mosquitto_plugin_id_t *identifier, void **userdata, struct mosquitto_opt *opts, int opt_count)
{
    mosquitto_callback_register(identifier, MOSQ_EVT_MESSAGE, rewrite_topic_callback, NULL, NULL);
    return MOSQ_ERR_SUCCESS;
}

int mosquitto_plugin_cleanup(void *userdata, struct mosquitto_opt *opts, int opt_count)
{
    return MOSQ_ERR_SUCCESS;
}
