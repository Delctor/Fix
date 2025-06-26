#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <array>
#include <cstdint>
#include <immintrin.h>
#include <iomanip>
#include <optional>
#include <functional>
#include <type_traits>
#include <tuple>
#include <variant>
#include "crc32Constexpr.h"

namespace fix
{
    template<size_t Ntable, size_t Ng>
    uint64_t perfectHash(uint64_t key, const std::array<uint64_t, Ng>& g) {
        size_t bucket = _mm_crc32_u64(0, key) % Ng;
        return _mm_crc32_u64(0, key + g[bucket]) % Ntable;
    }

    template<typename T>
    struct func_type {
        using type = void (*)(T);
    };

    template<>
    struct func_type<void> {
        using type = void (*)();
    };

    template<typename T>
    using func_type_t = typename func_type<T>::type;

    template<typename T, bool isDefault>
    struct CaseImpl {
        using type = T;
        using Func = func_type_t<T>;

        uint64_t key;
        Func func;

        constexpr CaseImpl() : key(0), func(nullptr) {}

        constexpr CaseImpl(uint64_t k, Func f) requires (!isDefault) : key(k), func(f) {}

        constexpr CaseImpl(Func f) requires (isDefault) : key(UINT64_MAX), func(f) {}

        constexpr CaseImpl& operator=(const CaseImpl& other) {
            this->key = other.key;
            this->func = other.func;
            return *this;
        }
    };

    template<typename T>
    CaseImpl(uint64_t, void(*)(T)) -> CaseImpl<T, false>;

    template<typename T>
    CaseImpl(void(*)(T)) -> CaseImpl<T, true>;

    CaseImpl(uint64_t, void(*)()) -> CaseImpl<void, false>;

    CaseImpl(void(*)()) -> CaseImpl<void, true>;

    template<typename T>
    using Case = CaseImpl<T, false>;

    template<typename T>
    using Default = CaseImpl<T, true>;

    template<typename Case>
    struct isDefault;

    template<typename T, bool isDefault_>
    struct isDefault<CaseImpl<T, isDefault_>> : std::bool_constant<isDefault_> {};

    template<
    typename T,
    size_t N,
    size_t tableSize,
    size_t bucketsSize,
    typename FuncType = void (*)(T),
    typename CaseType = Case<T>>
    constexpr auto getBucketsAndTable(const std::array<CaseType, N> cases)
    {
        static_assert(tableSize >= N && bucketsSize > 0, "Error");

        struct Bucket 
        {
            std::vector<uint64_t> keys;
            std::vector<FuncType> funcs;
        };

        std::vector<Bucket> buckets(bucketsSize);

        for (size_t i = 0; i < N; i++)
        {
            size_t b = perfectHashConstexpr(cases[i].key) % bucketsSize;
            buckets[b].keys.push_back(cases[i].key);
            buckets[b].funcs.push_back(cases[i].func);
        }

        std::array<uint64_t, bucketsSize> g;
        g.fill(0);

        std::array<std::optional<uint64_t>, tableSize> table;
        std::array<FuncType, tableSize> tableFuncs;
        tableFuncs.fill(nullptr);

        for (size_t b = 0; b < bucketsSize; ++b)
        {
            const auto& bucketKeys = buckets[b].keys;
            const auto& bucketFuncs = buckets[b].funcs;
            if (bucketKeys.empty()) continue;

            for (uint64_t d = 0; ; ++d)
            {
                bool collision = false;
                std::vector<size_t> slots;

                for (uint64_t k : bucketKeys)
                {
                    size_t h = perfectHashConstexpr(k + d) % tableSize;
                    if (table[h].has_value() || std::find(slots.begin(), slots.end(), h) != slots.end()) {
                        collision = true;
                        break;
                    }
                    slots.push_back(h);
                }

                if (!collision)
                {
                    g[b] = d;
                    for (size_t i = 0; i < bucketKeys.size(); ++i)
                    {
                        table[slots[i]] = bucketKeys[i];
                        tableFuncs[slots[i]] = bucketFuncs[i];
                    }
                    break;
                }
            }
        }

        return std::make_tuple(g, tableFuncs, table);
    }

    template <typename... Cases>
    struct get_common_type;

    template <typename Case0, typename... Rest>
    struct get_common_type<Case0, Rest...> {
        using type = typename Case0::type;
        static_assert((std::is_same_v<typename Case0::type, typename Rest::type> && ...),
                    "All Cases must have the same T");
    };

    template<typename... Args>
    constexpr size_t getNdefault()
    {
        size_t count = 0;

        ((count += isDefault<std::decay_t<Args>>::value ? 1 : 0), ...);
        return count;
    }

    template<size_t N, typename T>
    constexpr bool isNotRepeated(const std::array<Case<T>, N> cases)
    {
        if constexpr (cases.size() == 1) return true;
        
        for (size_t i = 0; i < cases.size(); i++)
        {
            for (size_t j = i + 1; j < cases.size(); j++)
            {
                if (cases[i].key == cases[j].key) return false;
            }
        }
        return true;
    }

    template <typename T, size_t Ndefault, typename... Args>
    constexpr std::tuple<std::array<Case<T>, sizeof...(Args) - Ndefault>, std::array<Default<T>, Ndefault>> separateHelper(Args... args)
    {
        std::array<std::variant<Case<T>, Default<T>>, sizeof...(Args)> arr {args...};

        std::array<Case<T>, sizeof...(Args) - Ndefault> arrCases;
        std::array<Default<T>, Ndefault> arrDef;

        size_t defIndex = 0;
        size_t casesIndex = 0;

        for (size_t i = 0; i < arr.size(); i++)
        {
            if (std::holds_alternative<Default<T>>(arr[i]))
            {
                arrDef[defIndex++] = std::get<Default<T>>(arr[i]);
            }
            else
            {
                arrCases[casesIndex++] = std::get<Case<T>>(arr[i]);
            }
        }
        return std::make_tuple(arrCases, arrDef);
    }

    template<size_t bucketsSizeTemplate = 0>
    class Switch {
    public:
        Switch(uint64_t key) : key(key) {}

        template<CaseImpl... cases, typename T = get_common_type<decltype(cases)...>::type>
        std::enable_if_t<std::is_same_v<T, void>> setCases()
        {
            this->setCasesImpl<cases...>()();
        }

        template<CaseImpl... cases, typename T = get_common_type<decltype(cases)...>::type>
        std::enable_if_t<!std::is_same_v<T, void>> setCases(T arg)
        {
            this->setCasesImpl<cases...>()(arg);
        }
    
    private:

        template<CaseImpl... cases>
        func_type_t<typename get_common_type<decltype(cases)...>::type> setCasesImpl() 
        {
            constexpr size_t nDefault = getNdefault<decltype(cases)...>();
            constexpr size_t N = sizeof...(cases) - nDefault;

            static_assert(N != 0, "There are no cases");
            static_assert(nDefault == 1, "More than one default case");

            constexpr size_t tableSize = N;
            constexpr size_t bucketsSize = bucketsSizeTemplate == 0 ? N : bucketsSizeTemplate;

            using T = typename get_common_type<decltype(cases)...>::type;

            constexpr auto tupleArgs = separateHelper<T, nDefault>(cases...);
            constexpr auto casesArr = std::get<0>(tupleArgs);
            constexpr auto defArr = std::get<1>(tupleArgs);
            constexpr auto defFunc = defArr[0].func;

            static_assert(isNotRepeated(casesArr), "Repeated keys");

            constexpr auto tuple = [] {
                if constexpr (std::is_void_v<T>) {
                    return getBucketsAndTable<void, N, tableSize, bucketsSize, void (*)(), Case<void>>(casesArr);
                } else {
                    return getBucketsAndTable<T, N, tableSize, bucketsSize>(casesArr);
                }
            }();

            static constexpr auto g = std::get<0>(tuple);
            static constexpr auto tableFuncs = std::get<1>(tuple);
            static constexpr auto table = std::get<2>(tuple);

            size_t i = perfectHash<tableSize, bucketsSize>(this->key, g);
            return table[i] == this->key ? tableFuncs[i] : defFunc;
        }


        uint64_t key;
    };

}