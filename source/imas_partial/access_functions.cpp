#include "access_functions.h"

#include <memory>
#include <vector>
#include <boost/algorithm/string.hpp>

#include <ual_backend.h>


namespace {

int read_size_from_backend(LLenv& env, const std::vector<std::string>& tokens, int depth, std::deque<int>& indices, std::shared_ptr<OperationContext> opCtx, std::shared_ptr<ArraystructContext> arrayCtx)
{
    if (depth == 0) {
        const char* dataObject = tokens[0].c_str();
        int access = ualconst::read_op;
        int range = ualconst::global_op;
        double time = 0.0;
        int interp = ualconst::closest_interp;

        PulseContext* pulseCtx = dynamic_cast<PulseContext*>(env.context);

        opCtx = std::make_shared<OperationContext>(*pulseCtx, dataObject, access, range, time, interp);

        env.backend->beginAction(opCtx.get());

        return read_size_from_backend(env, tokens, depth + 1, indices, opCtx, arrayCtx);
    } else {
        if (opCtx == nullptr) {
            throw std::runtime_error("opCtx null");
        }

        std::string path = "";
        std::string delim = "";

        while (depth < tokens.size() && tokens[depth] != "#") {
            path += (delim + tokens[depth]);

            depth++;
            delim = "/";
        }

        arrayCtx = std::make_shared<ArraystructContext>(*opCtx, path, "", arrayCtx.get());
        int index = indices.front();
        indices.pop_front();
        arrayCtx->nextIndex(index - arrayCtx->getIndex());

        int size = 0;
        env.backend->beginArraystructAction(arrayCtx.get(), &size);

        if (depth == tokens.size()) {
            return size;
        }
        return read_size_from_backend(env, tokens, depth + 1, indices, opCtx, arrayCtx);
    }
}

void* read_data_from_backend(LLenv& env, const std::vector<std::string>& tokens, int depth, std::deque<int>& indices, std::shared_ptr<OperationContext> opCtx, std::shared_ptr<ArraystructContext> arrayCtx)
{
    if (depth == 0) {
        const char* dataObject = tokens[0].c_str();
        int access = ualconst::read_op;
        int range = ualconst::global_op;
        double time = 0.0;
        int interp = ualconst::closest_interp;

        PulseContext* pulseCtx = dynamic_cast<PulseContext*>(env.context);

        opCtx = std::make_shared<OperationContext>(*pulseCtx, dataObject, access, range, time, interp);

        env.backend->beginAction(opCtx.get());

        return read_data_from_backend(env, tokens, depth + 1, indices, opCtx, arrayCtx);
    } else {
        if (opCtx == nullptr) {
            throw std::runtime_error("opCtx null");
        }

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

                env.backend->readData(arrayCtx.get(), field, timebase, &data, &datatype, &rank, dims);

                return data;
            }
        }

        arrayCtx = std::make_shared<ArraystructContext>(*opCtx, path, "", arrayCtx.get());
        int index = indices.front();
        indices.pop_front();
        arrayCtx->nextIndex(index - arrayCtx->getIndex());

        int size = 0;
        env.backend->beginArraystructAction(arrayCtx.get(), &size);

        return read_data_from_backend(env, tokens, depth + 1, indices, opCtx, arrayCtx);
    }
}

} // anon namespace

int imas_partial::read_size_from_backend(LLenv& env, const std::string& str, std::deque<int> indices)
{
    std::vector<std::string> tokens;
    boost::split(tokens, str, boost::is_any_of("/"), boost::token_compress_on);

    int depth = 0;

    std::shared_ptr<OperationContext> opCtx;
    std::shared_ptr<ArraystructContext> arrayCtx;

    return ::read_size_from_backend(env, tokens, depth, indices, opCtx, arrayCtx);
}

void* imas_partial::read_data_from_backend(LLenv& env, const std::string& str, std::deque<int> indices)
{
    std::vector<std::string> tokens;
    boost::split(tokens, str, boost::is_any_of("/"), boost::token_compress_on);

    int depth = 0;

    std::shared_ptr<OperationContext> opCtx;
    std::shared_ptr<ArraystructContext> arrayCtx;

    return ::read_data_from_backend(env, tokens, depth, indices, opCtx, arrayCtx);
}
