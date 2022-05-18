//
// Created by shved on 23.03.2021.
//
// pactl list sinks short
// 0 ... RUNNING
// 1 ... IDLE/SUSPENDED
// pactl set-default-sink 1
//

#include <QDebug>

#include "Pulse.h"

static void perform_callback(pa_context *c, Pulse *a) {
    pa_context_state_t s = pa_context_get_state(c);
    switch (s) {
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
            break;
        case PA_CONTEXT_READY:
            a->call(c);
            break;
        case PA_CONTEXT_FAILED:
        case PA_CONTEXT_TERMINATED:
        default:
            pa_mainloop_quit(a->operator pa_mainloop *(), s);
    }
}

void Pulse::perform(pa_context_notify_cb_t f) {
    pa_context *c = pa_context_new(pa_mainloop_get_api(mLoop), nullptr);
    if (c) {
        mCall = f;
        pa_context_set_state_callback(c, reinterpret_cast<pa_context_notify_cb_t>(perform_callback), this);
        if (pa_context_connect(c, nullptr, PA_CONTEXT_NOFLAGS, nullptr) == 0)
            pa_mainloop_run(mLoop, nullptr);
        pa_context_unref(c);
    }
}

static void get_sink_info_list_callback(pa_context *c, const pa_sink_info *i, int is_last, void *a) {
    if (is_last)
        return pa_context_disconnect(c);
    static std::map<int, const char *> pa_sink_state_name = {
            {PA_SINK_INVALID_STATE, "INVALID*"},
            {PA_SINK_RUNNING,       "RUNNING"},
            {PA_SINK_IDLE,          "IDLE"},
            {PA_SINK_SUSPENDED,     "SUSPENDED"},
            {PA_SINK_INIT,          "INIT*"},
            {PA_SINK_UNLINKED,      "UNLINKED*"},
    };
    static std::map<int, const char *> pa_port_available_name = {
            {PA_PORT_AVAILABLE_UNKNOWN, "UNKNOWN*"},
            {PA_PORT_AVAILABLE_NO,      "NO"},
            {PA_PORT_AVAILABLE_YES,     "YES"},
    };
    qDebug() << __PRETTY_FUNCTION__ << endl << i->index << pa_sink_state_name[i->state] << i->description << endl
             << "\tname:" << i->name << endl
             << "\tmute:" << i->mute << endl
             << "\towner_module:" << i->owner_module << endl
             << "\tmonitor_source_name:" << i->monitor_source_name << endl
             << "\tlatency:" << i->latency << endl
             << "\tconfigured_latency:" << i->configured_latency << endl
             << "\tbase_volume:" << i->base_volume << endl
             << "\tdriver:" << i->driver << endl
             << "\tcard:" << i->card << endl
             << "\tn_ports:" << i->n_ports;
    for (int j = 0; j < i->n_ports; ++j) {
        qDebug() << "\t\tname:" << i->ports[j]->name << endl
                 << "\t\tdescription:" << i->ports[j]->description << i->description << endl
                 << "\t\tpriority:" << i->ports[j]->priority << endl
                 << "\t\tavailable:" << pa_port_available_name[i->ports[j]->available];

    }

}

static void get_sink_info_list(pa_context *c, void *a) {
    pa_operation *o = pa_context_get_sink_info_list(
            c, get_sink_info_list_callback, a);
    if (o)
        pa_operation_unref(o);
}

static void get_card_info_list(pa_context *c, void *a) {
    pa_operation *o = pa_context_get_card_info_list(
            c, [](pa_context *c, const pa_card_info *i, int eol, void *a) {
                if (eol)
                    return pa_context_disconnect(c);
            }, a);
    if (o)
        pa_operation_unref(o);
}

void Pulse::availableSinks() {
//    perform(get_sink_info_list);
//    perform(reinterpret_cast<pa_context_notify_cb_t>(get_card_info_list));
}

// action = SET_DEFAULT_SINK;
// sink_name = 1
void Pulse::setSink(int id) {

}

Pulse::Pulse() : mLoop(pa_mainloop_new()) {}

Pulse::~Pulse() {
    pa_mainloop_free(mLoop);
}

void Pulse::call(pa_context *c) {
    mCall(c, this);
}
