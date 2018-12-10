
#
# start local nodeos
#

#nodeos -e -p eosio --plugin eosio::chain_api_plugin eosio::history_api_plugin --contracts-console verbose-http-errors = true --delete-all-blocks

#
# wallet unlock 
#
# cleos wallet unlock -n ampertester
#

#
# create eosio accounts
#
cleos create account eosio amperioutokn EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ

cleos create account eosio amperslvtokn EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ

cleos create account eosio amperdrstokn EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ

cleos create account eosio ampercust111 EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ

#
# deploy contracts
#
cleos set contract amperioutokn ./ioutoken/

cleos set contract amperslvtokn ./slvrtoken/

cleos set contract amperdrstokn ./drtoken/

#
# create tokens
#
cleos push action amperioutokn create '{"issuer":"amperioutokn","new_supply":"1000000.0000 IOU","transfer_locked":true}' -p amperioutokn 

cleos push action amperslvtokn create '{"issuer":"amperslvtokn","new_supply":"1000000.0000 SLVR","slvr_per_token_mg":2000,"transfer_locked":true,"redeem_locked":true}' -p amperslvtokn

### note drs tokens are created with amperslvtokn as issuer
cleos push action amperdrstokn create '{"issuer":"amperslvtokn","new_supply":"1000000.0000 ADR","transfer_locked":true}' -p amperdrstokn

#
# setup eosio.code permission to amperslvtokn contract
#
cleos set account permission amperslvcont active '{ "threshold": 1, "keys": [ { "key": "EOS5CMwziTNPnn3g2X7VkRKT77tecsushPLz1rCBDCVRjTucZDk2T", "weight": 1 } ], "accounts": [ { "permission": { "actor": "amperslvcont", "permission": "eosio.code" }, "weight": 1 } ] }'

#
# issue tokens
#
cleos push action amperioutokn issue '{"to":"ampercust111","quantity":"10.0000 IOU","memo":"issue"}'  -p amperioutokn

cleos push action amperslvtokn issue '{"to":"ampercust111","quantity":"10.0000 SLVR","memo":"issue"}'  -p amperslvtokn




