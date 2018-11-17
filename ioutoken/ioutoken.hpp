/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>

#include <string>

using namespace eosio;

namespace ampersand {

    using std::string;

    CONTRACT ioutoken : public eosio::contract {

    public:
	using contract::contract;

        ACTION create(name issuer,
                    asset maximum_supply,
                    bool transfer_locked);

        ACTION issue(name to, asset quantity, string memo);

        ACTION unlock(asset unlock);

        ACTION transfer(name from,
                      name to,
                      asset quantity,
                      string memo);

        ACTION burn(name owner,
                  asset quantity);

        inline asset get_supply(symbol sym)const;

        inline asset get_balance(name owner, symbol sym)const;

    private:

        TABLE account {
            asset balance;

            uint64_t primary_key()const { return balance.symbol.raw(); }

            EOSLIB_SERIALIZE(account, (balance))
        };

        TABLE currency_stats {
            asset supply;
            asset total_supply;
            name issuer;
            bool transfer_locked;

            uint64_t primary_key()const { return supply.symbol.raw(); }

            EOSLIB_SERIALIZE(currency_stats, (supply)(total_supply)(issuer)(transfer_locked))
        };

        typedef eosio::multi_index<"accounts"_n, account> accounts;
        typedef eosio::multi_index<"stats"_n, currency_stats> stats;

        void sub_balance( name owner, asset value );
        void add_balance( name owner, asset value, name ram_payer );

    public:

        struct transfer_args {
            name from;
            name to;
            asset quantity;
            string memo;
        };

    };

    asset ioutoken::get_supply( symbol sym )const
    {
        stats statstable(_code, sym.raw());
        const auto& st = statstable.get(sym.raw());
         return st.supply;
    }

    asset ioutoken::get_balance(name owner, symbol sym)const
    {
        accounts accountstable(_code, owner.value);
        const auto& ac = accountstable.get( sym.raw());
        return ac.balance;
    }

} /// namespace ampersand
