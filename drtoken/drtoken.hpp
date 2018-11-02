/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

#include <string>

using namespace eosio;

namespace ampersand {

    using std::string;

    class drtoken : public contract {

    public:
        
        drtoken(account_name self):contract(self){}

        // token creation with transfer_locked state    
        [[eosio::action]]
        void create(account_name issuer,
                    asset maximum_supply,
                    bool transfer_locked);

        // issue the created tokens
        [[eosio::action]]
        void issue(account_name to, asset quantity, string memo);

        /// unlock the transfer state of the asset being created
        [[eosio::action]]
        void unlock(asset unlock);

        // transfer tokens
        [[eosio::action]]
        void transfer(account_name from,
                      account_name to,
                      asset quantity,
                      string memo);
      
        inline asset get_supply(symbol_name sym)const;
         
        inline asset get_balance(account_name owner, symbol_name sym)const;

    private:
        // blockchain db structure to store account vs balance of tokens
        struct [[eosio::table]] account {
            asset balance;
            uint64_t primary_key()const { return balance.symbol.name(); }
        };

        // blockchain db structure to store token stats 
        struct [[eosio::table]] currency_stats {
            asset supply;
            asset max_supply;
            account_name issuer;
            bool transfer_locked;
            uint64_t primary_key()const { return supply.symbol.name(); }
        };

        typedef eosio::multi_index<N(accounts), account> accounts;
        typedef eosio::multi_index<N(stats), currency_stats> stats;

        void sub_balance( account_name owner, asset value );
        void add_balance( account_name owner, asset value, account_name ram_payer );

    public:
        struct transfer_args {
            account_name from;
            account_name to;
            asset quantity;
            string memo;
        };
    };

    asset drtoken::get_supply( symbol_name sym )const
    {
        stats statstable( _self, sym );
        const auto& st = statstable.get( sym );
         return st.supply;
    }

    asset drtoken::get_balance( account_name owner, symbol_name sym )const
    {
        accounts accountstable( _self, owner );
        const auto& ac = accountstable.get( sym );
        return ac.balance;
    }

} /// namespace ampersand
