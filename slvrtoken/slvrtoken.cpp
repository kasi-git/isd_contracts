/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include "slvrtoken.hpp"

namespace ampersand {

void slvrtoken::create(name issuer,
                     asset new_supply,
                     bool transfer_locked)
{
    require_auth(_code);
//    require_auth2(name("amprllc"), name("create"));
    
    auto sym = new_supply.symbol;
    eosio_assert(sym.is_valid(), "invalid symbol name ");
    eosio_assert(new_supply.is_valid(), "invalid supply");
    eosio_assert(new_supply.amount > 0, "max-supply must be positive");
    stats statstable(_code, sym.raw());

    auto iterator = statstable.find(sym.raw());

    // Token is added for the first time
    if (iterator == statstable.end()){	
        statstable.emplace(_code, [&](auto& token_stats) {
            token_stats.supply.symbol = new_supply.symbol;
            token_stats.total_supply = new_supply;
            token_stats.issuer = issuer;
            token_stats.transfer_locked = transfer_locked;
        });
    // Token Already exists, reissuing with new supply
    } else {
        statstable.modify(iterator, same_payer, [&](auto& token_stats_record) {
            token_stats_record.supply.symbol = new_supply.symbol;
            token_stats_record.total_supply += new_supply;
            token_stats_record.issuer = issuer;
            token_stats_record.transfer_locked = transfer_locked;
        });
    }
}

void slvrtoken::issue(name to,
                    asset quantity,
                    string memo)
{
    auto sym = quantity.symbol;
    eosio_assert(sym.is_valid(), "invalid symbol name");
    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

    stats statstable(_code, sym.raw());

    auto iterator = statstable.find(sym.raw());
    eosio_assert( iterator != statstable.end(),
                  "token with symbol does not exist, create token before issue");

    require_auth(iterator->issuer);
//    require_auth2(name("amprllc"), name("issue"));

    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "must issue positive quantity");
    eosio_assert(quantity.symbol == iterator->supply.symbol, "symbol precision mismatch");
    eosio_assert( quantity.amount <= iterator->total_supply.amount - iterator->supply.amount,
                  "quantity exceeds available supply");

    statstable.modify(iterator, same_payer, [&](auto& token_stats_record) {
        token_stats_record.supply += quantity;
    });

    add_balance(iterator->issuer, quantity, iterator->issuer);

    if(to != iterator->issuer) {
        SEND_INLINE_ACTION( *this, 
                            transfer, 
                            {iterator->issuer, name("active")},
                            {iterator->issuer, to, quantity, memo} );
    }
}

void slvrtoken::unlock(asset unlock)
{
    eosio_assert(unlock.symbol.is_valid(), "invalid symbol name");
    eosio_assert(unlock.is_valid(), "invalid supply");

    auto symbol_code = unlock.symbol.raw();
    stats statstable(_code, symbol_code);

    auto iterator = statstable.find(symbol_code);
    eosio_assert(iterator != statstable.end(), "token with the symbol doesn't exist");

    // authorization from issuer@issuer is needed eg: amprllc@issuer
    //require_auth((stats_record.issuer, name("issuer"));
    require_auth(iterator->issuer);

    statstable.modify(iterator, same_payer, [&](auto& token_stats_record) {
        token_stats_record.transfer_locked = false;
    });
}

void slvrtoken::transfer(name from,
                       name to,
                       asset quantity,
                       string memo )
{
    eosio_assert(from != to, "cannot transfer to self");

    require_auth(from);

    eosio_assert(is_account(to), "to account does not exist");

    auto sym = quantity.symbol;
    stats statstable(_code, sym.raw());

    eosio_assert(statstable.find(quantity.symbol.raw()) != statstable.end(), "token with the symbol doesn't exist");

    const auto& token_stats_record = statstable.get(sym.raw());

    if (token_stats_record.transfer_locked) {
        require_auth(token_stats_record.issuer);
    }

//    require_recipient(from);
//    require_recipient(to);

    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "must transfer positive quantity");
    eosio_assert(quantity.symbol == token_stats_record.supply.symbol, "symbol precision mismatch");
    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

    sub_balance(from, quantity);
    add_balance(to, quantity, from);
}

void slvrtoken::redeem(name owner,
                       asset quantity)
{

eosio::print("***redeem called*** ");
/*
    action(
        permission_level{get_self(), name("active")},
        name("drtokenac"), name("redeemtx"),
        std::make_tuple(name("slvrtokenac"), owner, "10.0000 DRT",std::string(""))
    ).send();
*/
    action(
        permission_level{get_self(), "active"_n},
        "drtokenac"_n, "tx"_n,
        std::make_tuple(owner,quantity)
    ).send();
}

void slvrtoken::sub_balance(name owner,
                            asset value)
{
    accounts from_acnts(_code, owner.value);

    const auto& from = from_acnts.get(value.symbol.raw(), "no balance object found");

    eosio_assert(from.balance.amount >= value.amount, "overdrawn balance");

    if(from.balance.amount == value.amount) {
       from_acnts.erase(from);
    } else {
        from_acnts.modify(from, owner, [&](auto& a) {
            a.balance -= value;
        });
   }
}

void slvrtoken::add_balance(name owner, asset value, name ram_payer)
{
    accounts to_acnts(_code, owner.value);

    auto to = to_acnts.find(value.symbol.raw());

    if(to == to_acnts.end()) {
       to_acnts.emplace(ram_payer, [&](auto& a){
            a.balance = value;
        });
    } else {
        to_acnts.modify(to, same_payer, [&](auto& a) {
            a.balance += value;
        });
    }
}

} /// namespace ampersand

EOSIO_DISPATCH(ampersand::slvrtoken, (create)(issue)(unlock)(redeem)(transfer))
