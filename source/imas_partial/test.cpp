#if 0
g++ -O0 -g $(pkg-config --cflags imas-cpp) $0 -o test.x $(pkg-config --libs imas-cpp)
exit $?
#endif

#include <string>
#include <vector>
#include <deque>
#include <cassert>
#include <boost/algorithm/string.hpp>

#include <ual_backend.h>
#include <ual_lowlevel.h>

int readSizeFromBackend(LLenv& env, const std::vector<std::string>& tokens, int depth, std::deque<int>& indices, OperationContext* opCtx, ArraystructContext* arrayCtx)
{
    if (depth == 0) {
        const char* dataObject = tokens[0].c_str();
        int access = ualconst::read_op;
        int range = ualconst::global_op;
        double time = 0.0;
        int interp = ualconst::closest_interp;

        PulseContext* pulseCtx = dynamic_cast<PulseContext*>(env.context);

        opCtx = new OperationContext(*pulseCtx, dataObject, access, range, time, interp);

        env.backend->beginAction(opCtx);

        return readSizeFromBackend(env, tokens, depth + 1, indices, opCtx, arrayCtx);
    } else {
        assert(opCtx != nullptr);

        std::string path = "";
        std::string delim = "";

        while (depth < tokens.size() && tokens[depth] != "#") {
            path += (delim + tokens[depth]);

            depth++;
            delim = "/";
        }

        arrayCtx = new ArraystructContext(*opCtx, path, "", arrayCtx);
        int index = indices.front();
        indices.pop_front();
        arrayCtx->nextIndex(index - arrayCtx->getIndex());

        int size = 0;
        env.backend->beginArraystructAction(arrayCtx, &size);

        assert(size == 10);

        if (depth == tokens.size()) {
            return size;
        }
        return readSizeFromBackend(env, tokens, depth + 1, indices, opCtx, arrayCtx);
    }
}

int readSizeFromBackend(LLenv& env, const std::string& str, std::deque<int> indices)
{
    std::vector<std::string> tokens;
    boost::split(tokens, str, boost::is_any_of("/"), boost::token_compress_on);

    int depth = 0;

    return readSizeFromBackend(env, tokens, depth, indices, nullptr, nullptr);
}

void readDataFromBackend(LLenv& env, const std::vector<std::string>& tokens, int depth, std::deque<int>& indices, OperationContext* opCtx, ArraystructContext* arrayCtx)
{
    if (depth == 0) {
        const char* dataObject = tokens[0].c_str();
        int access = ualconst::read_op;
        int range = ualconst::global_op;
        double time = 0.0;
        int interp = ualconst::closest_interp;

        PulseContext* pulseCtx = dynamic_cast<PulseContext*>(env.context);

        opCtx = new OperationContext(*pulseCtx, dataObject, access, range, time, interp);

        env.backend->beginAction(opCtx);

        return readDataFromBackend(env, tokens, depth + 1, indices, opCtx, arrayCtx);
    } else {
        assert(opCtx != nullptr);

        std::string path = "";
        std::string delim = "";

        while (tokens[depth] != "#") {
            path += (delim + tokens[depth]);

            depth++;
            delim = "/";

            if (depth == tokens.size()) {
                int rank = 0;
                int dims[64] = {0};
                const char* field = path.c_str();
                std::string time_path = "";
                if (path.substr(path.size() - 4) == "data") {
                    time_path = path;
                    time_path.replace(path.size() - 4, 4, "time");
                }
                const char* timebase = time_path.c_str();
                void* data = nullptr;
                int datatype;

                env.backend->readData(arrayCtx, field, timebase, &data, &datatype, &rank, dims);

                assert(rank == 1);
                assert(dims[0] == 1000);
                assert(((double*)data)[0] == -10.0);
                assert(((double*)data)[999] == 10.0);

                return;
            }
        }

        arrayCtx = new ArraystructContext(*opCtx, path, "", arrayCtx);
        int index = indices.front();
        indices.pop_front();
        arrayCtx->nextIndex(index - arrayCtx->getIndex());

        int size = 0;
        env.backend->beginArraystructAction(arrayCtx, &size);

        assert(size == 10);

        return readDataFromBackend(env, tokens, depth + 1, indices, opCtx, arrayCtx);
    }
}

void readDataFromBackend(LLenv& env, const std::string& str, std::deque<int> indices)
{
    std::vector<std::string> tokens;
    boost::split(tokens, str, boost::is_any_of("/"), boost::token_compress_on);

    int depth = 0;

    return readDataFromBackend(env, tokens, depth, indices, nullptr, nullptr);
}

int main()
{
    int backendId = ualconst::mdsplus_backend;
    int shot = 1000;
    int run = 0;
    const char* user = "g2jhollo";
    const char* tokamak = "test";
    const char* version = "3.21.1";
    
    int ctxId = Lowlevel::beginPulseAction(backendId, shot, run, user, tokamak, version);
    LLenv env = Lowlevel::getLLenv(ctxId);

    int mode = ualconst::open_pulse;
    const char* options = "";

    PulseContext* pulseCtx = dynamic_cast<PulseContext*>(env.context);

    env.backend->openPulse(pulseCtx, mode, options);

    std::deque<int> indices;
    int size = readSizeFromBackend(env, "magnetics/flux_loop", indices);

    assert(size == 10);

    std::string test1 = "magnetics/flux_loop/#/flux/data";
    indices.push_front(3);

    readDataFromBackend(env, test1, indices);
    
    return 0;
}
