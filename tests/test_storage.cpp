#include <utility>
#include <sstream>
#include <catch2/catch.hpp>
#include "json/single_include/nlohmann/json.hpp"

#include "common/utils.h"
#include "common/timestamp.h"

#include "storage/frame.h"
#include "storage/storage.h"
#include "storage/signal.h"

using nlohmann::json;
using namespace storage;


bool isEqual(storage::SignalData const &sd1, storage::SignalData const &sd2) {
    if (sd1.size() != sd2.size())
        return false;
    bool same = true;
    for (size_t i = 0; i < sd1.size(); i++) {
        same = same && common::approxEqual(sd1[i], sd2[i]);
    }
    return same;
}


TEST_CASE("Signal") {
    const int len = 10;
    SignalValue values[len] = {1.0, 2.0, 1.0, 2.0, 1.0, 2.0, 1.0, 2.0, 1.0, 2.0};
    std::string valid_repr = "[1.0,2.0,1.0,2.0,1.0,2.0,1.0,2.0,1.0,2.0]";

    SECTION("Creation") {
        SignalData s0;
        REQUIRE(s0.size() == 0);

        SignalData s1(values, len);
        REQUIRE(s1.size() == len);

        SignalData s2(s1);
        REQUIRE(s2.size() == len);
        REQUIRE(s1.size() == len);

        SignalData s3(std::move(s1));
        REQUIRE(s3.size() == len);
        REQUIRE(s2.size() == len);
        REQUIRE(s1.size() == 0);

        SignalData s4(values, len / 2, 2);
        REQUIRE(s4.size() == len / 2);
        REQUIRE(s4[0] == 1.);
        REQUIRE(s4[1] == 1.);

        SignalData s5(values + 1, len / 2, 2);
        REQUIRE(s5.size() == len / 2);
        REQUIRE(s5[0] == 2.);
        REQUIRE(s5[1] == 2.);

    }
    SECTION("Assignment") {
        SignalData s0;
        SignalData s1(values, len);

        s0 = s1;
        REQUIRE(s0.size() == len);
        REQUIRE(s1.size() == len);

        SignalData s2;
        s2 = std::move(s0);
        REQUIRE(s0.size() == 0);
        REQUIRE(s1.size() == len);
        REQUIRE(s2.size() == len);
    }
    SECTION("IO") {
        SignalData s0(values, len);
        std::stringstream ss;
        ss << s0;
        REQUIRE(ss.str() == valid_repr);
        REQUIRE(s0.repr() == valid_repr);
        ss << s0;
        SignalData s1;
        REQUIRE(s1.size() == 0);
        ss >> s1;
        REQUIRE(s1.size() == len);
        ss.str("");
        ss << s1;
        REQUIRE(ss.str() == valid_repr);
        REQUIRE(s1.repr() == valid_repr);

        s0 = SignalData();
        ss.str("");
        ss << s0;
        ss >> s1;
        REQUIRE(s1.size() == 0);
    }
    SECTION("Operations") {
        int part_len = len / 2 - 1;
        SignalData s0(values, len);
        SignalData s1 = s0.detachBack(part_len);
        REQUIRE(s1.size() == part_len);
        REQUIRE(s0.size() == (len - part_len));
        SignalData s2(s1);
        s2.attachBack(s1);
        REQUIRE(s2.size() == 2 * part_len);
        REQUIRE(s1.size() == part_len);

        SignalData s3 = s1.detachBack(part_len);
        REQUIRE(s1.size() == 0);
        REQUIRE(s3.size() == part_len);
        s0 = s3.detachBack(0);
        REQUIRE(s0.size() == 0);
        s3.attachBack(s0);
        REQUIRE(s3.size() == part_len);
        REQUIRE(s0.size() == 0);

        REQUIRE(s3[0] == 1.);
        REQUIRE(s3[part_len - 1] == 2.);
    }
}

TEST_CASE("Frame") {
    common::TimeStamp ts0(0, 0);
    common::TimeStamp ts1(10, 10);
    common::TimeStamp ts2(10, 20);
    common::TimeStamp ts3(10, 30);

    const int len = 5;
    SignalValue values[len] = {1., 2., 3., 4., 5.};
    SignalValue values2[len] = {42., 42., 42., 42., 42.};
    SignalValue values3[len + 1] = {42., 42., 42., 42., 42., 42.};

    SignalData sd(values, len);
    SignalData sd2(values2, len);
    SignalData sd3(values3, len + 1);

    common::SignalKey k1 = common::SignalKey::Uhtr;
    common::SignalKey k2 = common::SignalKey::Umod;
    SECTION("Creation") {
        Frame f1(ts1);
        REQUIRE(f1.getTs() == ts1);
        REQUIRE(f1.size() == 0);
    }
    SECTION("Subscription") {
        Frame f1(ts1);
        f1[k1] = sd;
        REQUIRE(f1.size() == sd.size());
        SignalData v = f1[k1];
        REQUIRE(isEqual(v, sd));
        SignalData const &lv = f1[k1];
        REQUIRE(isEqual(v, lv));
        REQUIRE_THROWS(v = f1[k2]);
        SignalData sd_half = sd.detachBack(len / 2);
        //size mismatch
        REQUIRE_THROWS(f1[k2] = sd_half);
    }
    SECTION("Keys interface") {
        Frame f1(ts1);
        REQUIRE(f1.keys().size() == 0);
        f1[k1] = sd;
        REQUIRE(f1.hasKey(k1));
        REQUIRE_FALSE(f1.hasKey(k2));
        REQUIRE(f1.keys().size() == 1);
        REQUIRE(f1.keys()[0] == k1);
        REQUIRE(isEqual(f1[k1], sd));

        f1[k1] = sd2;
        REQUIRE(isEqual(f1[k1], sd2));
        REQUIRE(sd2.size() != sd3.size());
        REQUIRE_THROWS(f1[k2] = sd3);

        bool deleted = f1.delKey(k2);
        REQUIRE_FALSE(deleted);
        deleted = f1.delKey(k1);
        REQUIRE(deleted);
        REQUIRE(f1.size() == 0);
        REQUIRE(f1.keys().size() == 0);
        f1[k2] = SignalData(values3, len + 1);
        REQUIRE(f1.size() == (len + 1));
        Frame const &fref = f1;
        REQUIRE(fref[k2].size() == len + 1);
        //no such key
        REQUIRE_THROWS(fref[k1].size());
    }
    SECTION("Movement") {
        Frame f1(ts1);
        f1[k1] = sd;
        Frame f2(std::move(f1));
        REQUIRE(f1.getTs() == ts0);
        REQUIRE(f2.getTs() == ts1);
        REQUIRE(f1.size() == 0);
        REQUIRE(f2.size() == sd.size());
        REQUIRE_FALSE(f1.hasKey(k1));
        REQUIRE(f2.hasKey(k1));

        Frame f3(ts0);
        f3 = std::move(f2);
        REQUIRE(f3.getTs() == ts1);
        REQUIRE(f3.size() == sd.size());
        REQUIRE(f3.hasKey(k1));
        REQUIRE(f3.hasKey(k1));
    }
    SECTION("Attachment") {
        Frame f1(ts2);
        Frame f2(ts1);
        Frame f3(ts2);
        f1[k1] = sd;
        f2[k1] = sd;

        //attached ts must be equals or higher than origin ts
        REQUIRE_FALSE(f1.attachBack(f2));
        //frame keys must be the same
        REQUIRE_FALSE(f1.attachBack(f3));
        REQUIRE(f1.size() == sd.size());
        f3[k1] = sd;
        REQUIRE(f1.attachBack(f3));
        REQUIRE(f1.size() == 2 * sd.size());
        f3[k2] = sd;
        REQUIRE_FALSE(f1.attachBack(f3));
        REQUIRE(f1.attachBack(f2, true));
        f2.setTs(ts3);
        REQUIRE(f1.attachBack(f2));
        REQUIRE(f1.size() == 4 * sd.size());
    }
    SECTION("Detachment") {
        Frame f1(ts1);
        REQUIRE_THROWS(f1.detachBack(1));
        f1[k1] = sd;
        Frame f2 = f1.detachBack(1);
        REQUIRE(f2.size() == 1);
        REQUIRE(f2.keys() == f1.keys());
        REQUIRE(f1.size() == (len - 1));
        REQUIRE(f2.getTs() > f1.getTs());
        Frame f3 = f1.detachBack(0);
        REQUIRE(f3.size() == 0);
        REQUIRE(f1.keys() == f3.keys());
        REQUIRE(f1.size() == (len - 1));
    }
    SECTION("Representation") {
        Frame f1(ts1);
        f1[k1] = sd;
        f1[k2] = sd2;
        std::string valid_repr = "{\"Timestamp\":[10,10],\"Uhtr\":[1.0,2.0,3.0,4.0,5.0],\"Umod\":[42.0,42.0,42.0,42.0,42.0]}";
        REQUIRE(f1.repr() == valid_repr);
        std::stringstream ss;
        ss << f1;
        REQUIRE(ss.str() == valid_repr);
        json j = f1;
        REQUIRE(j.size() == (2 + 1));//two signals and timestamp
    }
}

Frame buildTestFrame(common::TimeStamp ts, int len) {
    common::SignalKey k1 = common::SignalKey::Uhtr;
    std::vector<SignalValue> values;
    for (size_t i = 0; i < len; ++i)
        values.push_back(1);
    SignalData sd(&*values.begin(), len);
    Frame f(ts);
    f[k1] = sd;
    return std::move(f);
}

TEST_CASE("Storage") {
    const int len = 5;
    common::TimeStamp ts1(10, 10);
    common::TimeStamp ts2(10, 15);
    common::TimeStamp ts3(10, 25);

    SECTION("Creation") {
        Storage st(len);
        REQUIRE(st.getFrameSize() == len);
        REQUIRE(st.empty());
        REQUIRE(st.size() == 0);
        std::string hash = st.getHash();
        REQUIRE(st.getHash() == hash);
    }
    SECTION("Append") {
        Frame f1 = buildTestFrame(ts1, len);
        Frame f2 = buildTestFrame(ts2, len);
        Frame f3 = buildTestFrame(ts3, 2 * len);
        Frame f4 = buildTestFrame(ts1, 2 * len);

        //Storage where frame must be TimeStamp sorted to append
        Storage st(2 * len, false);

        bool status = false;
        // appended in head
        status = st.append(std::move(f1));
        REQUIRE(status);
        REQUIRE(st.size() == 0);
        status = st.append(std::move(f2));
        REQUIRE(status);
        REQUIRE(st.size() == 1);
        status = st.append(std::move(f3));
        REQUIRE(status);
        REQUIRE(st.size() == 2);
        status = st.append(std::move(f4));
        REQUIRE_FALSE(status);
        REQUIRE(f4.size());
        REQUIRE(st.size() == 2);

        Storage st1(len);
        status = st1.append(std::move(f4));
        REQUIRE(status);
        REQUIRE(st1.size() == 2);
    }
    SECTION("Subscript") {
        size_t num = 2;
        common::TimeStamp ts_l0(10, 0);
        common::TimeStamp ts_l1(10, (num - 1) * len);
        Frame f1 = buildTestFrame(ts_l0, num * len + 1);
        Storage st(len);
        st.append(std::move(f1));
        REQUIRE(st.size() == 2);
        REQUIRE(st[0].size() == len);
        REQUIRE(st[num - 1].size() == len);
        REQUIRE(st[num - 1].getTs() == ts_l1);
    }
    SECTION("Reset, finalize, hash") {
        Storage st(2 * len);
        Frame f1 = buildTestFrame(ts1, 3 * len);
        st.append(std::move(f1));
        REQUIRE(st.size() == 1);
        std::string hash = st.getHash();
        REQUIRE(st.getHash() == hash);
        st.finalize();
        REQUIRE(st.size() == 2);
        st.finalize();
        Frame f2 = buildTestFrame(ts1, len);
        REQUIRE(st.size() == 2);
        st.append(std::move(f2));
        REQUIRE(st.size() == 2);
        REQUIRE(st.getHash() == hash);
        st.reset();
        REQUIRE(st.getHash() != hash);
        REQUIRE(st.empty());
        st.finalize();
        REQUIRE(st.empty());
    }
    SECTION("Iteration") {
        size_t num = 10;
        common::TimeStamp ts_l0(10, 0);
        // last frame has len*(num-1) ts, previous - len*(num - 2)
        common::TimeStamp ts_l1(10, len * (num - 2));

        Storage st(len);
        Frame f1 = buildTestFrame(ts_l0, num * len);
        st.append(std::move(f1));
        REQUIRE(st.size() == num);
        size_t counter = 0;
        for (auto it = st.begin(); it != st.end(); ++it) {
            counter++;
        }
        REQUIRE(counter == num);
        counter = 0;
        common::TimeStamp tt = st.afterTs(ts_l1)->getTs();
        for (auto it = st.afterTs(ts_l1); it != st.end(); ++it)
            counter++;
        REQUIRE(counter == 1);
    }
}