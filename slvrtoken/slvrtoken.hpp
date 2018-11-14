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

    CONTRACT slvrtoken : public eosio::contract {

    public:
	using contract::contract;

        const name DRTOKEN_CONTRACT_ACCNAME = name("drtokenac");
        const name IOUTOKEN_CONTRACT_ACCNAME = name("ioutokenac");

        [[eosio::action]]
        void create(name issuer,
                    asset maximum_supply,
                    bool transfer_locked);

        [[eosio::action]]
        void issue(name to, asset quantity, string memo);

        [[eosio::action]]
        void unlock(asset unlock);

        [[eosio::action]]
        void transfer(name from,
                      name to,
                      asset quantity,
                      string memo);

        [[eosio::action]]
        void redeem(name owner,
                    asset quantity);

        [[eosio::action]]
        void burn(name owner,
                  asset quantity);

        inline asset get_supply(symbol sym)const;

        inline asset get_balance(name owner, symbol sym)const;

    private:

        struct [[eosio::table]]  account {
            asset balance;

            uint64_t primary_key()const { return balance.symbol.raw(); }

            EOSLIB_SERIALIZE(account, (balance))
        };

        struct [[eosio::table]] currency_stats {
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

    asset slvrtoken::get_supply( symbol sym )const
    {
        stats statstable(_code, sym.raw());
        const auto& st = statstable.get(sym.raw());
         return st.supply;
    }

    asset slvrtoken::get_balance(name owner, symbol sym)const
    {
        accounts accountstable(_code, owner.value);
        const auto& ac = accountstable.get( sym.raw());
        return ac.balance;
    }

} /// namespace ampersand

