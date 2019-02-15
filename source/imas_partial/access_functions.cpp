#include "access_functions.h"

#include <memory>
#include <vector>
#include <boost/algorithm/string.hpp>

#include <ual_backend.h>
#include <mdsobjects.h>

#include "is_integer.h"

namespace {

int read_size_from_backend(LLenv& env, const std::vector<std::string>& tokens, int depth,
                           std::shared_ptr<OperationContext> opCtx, std::shared_ptr<ArraystructContext> arrayCtx)
{
    if (depth == 0) {
        const char* dataObject = tokens[0].c_str();
        int access = ualconst::read_op;
        int range = ualconst::global_op;
        double time = 0.0;
        int interp = ualconst::closest_interp;

        auto pulseCtx = dynamic_cast<PulseContext*>(env.context);

        opCtx = std::make_shared<OperationContext>(*pulseCtx, dataObject, access, range, time, interp);

        env.backend->beginAction(opCtx.get());

        return read_size_from_backend(env, tokens, depth + 1, opCtx, arrayCtx);
    } else {
        if (opCtx == nullptr) {
            throw std::runtime_error("opCtx null");
        }

        std::string path;
        std::string delim;

        while (depth < tokens.size() && !imas_partial::is_integer(tokens[depth])) {
            path += (delim + tokens[depth]);

            depth++;
            delim = "/";
        }

        arrayCtx = std::make_shared<ArraystructContext>(*opCtx, path, "", arrayCtx.get());
        if (depth < tokens.size() && imas_partial::is_integer(tokens[depth])) {
            int index = (int)strtol(tokens[depth].c_str(), nullptr, 10) - 1; // array index is 0-based
            arrayCtx->nextIndex(index - arrayCtx->getIndex());
        }

        int size = 0;
        try {
            env.backend->beginArraystructAction(arrayCtx.get(), &size);
        } catch (UALNoDataException& ex) {
            size = 0;
        } catch (MDSplus::MdsException& ex) {
            size = 0;
        }

        if (depth == tokens.size() || (depth == tokens.size() - 1 && imas_partial::is_integer(tokens[depth]))) {
            ArraystructContext* ctx = arrayCtx.get();
            while (ctx) {
                env.backend->endAction(ctx);
                ctx = ctx->getParent();
            }

            env.backend->endAction(opCtx.get());

            return size;
        }
        return read_size_from_backend(env, tokens, depth + 1, opCtx, arrayCtx);
    }
}

imas_partial::MDSData read_data_from_backend(LLenv& env, const std::vector<std::string>& tokens, int depth,
                                             std::shared_ptr<OperationContext> opCtx,
                                             std::shared_ptr<ArraystructContext> arrayCtx)
{
    if (depth == 0) {
        const char* dataObject = tokens[0].c_str();
        int access = ualconst::read_op;
        int range = ualconst::global_op;
        double time = 0.0;
        int interp = ualconst::closest_interp;

        auto pulseCtx = dynamic_cast<PulseContext*>(env.context);

        opCtx = std::make_shared<OperationContext>(*pulseCtx, dataObject, access, range, time, interp);

        env.backend->beginAction(opCtx.get());

        return read_data_from_backend(env, tokens, depth + 1, opCtx, arrayCtx);
    } else {
        if (opCtx == nullptr) {
            throw std::runtime_error("opCtx null");
        }

        std::string path;
        std::string delim;

        while (!imas_partial::is_integer(tokens[depth])) {
            path += (delim + tokens[depth]);

            depth++;
            delim = "/";

            if (depth == tokens.size()) {
                imas_partial::MDSData data = {};
                const char* field = path.c_str();
                std::string time_path;
                if (path.size() >= 4 && path.substr(path.size() - 4) == "data") {
                    time_path = path;
                    time_path.replace(path.size() - 4, 4, "time");
                }
                const char* timebase = time_path.c_str();

                try {
                    env.backend->readData(arrayCtx.get(), field, timebase, &data.data, &data.datatype, &data.rank,
                                          data.dims);
                } catch (UALNoDataException& ex) {
                    std::cerr << "no data for " << field << std::endl;
                    // pass
                }

                ArraystructContext* ctx = arrayCtx.get();
                while (ctx) {
                    env.backend->endAction(ctx);
                    ctx = ctx->getParent();
                }

                env.backend->endAction(opCtx.get());

                return data;
            }
        }

        arrayCtx = std::make_shared<ArraystructContext>(*opCtx, path, "", arrayCtx.get());
        if (depth < tokens.size() && imas_partial::is_integer(tokens[depth])) {
            int index = (int)strtol(tokens[depth].c_str(), nullptr, 10) - 1; // array index is 0-based
            arrayCtx->nextIndex(index - arrayCtx->getIndex());
        }

        int size = 0;
        try {
            env.backend->beginArraystructAction(arrayCtx.get(), &size);
        } catch (UALNoDataException& ex) {
            size = 0;
        }

        return read_data_from_backend(env, tokens, depth + 1, opCtx, arrayCtx);
    }
}

} // anon namespace

int imas_partial::read_size_from_backend(LLenv& env, const std::string& str)
{
    std::vector<std::string> tokens;
    boost::split(tokens, str, boost::is_any_of("/"), boost::token_compress_on);

    int depth = 0;

    std::shared_ptr<OperationContext> opCtx;
    std::shared_ptr<ArraystructContext> arrayCtx;

    return ::read_size_from_backend(env, tokens, depth, opCtx, arrayCtx);
}

imas_partial::MDSData imas_partial::read_data_from_backend(LLenv& env, const std::string& str)
{
    std::vector<std::string> tokens;
    boost::split(tokens, str, boost::is_any_of("/"), boost::token_compress_on);

    int depth = 0;

    std::shared_ptr<OperationContext> opCtx;
    std::shared_ptr<ArraystructContext> arrayCtx;

    return ::read_data_from_backend(env, tokens, depth, opCtx, arrayCtx);
}
