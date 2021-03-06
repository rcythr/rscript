
#include <rscript/rscript.h>
#include <rscript/constants.h>

using namespace rscript;

const DataType L_VECTOR::type_value = DataType::VECTOR;

PL_ATOM rscript::parseVector(const std::string& input, size_t& offset)
{
    char c;
    EXPECT_CHAR(input.at(offset++), '#');
    char opener = input.at(offset++);
    char closer;
    if(opener == '(' || opener == '[')
    {
        if(opener == '(')
            closer = ')';
        else
            closer = ']';

        std::vector<PL_ATOM> parts;
        bool needsWS = false;

        while(offset < input.size())
        {
            c = input.at(offset);
            if(c == closer)
            {
                ++offset;
                break;
            }
            else if(c == ' ' || c == '\t' || c == '\r' || c == '\n')
            {
                needsWS = false;
                ++offset;
            }
            else
            {
                if(needsWS)
                {
                    throw std::runtime_error(std::string("Unexpected: '")+c+"', Expected some whitespace.");
                }
                else
                {
                    parts.push_back(parseExpression(input, offset));
                    needsWS = true;
                }
            }
        }

        return WRAP(L_VECTOR, std::move(parts));
    }
    throw std::runtime_error(std::string("Unexpected: '")+input[offset-1]+"', Expected '(' or '['");
}

std::string L_VECTOR::str()
{
    std::string output;
    output += "#(";
    std::string sep;
    for(auto& element : mAtoms)
    {
        output += sep;
        output += element->str();
        sep = " ";
    }
    output += ')';

    return output;
}

