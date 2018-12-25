
#
# start local nodeos
#

#nodeos -e -p eosio --plugin eosio::chain_api_plugin eosio::history_api_plugin --contracts-console verbose-http-errors = true --delete-all-blocks

#
# wallet unlock 
#
# cleos wallet unlock -n ampertester
#

# create eosio accounts
cleos create account eosio amperioutokn EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ
cleos create account eosio amperslvtokn EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ
cleos create account eosio amperdrstokn EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ
cleos create account eosio ampercust111 EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ
cleos create account eosio ampercust222 EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ
cleos create account eosio ampercust333 EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ EOS8kUohXrXjGm2ttihXaNm8DCsHmVXfDsEBvDWSBeiC2m8tCbAQQ

# deploy contracts
cleos set contract amperioutokn ./ioutoken/
cleos set contract amperslvtokn ./slvrtoken/
cleos set contract amperdrstokn ./drtoken/

# create tokens
cleos push action amperioutokn create '{"issuer":"amperioutokn","new_supply":"1000000.0000 IOU","transfer_locked":true}' -p amperioutokn 
cleos push action amperslvtokn create '{"issuer":"amperslvtokn","new_supply":"1000000.0000 SLVR","slvr_per_token_mg":2000,"transfer_locked":true,"redeem_locked":true}' -p amperslvtokn
### note drs tokens are created with amperslvtokn as issuer
cleos push action amperdrstokn create '{"issuer":"amperslvtokn","new_supply":"1000000.0000 ADR","transfer_locked":true}' -p amperdrstokn

# view stats table
cleos get table amperioutokn 4,IOU stats
cleos get table amperslvtokn 4,SLVR stats
cleos get table amperdrstokn 4,ADR stats

# setup eosio.code permission to amperslvtokn contract
cleos set account permission amperslvcont active '{ "threshold": 1, "keys": [ { "key": "EOS5CMwziTNPnn3g2X7VkRKT77tecsushPLz1rCBDCVRjTucZDk2T", "weight": 1 } ], "accounts": [ { "permission": { "actor": "amperslvcont", "permission": "eosio.code" }, "weight": 1 } ] }'

# attempt to issue more tokens than created
cleos push action amperioutokn issue '{"to":"amperioutokn","quantity":"1000009.0000 IOU","memo":"issue"}'  -p amperioutokn
cleos push action amperslvtokn issue '{"to":"amperslvtokn","quantity":"1000009.0000 SLVR","memo":"issue"}'  -p amperslvtokn
cleos push action amperdrstokn issue '{"to":"amperdrstokn","quantity":"1000009.0000 ADR","memo":"issue"}'  -p amperdrstokn

# issue iou tokens
cleos push action amperioutokn issue '{"to":"ampercust111","quantity":"20.0000 IOU","memo":"issue"}'  -p amperioutokn

# attempt to issue SLVR tokens to an account NOT having IOU tokens
cleos push action amperslvtokn issue '{"to":"ampercust222","quantity":"10.0000 SLVR","memo":"issue"}'  -p amperslvtokn

# issue slvr tokens
cleos push action amperslvtokn issue '{"to":"ampercust111","quantity":"10.0000 SLVR","memo":"issue"}'  -p amperslvtokn

# check iou and slvr token balances
cleos get currency balance amperioutokn cust111
cleos get currency balance amperslvtokn cust111

# attempt to transfer tokens without unlocking
cleos push action amperioutokn transfer '{"from":"ampercust111","to":"ampercust222","quantity":"10.0000 IOU","memo":"Tx"}' -p ampercust111
cleos push action amperslvtokn transfer '{"from":"ampercust111","to":"ampercust222","quantity":"10.0000 SLVR","memo":"Tx"}' -p ampercust111

# unlock transfer action for slvr token
cleos push action amperslvtokn unlock '{"unlock":"1000000.0000 SLVR"}'  -p amperslvtokn

# transfer slvr tokens
cleos push action amperslvtokn transfer '{"from":"ampercust111","to":"ampercust222","quantity":"5.0000 SLVR","memo":"Tx"}' -p ampercust111

# attempt to redeem slvr token
cleos push action amperslvtokn redeem '{"owner":"ampercust111","quantity":"5.0000 SLVR"}' -p ampercust111



