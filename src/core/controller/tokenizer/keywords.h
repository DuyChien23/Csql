//
// Created by chiendd on 24/09/2024.
//

#ifndef KEYWORDS_H
#define KEYWORDS_H
#include <map>

enum class SqlKeywords {
    alter_kw, any_kw, as_kw, auto_increment, asc_kw, abort_kw,
    by_kw, bool_kw, begin_kw,
    case_kw, check_kw, constraint_kw, create_kw, cross_kw, commit_kw,
    default_kw, delete_kw, distinct_kw, drop_kw, date_kw, database_kw, describe_kw, databases_kw, desc_kw,
    else_kw, end_kw, exists_kw,
    foreign_kw, from_kw, float_kw, false_kw, full_kw,
    group_kw,
    having_kw,
    if_kw, inner_kw, intersect_kw,
    index_kw, insert_kw, into_kw, int_kw,
    join_kw,
    key_kw,
    left_kw, limit_kw,
    null_kw,
    offset_kw, on_kw, order_kw,
    primary_kw,
    references_kw, right_kw, rollback_kw,
    select_kw, set_kw, some_kw, show_kw,
    table_kw, then_kw, transaction_kw, tables_kw, true_kw,
    union_kw, unique_kw, update_kw,
    values_kw, view_kw, varchar_kw,
    when_kw, where_kw,
    unkonwn_kw, use_kw,
};

static std::map<std::string, SqlKeywords> mSqlKeywords = {
    std::make_pair("alter", SqlKeywords::alter_kw),
    // std::make_pair("and", SqlKeywords::and_kw),
    std::make_pair("any", SqlKeywords::any_kw),
    std::make_pair("as", SqlKeywords::as_kw),
    std::make_pair("auto_increment", SqlKeywords::auto_increment),
    std::make_pair("asc", SqlKeywords::asc_kw),
    std::make_pair("abort", SqlKeywords::abort_kw),
    std::make_pair("by", SqlKeywords::by_kw),
    std::make_pair("bool", SqlKeywords::bool_kw),
    // std::make_pair("between", SqlKeywords::between_kw),
    std::make_pair("begin", SqlKeywords::begin_kw),
    std::make_pair("case", SqlKeywords::case_kw),
    std::make_pair("check", SqlKeywords::check_kw),
    std::make_pair("constraint", SqlKeywords::constraint_kw),
    std::make_pair("create", SqlKeywords::create_kw),
    std::make_pair("cross", SqlKeywords::cross_kw),
    std::make_pair("commit", SqlKeywords::commit_kw),
    std::make_pair("default", SqlKeywords::default_kw),
    std::make_pair("delete", SqlKeywords::delete_kw),
    std::make_pair("distinct", SqlKeywords::distinct_kw),
    std::make_pair("drop", SqlKeywords::drop_kw),
    std::make_pair("date", SqlKeywords::date_kw),
    std::make_pair("database", SqlKeywords::database_kw),
    std::make_pair("describe", SqlKeywords::describe_kw),
    std::make_pair("databases", SqlKeywords::databases_kw),
    std::make_pair("desc", SqlKeywords::desc_kw),
    std::make_pair("else", SqlKeywords::else_kw),
    std::make_pair("end", SqlKeywords::end_kw),
    std::make_pair("exists", SqlKeywords::exists_kw),
    std::make_pair("foreign", SqlKeywords::foreign_kw),
    std::make_pair("from", SqlKeywords::from_kw),
    std::make_pair("float", SqlKeywords::float_kw),
    std::make_pair("false", SqlKeywords::false_kw),
    std::make_pair("full", SqlKeywords::full_kw),
    std::make_pair("group", SqlKeywords::group_kw),
    std::make_pair("having", SqlKeywords::having_kw),
    std::make_pair("if", SqlKeywords::if_kw),
    std::make_pair("inner", SqlKeywords::inner_kw),
    std::make_pair("intersect", SqlKeywords::intersect_kw),
    // std::make_pair("in", SqlKeywords::in_kw),
    std::make_pair("index", SqlKeywords::index_kw),
    std::make_pair("insert", SqlKeywords::insert_kw),
    std::make_pair("into", SqlKeywords::into_kw),
    std::make_pair("int", SqlKeywords::int_kw),
    std::make_pair("join", SqlKeywords::join_kw),
    std::make_pair("key", SqlKeywords::key_kw),
    std::make_pair("left", SqlKeywords::left_kw),
    // std::make_pair("like", SqlKeywords::like_kw),
    std::make_pair("limit", SqlKeywords::limit_kw),
    // std::make_pair("not", SqlKeywords::not_kw),
    std::make_pair("null", SqlKeywords::null_kw),
    std::make_pair("offset", SqlKeywords::offset_kw),
    std::make_pair("on", SqlKeywords::on_kw),
    // std::make_pair("or", SqlKeywords::or_kw),
    std::make_pair("order", SqlKeywords::order_kw),
    std::make_pair("primary", SqlKeywords::primary_kw),
    std::make_pair("references", SqlKeywords::references_kw),
    std::make_pair("right", SqlKeywords::right_kw),
    std::make_pair("rollback", SqlKeywords::rollback_kw),
    std::make_pair("select", SqlKeywords::select_kw),
    std::make_pair("set", SqlKeywords::set_kw),
    std::make_pair("some", SqlKeywords::some_kw),
    std::make_pair("show", SqlKeywords::show_kw),
    std::make_pair("table", SqlKeywords::table_kw),
    std::make_pair("then", SqlKeywords::then_kw),
    std::make_pair("transaction", SqlKeywords::transaction_kw),
    std::make_pair("tables", SqlKeywords::tables_kw),
    std::make_pair("true", SqlKeywords::true_kw),
    std::make_pair("union", SqlKeywords::union_kw),
    std::make_pair("unique", SqlKeywords::unique_kw),
    std::make_pair("update", SqlKeywords::update_kw),
    std::make_pair("values", SqlKeywords::values_kw),
    std::make_pair("view", SqlKeywords::view_kw),
    std::make_pair("varchar", SqlKeywords::varchar_kw),
    std::make_pair("when", SqlKeywords::when_kw),
    std::make_pair("where", SqlKeywords::where_kw),
    std::make_pair("unknown", SqlKeywords::unkonwn_kw),
    std::make_pair("use", SqlKeywords::use_kw)
};

enum class SqlFunctions {
    // normal function
    ascii_ft,
    concat_ft, concat_ws_ft, char_length_ft, char_ft,
    length_ft, left_ft, locate_ft, lower_ft,
    substring_ft,
    replace_ft, right_ft, reverse_ft,
    insert_ft,
    trim_ft, upper_ft,

    // aggregate function
    count_ft, sum_ft, avg_ft, min_ft, max_ft, group_concat_ft,

    //unknown function
    unknown_ft,
};

static std::map<std::string, SqlFunctions> mSqlFunctions{
    std::make_pair("ascii", SqlFunctions::ascii_ft),
    std::make_pair("concat", SqlFunctions::concat_ft),
    std::make_pair("concat_ws", SqlFunctions::concat_ws_ft),
    std::make_pair("char_length", SqlFunctions::char_length_ft),
    std::make_pair("char", SqlFunctions::char_ft),
    std::make_pair("length", SqlFunctions::length_ft),
    std::make_pair("left", SqlFunctions::left_ft),
    std::make_pair("locate", SqlFunctions::locate_ft),
    std::make_pair("lower", SqlFunctions::lower_ft),
    std::make_pair("substring", SqlFunctions::substring_ft),
    std::make_pair("substr", SqlFunctions::substring_ft),
    std::make_pair("replace", SqlFunctions::replace_ft),
    std::make_pair("right", SqlFunctions::right_ft),
    std::make_pair("reverse", SqlFunctions::reverse_ft),
    std::make_pair("insert", SqlFunctions::insert_ft),
    std::make_pair("trim", SqlFunctions::trim_ft),
    std::make_pair("upper", SqlFunctions::upper_ft),
};

static std::map<std::string, SqlFunctions> mSqlAggregateFunctions{
    std::make_pair("count", SqlFunctions::count_ft),
    std::make_pair("sum", SqlFunctions::sum_ft),
    std::make_pair("avg", SqlFunctions::avg_ft),
    std::make_pair("min", SqlFunctions::min_ft),
    std::make_pair("max", SqlFunctions::max_ft),
    std::make_pair("group_concat", SqlFunctions::group_concat_ft),
};

enum class SqlOperators {
    add_op, and_op, between_op, divide_op, dot_op,
    equal_op, gt_op, gte_op, in_op, lt_op, lte_op, like_op,
    mod_op, multiply_op, nor_op, not_op, not_equal_op,
    not_between_op, not_in_op, not_like_op,
    or_op, power_op, subtract_op, unknown_op,

    // others
    open_parenthesis, close_parenthesis,
};

static std::map<std::string, SqlOperators> mSqlOperators{
    std::make_pair("+", SqlOperators::add_op),
    std::make_pair("and", SqlOperators::and_op),
    std::make_pair("between", SqlOperators::between_op),
    std::make_pair("/", SqlOperators::divide_op),
    std::make_pair(".", SqlOperators::dot_op),
    std::make_pair("=", SqlOperators::equal_op),
    std::make_pair(">", SqlOperators::gt_op),
    std::make_pair(">=", SqlOperators::gte_op),
    std::make_pair("in", SqlOperators::in_op),
    std::make_pair("<", SqlOperators::lt_op),
    std::make_pair("<=", SqlOperators::lte_op),
    std::make_pair("like", SqlOperators::like_op),
    std::make_pair("%", SqlOperators::mod_op),
    std::make_pair("*", SqlOperators::multiply_op),
    std::make_pair("nor", SqlOperators::nor_op),
    std::make_pair("not", SqlOperators::not_op),
    std::make_pair("<>", SqlOperators::not_equal_op),
    std::make_pair("or", SqlOperators::or_op),
    std::make_pair("^", SqlOperators::power_op),
    std::make_pair("-", SqlOperators::subtract_op),
    std::make_pair("unknown", SqlOperators::unknown_op)
};


#endif //KEYWORDS_H
