#ifndef UDA_PLUGIN_EXP2IMAS_EXP2IMAS_SSH_SERVER_H
#define UDA_PLUGIN_EXP2IMAS_EXP2IMAS_SSH_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

int ssh_run_server(const char* experiment, const char* ssh_host, const char* remote_host);

#ifdef __cplusplus
}
#endif

#endif // UDA_PLUGIN_EXP2IMAS_EXP2IMAS_SSH_SERVER_H