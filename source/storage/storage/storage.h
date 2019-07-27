#ifndef UCAL2_STORAGE_STORAGE_H
#define UCAL2_STORAGE_STORAGE_H

#include "common/timestamp.h"
#include "frame.h"

namespace storage {
    class Storage {
    public:
        using Array=std::vector<Frame>;
        using Iterator=Array::const_iterator;

        Storage(size_t frame_size = 10, bool enforce_ts = true);

        bool append(Frame &&f);

        bool finalize();

        Frame const &operator[](size_t ind);

        size_t size() const;

        bool empty() const;

        Iterator begin() const;

        Iterator end() const;

        Iterator afterTs(common::TimeStamp ts) const;

        std::string getHash() const;

        bool reset();

        size_t getFrameSize() const;

    private:
        Array data_;
        std::unique_ptr<Frame> head_;
        size_t const frame_size_;
        bool const enforce_ts_;
        std::string hash_;
        const size_t hash_len_ = 16;
    };
}
#endif//UCAL2_STORAGE_STORAGE_H