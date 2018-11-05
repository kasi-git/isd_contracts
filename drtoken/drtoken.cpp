/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include "drtoken.hpp"

namespace ampersand {

void drtoken::create(name issuer,
                     asset maximum_supply,
                     bool transfer_locked)
{
    require_auth(_code);
//    require_auth2(name("amprllc"), name("create"));
    
    auto sym = maximum_supply.symbol;
    eosio_assert(sym.is_valid(), "invalid symbol name");
    eosio_assert(maximum_supply.is_valid(), "invalid supply");
    eosio_assert(maximum_supply.amount > 0, "max-supply must be positive");

    stats statstable(_code, sym.raw());
    auto existing = statstable.find(sym.raw());
    eosio_assert(existing == statstable.end(), "token with symbol already exists");

    statstable.emplace(_self, [&](auto& s) {
        s.supply.symbol = maximum_supply.symbol;
        s.max_supply = maximum_supply;
        s.issuer = issuer;
        s.transfer_locked = transfer_locked;
    });
}

void drtoken::issue(name to,
                    asset quantity,
                    string memo)
{
    auto sym = quantity.symbol;
    eosio_assert(sym.is_valid(), "invalid symbol name");
    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

    stats statstable(_code, sym.raw());

    auto existing = statstable.find(sym.raw());
    eosio_assert( existing != statstable.end(),
                  "token with symbol does not exist, create token before issue");
    const auto& st = *existing;

    require_auth(st.issuer);
//    require_auth2(name("amprllc"), name("issue"));

    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "must issue positive quantity");
    eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
    eosio_assert( quantity.amount <= st.max_supply.amount - st.supply.amount,
                  "quantity exceeds available supply");

    statstable.modify(st, same_payer, [&](auto& s) {
        s.supply += quantity;
    });

    add_balance(st.issuer, quantity, st.issuer);

    if(to != st.issuer) {
        SEND_INLINE_ACTION( *this, transfer, {st.issuer,name("active")}, {st.issuer, to, quantity, memo} );
    }
}

void drtoken::unlock(asset unlock)
{
    eosio_assert(unlock.symbol.is_valid(), "invalid symbol name");
    eosio_assert(unlock.is_valid(), "invalid supply");

    auto symbol_code = unlock.symbol.raw();
    stats statstable(_code, symbol_code);

    auto token = statstable.find(symbol_code);
    eosio_assert(token != statstable.end(), "token with the symbol doesn't exist");

    const auto &stats_record = *token;

    // authorization from issuer@issuer is needed eg: amprllc@issuer
    //require_auth((stats_record.issuer, name("issuer"));
    require_auth(stats_record.issuer);

    statstable.modify(stats_record, same_payer, [&](auto& srec) {
        srec.transfer_locked = false;
    });
}

void drtoken::transfer(name from,
                       name to,
                       asset quantity,
                       string memo )
{
    eosio_assert(from != to, "cannot transfer to self");

    require_auth(from);

    eosio_assert(is_account(to), "to account does not exist");

    auto sym = quantity.symbol;
    stats statstable(_code, sym.raw());

    const auto& st = statstable.get(sym.raw());

    if (st.transfer_locked) {
        require_auth(st.issuer);
    }

//    require_recipient(from);
//    require_recipient(to);

    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "must transfer positive quantity");
    eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

    sub_balance(from, quantity);
    add_balance(to, quantity, from);
}

void drtoken::sub_balance(name owner,
                          asset value)
{
    accounts from_acnts(_self, owner.value);

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

void drtoken::add_balance(name owner, asset value, name ram_payer)
{
    accounts to_acnts(_self, owner.value);

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

EOSIO_DISPATCH(ampersand::drtoken, (create)(issue)(unlock)(transfer))
