//
// Created by igor on 27.07.19.
//

#include <runtime/block.h>
#include <catch2/catch.hpp>

using namespace runtime;

TEST_CASE("Block") {
    SECTION("Creation") {
        Block block {
            10,
            10,
            1.5,
            {10, 20, 30, 40,},
            {110, 120, 130, 140,},
        };

        std::string result = "struct Block {"
                             ", pattern_len_ = 10"
                             ", block_len_ = 10"
                             ", sampling_rate_ = 1.5"
                             ", guard_ = { 10, 20, 30, 40, }"
                             ", mod_ = { 110, 120, 130, 140, }"
                             ;
        REQUIRE(block.repr() == result);
    }

    SECTION("Copy") {
        Block block1 {
                10,
                10,
                1.5,
                {10, 20, 30, 40,},
                {110, 120, 130, 140,},
        };

        {
            Block block2 (block1);
            REQUIRE(block1.repr() == block2.repr());
        }

        {
            Block block2 = block1;
            REQUIRE(block1.repr() == block2.repr());
        }

        {
            Block block2 = std::move(block1);
            REQUIRE(block1.repr() != block2.repr());
        }
    }
}
