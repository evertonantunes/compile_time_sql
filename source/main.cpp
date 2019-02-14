#include "api.hpp"
#include "sqlite_factory.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

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

        struct AE : public Table<decltype("T_AE"_s)>
        {
            static constexpr Column<AE, decltype("id"_s)     , std::ptrdiff_t, Flags<pk, not_null>                       > id      = {};
            static constexpr Column<AE, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>                           > tag     = {};
            static constexpr Column<AE, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<AE, decltype("value"_s)  , std::ptrdiff_t, Flags<not_null>                           > value   = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct AS: public Table<decltype("T_AS"_s)>
        {
            static constexpr Column<AS, decltype("id"_s)     , std::ptrdiff_t   , Flags<pk, not_null>                       > id        = {};
            static constexpr Column<AS, decltype("tag"_s)    , std::ptrdiff_t   , Flags<not_null>    > tag       = {};
            static constexpr Column<AS, decltype("dataset"_s), std::ptrdiff_t   , Flags<fk<decltype(DataSet::id)>, not_null>> dataset   = {};
            static constexpr Column<AS, decltype("value"_s)  , std::string_view , Flags<not_null>                           > value     = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct AT: public Table<decltype("T_AT"_s)>
        {
            static constexpr Column<AT, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<AT, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<AT, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<AT, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct OB: public Table<decltype("T_OB"_s)>
        {
            static constexpr Column<OB, decltype("id"_s)     , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<OB, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<OB, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<OB, decltype("value"_s)  , std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct OW: public Table<decltype("T_OW"_s)>
        {
            static constexpr Column<OW, decltype("id"_s)     , std::ptrdiff_t   , Flags<pk, not_null>                       > id        = {};
            static constexpr Column<OW, decltype("tag"_s)    , std::ptrdiff_t   , Flags<not_null>    > tag       = {};
            static constexpr Column<OW, decltype("dataset"_s), std::ptrdiff_t   , Flags<fk<decltype(DataSet::id)>, not_null>> dataset   = {};
            static constexpr Column<OW, decltype("value"_s)  , std::string_view , Flags<not_null>                           > value     = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct OF: public Table<decltype("T_OF"_s)>
        {
            static constexpr Column<OF, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<OF, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<OF, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<OF, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct OD: public Table<decltype("T_OD"_s)>
        {
            static constexpr Column<OD, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<OD, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<OD, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<OD, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct SQ: public Table<decltype("T_SQ"_s)>
        {
            static constexpr Column<SQ, decltype("id"_s)     , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<SQ, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<SQ, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<SQ, decltype("value"_s)  , std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> value = {};
            static constexpr Column<SQ, decltype("c_index"_s), std::ptrdiff_t, Flags<not_null>                           > index = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value), decltype(index)>; // future introspection
        };

        struct UT: public Table<decltype("T_UT"_s)>
        {
            static constexpr Column<UT, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<UT, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<UT, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<UT, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct UN: public Table<decltype("T_UN"_s)>
        {
            static constexpr Column<UN, decltype("id"_s)        , std::ptrdiff_t    , Flags<pk, not_null>                       > id        = {};
            static constexpr Column<UN, decltype("tag"_s)       , std::ptrdiff_t    , Flags<not_null>    > tag       = {};
            static constexpr Column<UN, decltype("dataset"_s)   , std::ptrdiff_t    , Flags<fk<decltype(DataSet::id)>, not_null>> dataset   = {};
            static constexpr Column<UN, decltype("value"_s)     , std::string_view  , Flags<not_null>                           > value     = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct UL: public Table<decltype("T_UL"_s)>
        {
            static constexpr Column<UL, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<UL, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<UL, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<UL, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct US: public Table<decltype("T_US"_s)>
        {
            static constexpr Column<US, decltype("id"_s)     , std::ptrdiff_t, Flags<pk, not_null>                       > id      = {};
            static constexpr Column<US, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<US, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<US, decltype("value"_s)  , unsigned short, Flags<not_null>                           > value   = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct SS: public Table<decltype("T_SS"_s)>
        {
            static constexpr Column<SS, decltype("id"_s)     , std::ptrdiff_t, Flags<pk, not_null>                       > id      = {};
            static constexpr Column<SS, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<SS, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<SS, decltype("value"_s)  , signed short  , Flags<not_null>                           > value   = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct SL: public Table<decltype("T_SL"_s)>
        {
            static constexpr Column<SL, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<SL, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<SL, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<SL, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct FD: public Table<decltype("T_FD"_s)>
        {
            static constexpr Column<FD, decltype("id"_s)     , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<FD, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<FD, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<FD, decltype("value"_s)  , double        , Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct FL: public Table<decltype("T_FL"_s)>
        {
            static constexpr Column<FL, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<FL, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<FL, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<FL, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct DA: public Table<decltype("T_DA"_s)>
        {
            static constexpr Column<DA, decltype("id"_s)     , std::ptrdiff_t  , Flags<pk, not_null>                        > id        = {};
            static constexpr Column<DA, decltype("tag"_s)    , std::ptrdiff_t  , Flags<not_null>     > tag       = {};
            static constexpr Column<DA, decltype("dataset"_s), std::ptrdiff_t  , Flags<fk<decltype(DataSet::id)>, not_null> > dataset   = {};
            static constexpr Column<DA, decltype("value"_s)  , std::string_view, Flags<not_null>                            > value     = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct UI: public Table<decltype("T_UI"_s)>
        {
            static constexpr Column<UI, decltype("id"_s)     , std::ptrdiff_t  , Flags<pk, not_null>                       > id      = {};
            static constexpr Column<UI, decltype("tag"_s)    , std::ptrdiff_t  , Flags<not_null>    > tag     = {};
            static constexpr Column<UI, decltype("dataset"_s), std::ptrdiff_t  , Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<UI, decltype("value"_s)  , std::string_view, Flags<not_null>                           > value   = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct SH: public Table<decltype("T_SH"_s)>
        {
            static constexpr Column<SH, decltype("id"_s)     , std::ptrdiff_t  , Flags<pk, not_null>                        > id        = {};
            static constexpr Column<SH, decltype("tag"_s)    , std::ptrdiff_t  , Flags<not_null>     > tag       = {};
            static constexpr Column<SH, decltype("dataset"_s), std::ptrdiff_t  , Flags<fk<decltype(DataSet::id)>, not_null> > dataset   = {};
            static constexpr Column<SH, decltype("value"_s)  , std::string_view, Flags<not_null>                            > value     = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct TM: public Table<decltype("T_TM"_s)>
        {
            static constexpr Column<TM, decltype("id"_s)     , std::ptrdiff_t  , Flags<pk, not_null>                        > id        = {};
            static constexpr Column<TM, decltype("tag"_s)    , std::ptrdiff_t  , Flags<not_null>     > tag       = {};
            static constexpr Column<TM, decltype("dataset"_s), std::ptrdiff_t  , Flags<fk<decltype(DataSet::id)>, not_null> > dataset   = {};
            static constexpr Column<TM, decltype("value"_s)  , std::string_view, Flags<not_null>                            > value     = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct UC: public Table<decltype("T_UC"_s)>
        {
            static constexpr Column<UC, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<UC, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<UC, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<UC, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct UR: public Table<decltype("T_UR"_s)>
        {
            static constexpr Column<UR, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<UR, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<UR, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<UR, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct DT: public Table<decltype("T_DT"_s)>
        {
            static constexpr Column<DT, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<DT, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<DT, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<DT, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct DS: public Table<decltype("T_DS"_s)>
        {
            static constexpr Column<DS, decltype("id"_s)     , std::ptrdiff_t, Flags<pk, not_null>                       > id      = {};
            static constexpr Column<DS, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<DS, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<DS, decltype("value"_s)  , double        , Flags<not_null>                           > value   = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct CS: public Table<decltype("T_CS"_s)>
        {
            static constexpr Column<CS, decltype("id"_s)     , std::ptrdiff_t  , Flags<pk, not_null>                       > id      = {};
            static constexpr Column<CS, decltype("tag"_s)    , std::ptrdiff_t  , Flags<not_null>    > tag     = {};
            static constexpr Column<CS, decltype("dataset"_s), std::ptrdiff_t  , Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<CS, decltype("value"_s)  , std::string_view, Flags<not_null>                           > value   = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct IS: public Table<decltype("T_IS"_s)>
        {
            static constexpr Column<IS, decltype("id"_s)     , std::ptrdiff_t, Flags<pk, not_null>                       > id      = {};
            static constexpr Column<IS, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<IS, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<IS, decltype("value"_s)  , std::ptrdiff_t, Flags<not_null>                           > value   = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct LT: public Table<decltype("T_LT"_s)>
        {
            static constexpr Column<LT, decltype("id"_s)   , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<LT, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<LT, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<LT, decltype("value"_s), std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct OL: public Table<decltype("T_OL"_s)>
        {
            static constexpr Column<OL, decltype("id"_s)     , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<OL, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<OL, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<OL, decltype("value"_s)  , std::ptrdiff_t, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct PN: public Table<decltype("T_PN"_s)>
        {
            static constexpr Column<PN, decltype("id"_s)     , std::ptrdiff_t, Flags<pk, not_null>> id    = {};
            static constexpr Column<PN, decltype("tag"_s)    , std::ptrdiff_t, Flags<not_null>    > tag     = {};
            static constexpr Column<PN, decltype("dataset"_s), std::ptrdiff_t, Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<PN, decltype("value"_s)  , std::string_view, Flags<not_null>    > value = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct ST: public Table<decltype("T_ST"_s)>
        {
            static constexpr Column<ST, decltype("id"_s)     , std::ptrdiff_t  , Flags<pk, not_null>                        > id        = {};
            static constexpr Column<ST, decltype("tag"_s)    , std::ptrdiff_t  , Flags<not_null>     > tag       = {};
            static constexpr Column<ST, decltype("dataset"_s), std::ptrdiff_t  , Flags<fk<decltype(DataSet::id)>, not_null> > dataset   = {};
            static constexpr Column<ST, decltype("value"_s)  , std::string_view, Flags<not_null>                            > value     = {};

            using columns = std::tuple<decltype(id), decltype(tag), decltype(dataset), decltype(value)>; // future introspection
        };

        struct LO: public Table<decltype("T_LO"_s)>
        {
            static constexpr Column<LO, decltype("id"_s)     , std::ptrdiff_t , Flags<pk, not_null>                       > id      = {};
            static constexpr Column<LO, decltype("tag"_s)    , std::ptrdiff_t , Flags<not_null>    > tag     = {};
            static constexpr Column<LO, decltype("dataset"_s), std::ptrdiff_t , Flags<fk<decltype(DataSet::id)>, not_null>> dataset = {};
            static constexpr Column<LO, decltype("value"_s)  , std::string_view, Flags<not_null>                          > value   = {};

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

namespace
{
    std::ptrdiff_t get_tag( const auto &key )
    {
        std::ptrdiff_t result = 0;
        {
            std::stringstream stream;
            stream << std::hex << key;
            stream >> result;
        }
        return result;
    }


    template<typename T>
    void load_vr(const std::ptrdiff_t dataset_id, const std::ptrdiff_t tag, const nlohmann::json &data)
    {
        using namespace database::tables;

        if (data.size() == 0)
        {
            return;
        }

        if constexpr (std::is_same_v<T, PN>)
        {
            using type_t = typename decltype(T::value)::type_t;
            database::insert_into<T>(  T::tag = tag
                                     , T::dataset = dataset_id
                                     , T::value = data["Value"][0]["Alphabetic"].get<type_t>() );
        }
        else if constexpr (std::is_same_v<T, OW> || std::is_same_v<T, UN>)
        {
            using type_t = typename decltype(T::value)::type_t;
            database::insert_into<T>(  T::tag = tag
                                     , T::dataset = dataset_id
                                     , T::value = data["InlineBinary"].get<type_t>() );
        }
        else
        {
            if (!data.is_object())
            {
                return;
            }

            using type_t = typename decltype(T::value)::type_t;

            for ( const auto &value : data["Value"] )
            {
                database::insert_into<T>(  T::tag = tag
                                          , T::dataset = dataset_id
                                          , T::value = value.get<type_t>() );
            }
        }
    }

    void load( const std::ptrdiff_t dataset_id, const nlohmann::json &data )
    {
        using namespace database::tables;

        if (data.size() == 0)
        {
            return;
        }

        if (!data.is_object())
        {
            return;
        }

        for ( const auto &item : data.items())
        {
            const std::ptrdiff_t tag_code = get_tag(item.key());

            const auto vr = item.value()["vr"].get<std::string_view>();

            if (    (item.value().find("Value") == end(item.value()))
                 && (item.value().find("InlineBinary") == end(item.value())))
            {
                continue;
            }

            if (vr == "SQ")
            {
                std::ptrdiff_t index = 0;
                for ( const auto &sub_items : item.value()["Value"])
                {
                    const auto sub_dataset_id = database::insert_into<DataSet>(DataSet::date = 0l);

                    database::insert_into<SQ>(  SQ::tag = tag_code
                                              , SQ::dataset = dataset_id
                                              , SQ::value = sub_dataset_id
                                              , SQ::index = index++ );

                    const auto &sub_dataset_data = sub_items;
                    load(sub_dataset_id, sub_dataset_data);
                }
            }

            if (vr == "AE") load_vr<AE>(dataset_id, tag_code, item.value()); continue;
            if (vr == "AS") load_vr<AS>(dataset_id, tag_code, item.value()); continue;
            if (vr == "AT") load_vr<AT>(dataset_id, tag_code, item.value()); continue;
            if (vr == "OB") load_vr<OB>(dataset_id, tag_code, item.value()); continue;
            if (vr == "OW") load_vr<OW>(dataset_id, tag_code, item.value()); continue;
            if (vr == "OF") load_vr<OF>(dataset_id, tag_code, item.value()); continue;
            if (vr == "OD") load_vr<OD>(dataset_id, tag_code, item.value()); continue;
            if (vr == "SQ") load_vr<SQ>(dataset_id, tag_code, item.value()); continue;
            if (vr == "UT") load_vr<UT>(dataset_id, tag_code, item.value()); continue;
            if (vr == "UN") load_vr<UN>(dataset_id, tag_code, item.value()); continue;
            if (vr == "UL") load_vr<UL>(dataset_id, tag_code, item.value()); continue;
            if (vr == "US") load_vr<US>(dataset_id, tag_code, item.value()); continue;
            if (vr == "SS") load_vr<SS>(dataset_id, tag_code, item.value()); continue;
            if (vr == "SL") load_vr<SL>(dataset_id, tag_code, item.value()); continue;
            if (vr == "FD") load_vr<FD>(dataset_id, tag_code, item.value()); continue;
            if (vr == "FL") load_vr<FL>(dataset_id, tag_code, item.value()); continue;
            if (vr == "DA") load_vr<DA>(dataset_id, tag_code, item.value()); continue;
            if (vr == "UI") load_vr<UI>(dataset_id, tag_code, item.value()); continue;
            if (vr == "SH") load_vr<SH>(dataset_id, tag_code, item.value()); continue;
            if (vr == "TM") load_vr<TM>(dataset_id, tag_code, item.value()); continue;
            if (vr == "UC") load_vr<UC>(dataset_id, tag_code, item.value()); continue;
            if (vr == "UR") load_vr<UR>(dataset_id, tag_code, item.value()); continue;
            if (vr == "DT") load_vr<DT>(dataset_id, tag_code, item.value()); continue;
            if (vr == "DS") load_vr<DS>(dataset_id, tag_code, item.value()); continue;
            if (vr == "CS") load_vr<CS>(dataset_id, tag_code, item.value()); continue;
            if (vr == "IS") load_vr<IS>(dataset_id, tag_code, item.value()); continue;
            if (vr == "LT") load_vr<LT>(dataset_id, tag_code, item.value()); continue;
            if (vr == "OL") load_vr<OL>(dataset_id, tag_code, item.value()); continue;
            if (vr == "PN") load_vr<PN>(dataset_id, tag_code, item.value()); continue;
            if (vr == "ST") load_vr<ST>(dataset_id, tag_code, item.value()); continue;
            if (vr == "LO") load_vr<LO>(dataset_id, tag_code, item.value()); continue;
            std::cout << "Not implemented: " << vr << std::endl;
        }
    }


    template<typename T>
    using select_t = decltype(database::select(T::id, T::tag, T::value).template from<database::tables::DataSet>().template left_join<T>(T::dataset == database::tables::DataSet::id));

    template<typename T>
    void print( const std::ptrdiff_t dataset_id, const std::size_t level )
    {
        using namespace database::tables;
        for (const auto [id, tag, value] : select_t<T>().where(DataSet::id == dataset_id))
        {
            std::cout << dataset_id << " " << typename T::name_t().view() << " " << id << " tag: " << tag << " value: " << value << std::endl;
        }
    }

    void print( const std::ptrdiff_t id, const std::size_t level = 0 )
    {
        using namespace database::tables;

        print<AE>(id, level);
        print<AS>(id, level);
        print<AT>(id, level);
        print<OB>(id, level);
        print<OW>(id, level);
        print<OF>(id, level);
        print<OD>(id, level);
        print<UT>(id, level);
        print<UN>(id, level);
        print<UL>(id, level);
        print<US>(id, level);
        print<SS>(id, level);
        print<SL>(id, level);
        print<FD>(id, level);
        print<FL>(id, level);
        print<DA>(id, level);
        print<UI>(id, level);
        print<SH>(id, level);
        print<TM>(id, level);
        print<UC>(id, level);
        print<UR>(id, level);
        print<DT>(id, level);
        print<DS>(id, level);
        print<CS>(id, level);
        print<IS>(id, level);
        print<LT>(id, level);
        print<OL>(id, level);
        print<PN>(id, level);
        print<ST>(id, level);
        print<LO>(id, level);

        for ( const auto [tag, value] : database::select(SQ::tag, SQ::value)
              .from<DataSet>()
              .left_join<SQ>(SQ::dataset == DataSet::id)
              .where(DataSet::id == id))
        {
            std::cout << id << " SQ tag: " << tag << " value: " << value << std::endl;
            print(value, level + 1);
        }
    }
}

int main()
{
    using namespace database::tables;

   // database::factory_t::instance()->reset("/home/everton/compile_time_sql/database.db");

    database::create_table<database::tables::DataSet>();
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

    const auto id = database::insert_into<DataSet>(DataSet::date = 0l);
    {
        nlohmann::json data;
        {
            std::ifstream stream("/home/everton/projects/compile_time_sql/file.json");
            if (!stream)
            {
                std::cerr << "faile.json not open" << std::endl;
                return -1;
            }
            stream >> data;
        }

        load(id, data);
    }

    print(id);

    return 0;
}
