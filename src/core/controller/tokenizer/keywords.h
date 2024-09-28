//
// Created by chiendd on 24/09/2024.
//

#ifndef KEYWORDS_H
#define KEYWORDS_H
#include <map>

namespace Csql {
    enum class SqlKeywords {
        alter_kw, and_kw, any_kw, as_kw,
        by_kw, bool_kw,
        case_kw, check_kw, constraint_kw, create_kw,
        default_kw, delete_kw, distinct_kw, drop_kw, datetime_kw,
        else_kw, end_kw, exists_kw,
        foreign_kw, from_kw, float_kw,
        group_kw,
        having_kw,
        if_kw, inner_kw, intersect_kw,
        in_kw, index_kw, insert_kw, into_kw, int_kw,
        join_kw,
        key_kw,
        left_kw, like_kw, limit_kw,
        not_kw, null_kw,
        offset_kw, on_kw, or_kw, order_kw,
        primary_kw,
        references_kw, right_kw, rollback_kw,
        select_kw, set_kw, some_kw,
        table_kw, then_kw, transaction_kw,
        union_kw, unique_kw, update_kw,
        values_kw, view_kw, varchar_kw,
        when_kw, where_kw,
        unkonwn_kw,
    };

    static std::map<std::string, SqlKeywords> mSqlKeywords = {
        std::make_pair("alter", SqlKeywords::alter_kw),
        std::make_pair("and", SqlKeywords::and_kw),
        std::make_pair("any", SqlKeywords::any_kw),
        std::make_pair("as", SqlKeywords::as_kw),
        std::make_pair("by", SqlKeywords::by_kw),
        std::make_pair("bool", SqlKeywords::bool_kw),
        std::make_pair("case", SqlKeywords::case_kw),
        std::make_pair("check", SqlKeywords::check_kw),
        std::make_pair("constraint", SqlKeywords::constraint_kw),
        std::make_pair("create", SqlKeywords::create_kw),
        std::make_pair("default", SqlKeywords::default_kw),
        std::make_pair("delete", SqlKeywords::delete_kw),
        std::make_pair("distinct", SqlKeywords::distinct_kw),
        std::make_pair("drop", SqlKeywords::drop_kw),
        std::make_pair("datetime", SqlKeywords::datetime_kw),
        std::make_pair("else", SqlKeywords::else_kw),
        std::make_pair("end", SqlKeywords::end_kw),
        std::make_pair("exists", SqlKeywords::exists_kw),
        std::make_pair("foreign", SqlKeywords::foreign_kw),
        std::make_pair("from", SqlKeywords::from_kw),
        std::make_pair("float", SqlKeywords::float_kw),
        std::make_pair("group", SqlKeywords::group_kw),
        std::make_pair("having", SqlKeywords::having_kw),
        std::make_pair("if", SqlKeywords::if_kw),
        std::make_pair("inner", SqlKeywords::inner_kw),
        std::make_pair("intersect", SqlKeywords::intersect_kw),
        std::make_pair("in", SqlKeywords::in_kw),
        std::make_pair("index", SqlKeywords::index_kw),
        std::make_pair("insert", SqlKeywords::insert_kw),
        std::make_pair("into", SqlKeywords::into_kw),
        std::make_pair("int", SqlKeywords::int_kw),
        std::make_pair("join", SqlKeywords::join_kw),
        std::make_pair("key", SqlKeywords::key_kw),
        std::make_pair("left", SqlKeywords::left_kw),
        std::make_pair("like", SqlKeywords::like_kw),
        std::make_pair("limit", SqlKeywords::limit_kw),
        std::make_pair("not", SqlKeywords::not_kw),
        std::make_pair("null", SqlKeywords::null_kw),
        std::make_pair("offset", SqlKeywords::offset_kw),
        std::make_pair("on", SqlKeywords::on_kw),
        std::make_pair("or", SqlKeywords::or_kw),
        std::make_pair("order", SqlKeywords::order_kw),
        std::make_pair("primary", SqlKeywords::primary_kw),
        std::make_pair("references", SqlKeywords::references_kw),
        std::make_pair("right", SqlKeywords::right_kw),
        std::make_pair("rollback", SqlKeywords::rollback_kw),
        std::make_pair("select", SqlKeywords::select_kw),
        std::make_pair("set", SqlKeywords::set_kw),
        std::make_pair("some", SqlKeywords::some_kw),
        std::make_pair("table", SqlKeywords::table_kw),
        std::make_pair("then", SqlKeywords::then_kw),
        std::make_pair("transaction", SqlKeywords::transaction_kw),
        std::make_pair("union", SqlKeywords::union_kw),
        std::make_pair("unique", SqlKeywords::unique_kw),
        std::make_pair("update", SqlKeywords::update_kw),
        std::make_pair("values", SqlKeywords::values_kw),
        std::make_pair("view", SqlKeywords::view_kw),
        std::make_pair("varchar", SqlKeywords::varchar_kw),
        std::make_pair("when", SqlKeywords::when_kw),
        std::make_pair("where", SqlKeywords::where_kw),
        std::make_pair("unknown", SqlKeywords::unkonwn_kw)
    };

    enum class SqlOperators {
        add_op, and_op, between_op, divide_op, dot_op,
        equal_op, gt_op, gte_op, lt_op, lte_op,
        mod_op, multiply_op, nor_op, not_op, not_equal_op,
        or_op, power_op, subtract_op, unknown_op
    };

    static std::map<std::string, SqlOperators> mSqlOperators {
        std::make_pair("+", SqlOperators::add_op),
        std::make_pair("and", SqlOperators::and_op),
        std::make_pair("between", SqlOperators::between_op),
        std::make_pair("/", SqlOperators::divide_op),
        std::make_pair(".", SqlOperators::dot_op),
        std::make_pair("=", SqlOperators::equal_op),
        std::make_pair(">", SqlOperators::gt_op),
        std::make_pair(">=", SqlOperators::gte_op),
        std::make_pair("<", SqlOperators::lt_op),
        std::make_pair("<=", SqlOperators::lte_op),
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
}

#endif //KEYWORDS_H
