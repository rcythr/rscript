
#include <rscript/builtin_handler.h>

#include <sstream>

using namespace rscript;

BuiltinHandler::BuiltinHandler(std::string name) 
    : name(name)
    , default_handler(nullptr) 
{
}

BuiltinHandler::BuiltinHandler(std::string name, std::function<PL_ATOM(std::vector<PL_ATOM>&, SymbolTable&)> func) 
    : name(name)
    , default_handler(func) 
{
}

BuiltinHandler::~BuiltinHandler()
{
    for(auto& binding : handlers)
    {
        delete binding.second;
    }
}

PL_ATOM BuiltinHandler::handle(std::vector<PL_ATOM>& args, SymbolTable& symbols)
{
    std::vector<DataType> key;
    for(size_t i=0; i < args.size(); ++i)
    {
        key.push_back(args[i]->mType);
    }

    for(auto& p : handlers)
    {
        if(key.size() == p.first.size())
        {
            bool invalid = false;
            for(size_t i=0; i < key.size(); ++i)
            {
                if(p.first[i] != DataType::ATOM && p.first[i] != key[i])
                {
                    invalid = true;
                    break;
                }
            }
            if(!invalid)
                return p.second->handle(args, symbols);
        }
    }

    if(default_handler != nullptr)
    {
        return default_handler(args, symbols);
    }

    std::stringstream input_args;
    for(DataType d : key)
    {
        input_args << TYPE_NAME_LOOKUP[(int)d];
        input_args << " ";
    }

    throw std::runtime_error(name + " has no overload which takes arguments ( " + input_args.str() + "). ");

    return nullptr;
}
