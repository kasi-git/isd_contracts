cmake_minimum_required(VERSION 3.5)
project(ampertokens VERSION 1.0.0)

find_package(eosio.cdt)

add_contract( slvrtoken slvrtoken slvrtoken.cpp )

