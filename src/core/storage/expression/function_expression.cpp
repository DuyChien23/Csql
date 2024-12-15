//
// Created by chiendd on 22/12/2024.
//

#include "function_expression.h"

#include "../../util/helpers.h"

void expectParam(const SqlTypes& value, DataTypes type) {
    if (Helpers::SqlTypeHandle::covertSqlValueToDataTypes(value) != type) {
        throw Errors("Invalid type of parameter");
    }
}

SqlTypes ascii(const std::vector<SqlTypes> &parameters) {
    if (parameters.size() != 1) {
        throw Errors("Invalid number of parameters");
    }

    auto value = parameters[0];
    expectParam(value, DataTypes::int_type);

    return value;
}

SqlTypes concat(const std::vector<SqlTypes> &parameters) {
    if (parameters.size() < 2) {
        throw Errors("Invalid number of parameters");
    }

    std::string result = "";
    for (auto param : parameters) {
        auto value = param;
        result += Helpers::SqlTypeHandle::covertSqlTypeToString(value);
    }

    return result;
}

SqlTypes concat_ws(const std::vector<SqlTypes> &parameters) {
    if (parameters.size() < 3) {
        throw Errors("Invalid number of parameters");
    }

    auto separator = parameters[0];
    expectParam(separator, DataTypes::varchar_type);

    std::string result = "";
    for (int i = 1; i < parameters.size(); i++) {
        auto value = parameters[i];
        result += Helpers::SqlTypeHandle::covertSqlTypeToString(value);
        if (i != parameters.size() - 1) {
            result += Helpers::SqlTypeHandle::covertSqlTypeToString(separator);
        }
    }

    return result;
}

SqlTypes charft(const std::vector<SqlTypes> &parameters) {
    if (parameters.size() != 1) {
        throw Errors("Invalid number of parameters");
    }

    auto value = parameters[0];
    expectParam(value, DataTypes::int_type);

    return std::string(1, (char) std::get<SqlIntType>(value));
}

SqlTypes char_length(const std::vector<SqlTypes> &parameters) {
    if (parameters.size() != 1) {
        throw Errors("Invalid number of parameters");
    }

    auto value = parameters[0];
    expectParam(value, DataTypes::varchar_type);

    return std::get<SqlVarcharType>(value).size();
}

SqlTypes length(const std::vector<SqlTypes> &parameters) {
    if (parameters.size() != 1) {
        throw Errors("Invalid number of parameters");
    }

    auto value = parameters[0];
    expectParam(value, DataTypes::varchar_type);

    return std::get<SqlVarcharType>(value).size();
}

SqlTypes left(const std::vector<SqlTypes> &parameters) {
    if (parameters.size() != 2) {
        throw Errors("Invalid number of parameters");
    }

    auto value = parameters[0];
    expectParam(value, DataTypes::varchar_type);

    auto length = parameters[1];
    expectParam(length, DataTypes::int_type);

    return std::get<SqlVarcharType>(value).substr(0, std::get<SqlIntType>(length));
}

SqlTypes locate(const std::vector<SqlTypes> &parameters) {
    if (parameters.size() != 2) {
        throw Errors("Invalid number of parameters");
    }

    auto substring = parameters[0];
    expectParam(substring, DataTypes::varchar_type);

    auto value = parameters[1];
    expectParam(value, DataTypes::varchar_type);

    auto pos = std::get<SqlVarcharType>(value).find(std::get<SqlVarcharType>(substring));
    if (pos == std::string::npos) {
        return SqlIntType(0);
    }

    return pos + 1;
}

SqlTypes lower(const std::vector<SqlTypes> &parameters) {
    if (parameters.size() != 1) {
        throw Errors("Invalid number of parameters");
    }

    auto value = parameters[0];
    expectParam(value, DataTypes::varchar_type);

    std::string result = std::get<SqlVarcharType>(value);
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);

    return result;
}

SqlTypes substring(const std::vector<SqlTypes> &parameters) {
    if (parameters.size() != 3) {
        throw Errors("Invalid number of parameters");
    }

    auto value = parameters[0];
    expectParam(value, DataTypes::varchar_type);

    auto start = parameters[1];
    expectParam(start, DataTypes::int_type);

    auto length = parameters[2];
    expectParam(length, DataTypes::int_type);

    return std::get<SqlVarcharType>(value).substr(std::get<SqlIntType>(start) - 1, std::get<SqlIntType>(length));
}

SqlTypes replace(const std::vector<SqlTypes> &parameters) {
    if (parameters.size() != 3) {
        throw Errors("Invalid number of parameters");
    }

    auto value = parameters[0];
    expectParam(value, DataTypes::varchar_type);

    auto from = parameters[1];
    expectParam(from, DataTypes::varchar_type);

    auto to = parameters[2];
    expectParam(to, DataTypes::varchar_type);

    std::string result = std::get<SqlVarcharType>(value);
    size_t start_pos = 0;
    while ((start_pos = result.find(std::get<SqlVarcharType>(from), start_pos)) != std::string::npos) {
        result.replace(start_pos, std::get<SqlVarcharType>(from).length(), std::get<SqlVarcharType>(to));
        start_pos += std::get<SqlVarcharType>(to).length();
    }

    return result;
}

SqlTypes right(const std::vector<SqlTypes> &parameters) {
    if (parameters.size() != 2) {
        throw Errors("Invalid number of parameters");
    }

    auto value = parameters[0];
    expectParam(value, DataTypes::varchar_type);

    auto length = parameters[1];
    expectParam(length, DataTypes::int_type);

    return std::get<SqlVarcharType>(value).substr(std::get<SqlVarcharType>(value).size() - std::get<SqlIntType>(length));
}

SqlTypes reverse(const std::vector<SqlTypes> &parameters) {
    if (parameters.size() != 1) {
        throw Errors("Invalid number of parameters");
    }

    auto value = parameters[0];
    expectParam(value, DataTypes::varchar_type);

    std::string result = std::get<SqlVarcharType>(value);
    std::reverse(result.begin(), result.end());

    return result;
}

SqlTypes trim(const std::vector<SqlTypes> &parameters) {
    if (parameters.size() != 1) {
        throw Errors("Invalid number of parameters");
    }

    auto value = parameters[0];
    expectParam(value, DataTypes::varchar_type);

    std::string result = std::get<SqlVarcharType>(value);
    result.erase(0, result.find_first_not_of(' '));
    result.erase(result.find_last_not_of(' ') + 1);

    return result;
}

SqlTypes upper(const std::vector<SqlTypes> &parameters) {
    if (parameters.size() != 1) {
        throw Errors("Invalid number of parameters");
    }

    auto value = parameters[0];
    expectParam(value, DataTypes::varchar_type);

    std::string result = std::get<SqlVarcharType>(value);
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);

    return result;
}

FunctionExpression::FunctionExpression(FunctionTypes aType, std::vector<Expression *> &theParameters) {
    type = aType;
    parameters = theParameters;
}

SqlTypes FunctionExpression::apply(const JoinedTuple &tuple) {
    std::vector<SqlTypes> temp;
    for (auto param : parameters) {
        temp.push_back(param->apply(tuple));
    }

    switch (type) {
        case FunctionTypes::ascii_ft:
            return ascii(temp);
        case FunctionTypes::concat_ft:
            return concat(temp);
        case FunctionTypes::concat_ws_ft:
            return concat_ws(temp);
        case FunctionTypes::char_ft:
            return charft(temp);
        case FunctionTypes::char_length_ft:
            return char_length(temp);
        case FunctionTypes::length_ft:
            return length(temp);
        case FunctionTypes::left_ft:
            return left(temp);
        case FunctionTypes::locate_ft:
            return locate(temp);
        case FunctionTypes::lower_ft:
            return lower(temp);
        case FunctionTypes::substring_ft:
            return substring(temp);
        case FunctionTypes::replace_ft:
            return replace(temp);
        case FunctionTypes::right_ft:
            return right(temp);
        case FunctionTypes::reverse_ft:
            return reverse(temp);
        case FunctionTypes::trim_ft:
            return trim(temp);
        case FunctionTypes::upper_ft:
            return upper(temp);
        default:
            throw Errors("No case");
    }
}



