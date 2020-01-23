#include "exp2imas_mds.h"

#include <regex.h>
#include <mdsobjects.h>

#include <logging/logging.h>
#include <clientserver/stringUtils.h>
#include <plugins/udaPlugin.h>

#include "exp2imas_ramCache.h"
#include "exp2imas_ssh.h"
#include "exp2imas_ssh_server.h"

namespace {

MDSplus::Connection* conn = nullptr;

int get_signal_length(MDSplus::Connection& connection, std::string signal)
{
    signal += "; SIZE(_sig);";

    try {
        MDSplus::Data* ret = connection.get(signal.c_str());
        int size = ret->getInt();
        MDSplus::deleteData(ret);
        return size;
    } catch (MDSplus::MdsException& ex) {
        UDA_LOG(UDA_LOG_ERROR, "Unable to get length of %s.\n", signal.c_str());
        return -1;
    }
}

constexpr int max_hosts = 10;

typedef struct ServerThreadData {
    const char* experiment;
    const char* ssh_hosts[max_hosts];
    int num_hosts;
    const char* mds_host;
} SERVER_THREAD_DATA;

void* server_task(void* ptr)
{
    auto data = (SERVER_THREAD_DATA*)ptr;
    ssh_run_server(data->experiment, data->ssh_hosts, data->num_hosts, data->mds_host);
    return nullptr;
}

void replace(char* out, char* in, const char* replace, const char* with)
{
    char* pos = strstr(in, replace);
    if (pos != nullptr) {
        char* tmp = StringReplaceAll(in, replace, with);
        sprintf(out, "%s", tmp);
        free(tmp);
    } else {
        sprintf(out, "%s", in);
    }
}

} // anon namespace

int mds_close()
{
    delete conn;
    conn = nullptr;
    return 0;
}

int mds_get(const char* experiment, const char* signalName, int shot, int run,
            float** time, float** data, int* len, int* time_len, int time_dim)
{
    if (conn == nullptr) {

        char host[100];

        if (StringIEquals(experiment, "TCV") || StringIEquals(experiment, "AUG")) {
            g_server_port = 0;
            g_initialised = false;

            pthread_cond_init(&g_initialised_cond, nullptr);
            pthread_mutex_init(&g_initialised_mutex, nullptr);

            pthread_t server_thread;
            SERVER_THREAD_DATA thread_data = {};
            thread_data.experiment = experiment;

            if (StringIEquals(experiment, "TCV")) {
                thread_data.ssh_hosts[0] = "lac911.epfl.ch";
                thread_data.num_hosts = 1;
                thread_data.mds_host = "tcvdata.epfl.ch";
            } else if (StringIEquals(experiment, "AUG")) {
                thread_data.ssh_hosts[0] = "gate1.aug.ipp.mpg.de";
                thread_data.ssh_hosts[1] = "gate2.aug.ipp.mpg.de";
                thread_data.ssh_hosts[2] = "gate3.aug.ipp.mpg.de";
                thread_data.num_hosts = 3;
                thread_data.mds_host = "mdsplus.aug.ipp.mpg.de";
            }

            pthread_create(&server_thread, nullptr, server_task, &thread_data);

            pthread_mutex_lock(&g_initialised_mutex);
            while (!g_initialised) {
                pthread_cond_wait(&g_initialised_cond, &g_initialised_mutex);
            }
            pthread_mutex_unlock(&g_initialised_mutex);

            pthread_mutex_destroy(&g_initialised_mutex);
            pthread_cond_destroy(&g_initialised_cond);

            struct timespec sleep_for = {};
            sleep_for.tv_sec = 0;
            sleep_for.tv_nsec = 100000000;
            nanosleep(&sleep_for, nullptr);

            sprintf(host, "localhost:%d", g_server_port);
        } else {
            strcpy(host, "mdsplus.jet.efda.org:8000");
        }

        try {
            conn = new MDSplus::Connection((char*)host);
        } catch (MDSplus::MdsException& ex) {
            UDA_LOG(UDA_LOG_ERROR, "Error connecting to %s.\n", host);
            return -1;
        }
    }

    char work[2048];

    regex_t re;
    int r = regcomp(&re, "([A-Z]+)\\|([A-Za-z0-9]+)", REG_EXTENDED);
    if (r) {
        fprintf(stderr, "regex failed to compile\n");
        RAISE_PLUGIN_ERROR("regex failed to compile");
    }

    regmatch_t matches[3];
    r = regexec(&re, signalName, 3, matches, 0);

    bool is_tdi = false;

    if (r == 0) {
        char* tmp = strdup(signalName);
        tmp[matches[1].rm_eo] = '\0';
        sprintf(work, R"(_sig=augdiag(%%SHOT%%,"%s","%s"))", tmp, &tmp[matches[2].rm_so]);
        free(tmp);
    } else if (STR_STARTSWITH(signalName, "%TDI%")) {
        sprintf(work, "%s", &signalName[5]);
        is_tdi = true;
    } else {
        sprintf(work, "%s", signalName);
    }

    char tmp1[2048];
    char tmp2[2048];
    char tmp3[2048];

    char shot_str[10];
    sprintf(shot_str, "%d", shot);
    replace(tmp1, work, "%SHOT%", shot_str);

    char run_str[10];
    sprintf(run_str, "%d", run);
    replace(tmp2, tmp1, "%SEQUENCE%", run_str);

    const char* username = getenv("UDA_EXP2IMAS_USERNAME");
    if (username == nullptr) {
        username = "";
    }
    replace(tmp3, tmp2, "%LOCALUSERNAME%", username);

    char signal[2048];

    if (is_tdi) {
        sprintf(signal, "%s", tmp3);
    } else if (StringIEquals(experiment, "JET")) {
        sprintf(signal, "_sig=jet(\"%s\",%d)", tmp3, shot);
    } else {
        sprintf(signal, "_sig=%s", tmp3);
    }

    fprintf(stderr, "fetching signal %s", signal);

    static RAM_CACHE* cache = nullptr;
    if (cache == nullptr) {
        cache = ram_cache_new(100);
    }

    char len_key[2048];
    char time_len_key[2048];
    char time_key[2048];
    char data_key[2048];

    sprintf(len_key, "%s/length", signal);
    sprintf(time_len_key, "%s/time_length", signal);
    sprintf(time_key, "%s/time", signal);
    sprintf(data_key, "%s/data", signal);

    auto cache_len = (int*)ram_cache_get(cache, len_key);
    if (cache_len != nullptr) {
        *len = *cache_len;
        if (time != nullptr) {
            *time_len = *(int*)ram_cache_get(cache, time_len_key);
            *time = (float*)malloc(*time_len * sizeof(float));
            memcpy(*time, (int*)ram_cache_get(cache, time_key), *time_len * sizeof(float));
        }
        *data = (float*)malloc(*len * sizeof(float));
        memcpy(*data, (int*)ram_cache_get(cache, data_key), *len * sizeof(float));
        fprintf(stderr, " -> from cache\n");
        return 0;
    }

    if (StringIEquals(experiment, "TCV")) {
        std::string buf = "TreeOpen('tcv_shot'," + std::to_string(shot) + ")";

        try {
            conn->get(buf.c_str());
        } catch (MDSplus::MdsException& ex) {
            UDA_LOG(UDA_LOG_ERROR, "Error opening tree for shot %d: %s.\n", shot, ex.what());
            return -1;
        }
    }

    //*len = get_signal_length(*conn, signal);

    //if (*len < 0) {
    //    fprintf(stderr, " -> unable to get signal length.\n");
    //    UDA_LOG(UDA_LOG_ERROR, "Unable to get signal length.\n");
    //    return -1;
    //}

    std::string buf;

    if (time != nullptr) {
        buf = std::string(signal) + "; dim_of(_sig, " + std::to_string(time_dim - 1) + ");";

        *time = nullptr;

        try {
            MDSplus::Data* ret = conn->get(buf.c_str());
            if (ret->dtype == DTYPE_FLOAT || ret->dtype == DTYPE_L) {
                float* fdata = ret->getFloatArray(time_len);
                size_t sz = *time_len * sizeof(float);
                *time = (float*)malloc(sz);
                memcpy(*time, fdata, sz);
                MDSplus::deleteData(ret);
            } else if (ret->dtype == DTYPE_DOUBLE || ret->dtype == DTYPE_D) {
                double* ddata = ret->getDoubleArray(time_len);
                size_t sz = *time_len * sizeof(float);
                *time = (float*)malloc(sz);
                for (int i = 0; i < *time_len; ++i) {
                    (*time)[i] = ddata[i];
                }
                MDSplus::deleteData(ret);
            } else {
                fprintf(stderr, " -> data returned in wrong format\n");
                MDSplus::deleteData(ret);
                return -1;
            }
        } catch (MDSplus::MdsException& ex) {
            fprintf(stderr, " -> unable to get signal\n");
            UDA_LOG(UDA_LOG_ERROR, "Unable to get signal.\n");
            return -1;
        }
    }


    buf = std::string(signal) + "; _sig;";

    *data = nullptr;

    try {
        MDSplus::Data* ret = conn->get(buf.c_str());
        if (ret->dtype == DTYPE_FLOAT || ret->dtype == DTYPE_L) {
            float* fdata = ret->getFloatArray(len);
            size_t sz = *len * sizeof(float);
            *data = (float*)malloc(sz);
            memcpy(*data, fdata, sz);
            MDSplus::deleteData(ret);
        } else if (ret->dtype == DTYPE_DOUBLE || ret->dtype == DTYPE_D) {
            double* ddata = ret->getDoubleArray(len);
            size_t sz = *len * sizeof(float);
            *data = (float*)malloc(sz);
            for (int i = 0; i < *time_len; ++i) {
                (*time)[i] = ddata[i];
            }
            MDSplus::deleteData(ret);
        } else {
            fprintf(stderr, " -> data returned in wrong format\n");
            MDSplus::deleteData(ret);
            return -1;
        }
    } catch (MDSplus::MdsException& ex) {
        fprintf(stderr, " -> unable to get signal\n");
        UDA_LOG(UDA_LOG_ERROR, "Unable to get signal.\n");
        return -1;
    }

    fprintf(stderr, " -> from mdsplus\n");

    ram_cache_add(cache, len_key, len, sizeof(int));
    if (time != nullptr) {
        ram_cache_add(cache, time_len_key, time_len, sizeof(int));
        ram_cache_add(cache, time_key, *time, *time_len * sizeof(float));
    }
    ram_cache_add(cache, data_key, *data, *len * sizeof(float));

    return 0;
}
