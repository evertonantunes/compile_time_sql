#include "api.hpp"
#include "sqlite_factory.hpp"
#include <iostream>

namespace database
{
    using factory_t = factory::SQLite;

    namespace tables
    {
        using namespace sql;

        struct DataSet : public Table<decltype("T_dataset"_s)>
        {
            static constexpr Column<DataSet, decltype("id"_s)          , std::ptrdiff_t  , Flags<pk, not_null>> id             = {};
            static constexpr Column<DataSet, decltype("date"_s)        , std::ptrdiff_t  , Flags<not_null>> date               = {};

            using columns = std::tuple<decltype(id), decltype(date)>; // future introspection
        };

        struct Tag : public Table<decltype("T_tag"_s)>
        {
            static constexpr Column<Tag, decltype("id"_s)   , std::ptrdiff_t  , Flags<pk, not_null>> id    = {};
            static constexpr Column<Tag, decltype("value"_s), std::ptrdiff_t  , Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(value)>; // future introspection
        };

        struct AE : public Table<decltype("T_AE"_s)>
        {
            static constexpr Column<AE, decltype("id"_s)     , std::ptrdiff_t, Flags<pk, not_null>                       > id      = {};
            static constexpr Column<AE, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<AE, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<AE, decltype("value"_s)  , std::ptrdiff_t, Flags<not_null>                           > value   = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct AS: public Table<decltype("T_AS"_s)>
        {
            static constexpr Column<AS, decltype("id"_s)     , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<AS, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<AS, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<AS, decltype("value"_s)  , std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct AT: public Table<decltype("T_AT"_s)>
        {
            static constexpr Column<AT, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<AT, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<AT, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<AT, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct OB: public Table<decltype("T_OB"_s)>
        {
            static constexpr Column<OB, decltype("id"_s)     , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<OB, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<OB, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<OB, decltype("value"_s)  , std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct OW: public Table<decltype("T_OW"_s)>
        {
            static constexpr Column<OW, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<OW, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<OW, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<OW, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct OF: public Table<decltype("T_OF"_s)>
        {
            static constexpr Column<OF, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<OF, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<OF, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<OF, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct OD: public Table<decltype("T_OD"_s)>
        {
            static constexpr Column<OD, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<OD, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<OD, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<OD, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct SQ: public Table<decltype("T_SQ"_s)>
        {
            static constexpr Column<SQ, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<SQ, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<SQ, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<SQ, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct UT: public Table<decltype("T_UT"_s)>
        {
            static constexpr Column<UT, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<UT, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<UT, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<UT, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct UN: public Table<decltype("T_UN"_s)>
        {
            static constexpr Column<UN, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<UN, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<UN, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<UN, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct UL: public Table<decltype("T_UL"_s)>
        {
            static constexpr Column<UL, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<UL, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<UL, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<UL, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct US: public Table<decltype("T_US"_s)>
        {
            static constexpr Column<US, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<US, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<US, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<US, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct SS: public Table<decltype("T_SS"_s)>
        {
            static constexpr Column<SS, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<SS, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<SS, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<SS, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct SL: public Table<decltype("T_SL"_s)>
        {
            static constexpr Column<SL, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<SL, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<SL, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<SL, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct FD: public Table<decltype("T_FD"_s)>
        {
            static constexpr Column<FD, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<FD, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<FD, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<FD, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct FL: public Table<decltype("T_FL"_s)>
        {
            static constexpr Column<FL, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<FL, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<FL, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<FL, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct DA: public Table<decltype("T_DA"_s)>
        {
            static constexpr Column<DA, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<DA, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<DA, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<DA, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct UI: public Table<decltype("T_UI"_s)>
        {
            static constexpr Column<UI, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<UI, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<UI, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<UI, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct SH: public Table<decltype("T_SH"_s)>
        {
            static constexpr Column<SH, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<SH, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<SH, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<SH, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct TM: public Table<decltype("T_TM"_s)>
        {
            static constexpr Column<TM, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<TM, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<TM, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<TM, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct UC: public Table<decltype("T_UC"_s)>
        {
            static constexpr Column<UC, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<UC, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<UC, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<UC, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct UR: public Table<decltype("T_UR"_s)>
        {
            static constexpr Column<UR, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<UR, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<UR, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<UR, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct DT: public Table<decltype("T_DT"_s)>
        {
            static constexpr Column<DT, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<DT, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<DT, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<DT, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct DS: public Table<decltype("T_DS"_s)>
        {
            static constexpr Column<DS, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<DS, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<DS, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<DS, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct CS: public Table<decltype("T_CS"_s)>
        {
            static constexpr Column<CS, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<CS, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<CS, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<CS, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct IS: public Table<decltype("T_IS"_s)>
        {
            static constexpr Column<IS, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<IS, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<IS, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<IS, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct LT: public Table<decltype("T_LT"_s)>
        {
            static constexpr Column<LT, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<LT, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<LT, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<LT, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct OL: public Table<decltype("T_OL"_s)>
        {
            static constexpr Column<OL, decltype("id"_s)     , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<OL, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<OL, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<OL, decltype("value"_s)  , std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct PN: public Table<decltype("T_PN"_s)>
        {
            static constexpr Column<PN, decltype("id"_s)     , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<PN, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<PN, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<PN, decltype("value"_s)  , std::string_view, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct ST: public Table<decltype("T_ST"_s)>
        {
            static constexpr Column<AE, decltype("id"_s)     , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<AE, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<AE, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<AE, decltype("value"_s)  , std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct LO: public Table<decltype("T_LO"_s)>
        {
            static constexpr Column<LO, decltype("id"_s)     , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<LO, decltype("tag"_s)    , std::ptrdiff_t, Flags<fk<decltype(Tag::id)>, not_null>    > tag     = {};
            static constexpr Column<LO, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<LO, decltype("value"_s)  , std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };


    }

    template<typename ...T>
    auto select( T &&...args )
    {
        return sql::select<factory_t>(std::forward<T>(args)...);
    }

    template<typename H, typename ...T>
    auto insert_into( T &&...args )
    {
        return sql::insert_into<factory_t, H>(std::forward<T>(args)...);
    }

    template<typename T>
    void create_table()
    {
        sql::create_table<factory_t, T>();
    }
}

int main()
{
    using namespace database::tables;

    database::create_table<database::tables::DataSet>();
    database::create_table<database::tables::Tag>();
    database::create_table<database::tables::AE>();
    database::create_table<database::tables::AS>();
    database::create_table<database::tables::AT>();
    database::create_table<database::tables::OB>();
    database::create_table<database::tables::OW>();
    database::create_table<database::tables::OF>();
    database::create_table<database::tables::OD>();
    database::create_table<database::tables::SQ>();
    database::create_table<database::tables::UT>();
    database::create_table<database::tables::UN>();
    database::create_table<database::tables::UL>();
    database::create_table<database::tables::US>();
    database::create_table<database::tables::SS>();
    database::create_table<database::tables::SL>();
    database::create_table<database::tables::FD>();
    database::create_table<database::tables::FL>();
    database::create_table<database::tables::DA>();
    database::create_table<database::tables::UI>();
    database::create_table<database::tables::SH>();
    database::create_table<database::tables::TM>();
    database::create_table<database::tables::UC>();
    database::create_table<database::tables::UR>();
    database::create_table<database::tables::DT>();
    database::create_table<database::tables::DS>();
    database::create_table<database::tables::CS>();
    database::create_table<database::tables::IS>();
    database::create_table<database::tables::LT>();
    database::create_table<database::tables::OL>();
    database::create_table<database::tables::PN>();
    database::create_table<database::tables::ST>();
    database::create_table<database::tables::LO>();

    database::factory_t::instance()->execute("PRAGMA foreign_keys = ON;");

    const auto id = database::insert_into<DataSet>(DataSet::date = 1112317l); // return new id
    const auto id_tag_patient_name = database::insert_into<Tag>( Tag::value = 0x00100010l ); // return new id

    database::insert_into<PN>(   PN::tag = id_tag_patient_name
                               , PN::dataset = id
                               , PN::value = "Blaise" );


    for ( const auto [tag, value] : database::select( Tag::value, PN::value )
          .from<DataSet>()
          .left_join<PN>(PN::dataset == DataSet::id)
          .left_join<Tag>(Tag::id == PN::tag)
          .where(DataSet::id == id))
    {
        std::cout << " tag: " << tag << " value: " << value << std::endl;
    }

    return 0;
}
