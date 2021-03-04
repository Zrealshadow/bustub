//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.cpp
//
// Identification: src/buffer/lru_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_replacer.h"

namespace bustub {

LRUReplacer::LRUReplacer(size_t num_pages) {
    this->num_pages = num_pages;
}

LRUReplacer::~LRUReplacer() = default;

bool LRUReplacer::Victim(frame_id_t *frame_id) {
    std::lock_guard<std::mutex> lck(latch_);
    if (free_ordered_frame.size() == 0){
        return false;
    }
    *frame_id = free_ordered_frame.front();
    free_ordered_frame.pop_front();
    map_pos.erase((*frame_id));
    return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
    std::lock_guard<std::mutex> lck(latch_);
    auto got = map_pos.find(frame_id);
    if (got != map_pos.end()){
        std::list<frame_id_t>::iterator pos = got->second;
        free_ordered_frame.erase(pos);
        map_pos.erase(frame_id);
    }
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
    std::lock_guard<std::mutex> lck(latch_);
    auto got = map_pos.find(frame_id);
    if (got != map_pos.end()) {
        //frame exists in free ordered frame 
        // auto pos = free_ordered_frame.begin();
        // std::advance(pos, got->second);
        // free_ordered_frame.erase(pos);
        return;
    }
    std::list<frame_id_t>::iterator pos = free_ordered_frame.insert(free_ordered_frame.end(),frame_id);
    map_pos.insert({frame_id, pos});
}

size_t LRUReplacer::Size() { 
    std::lock_guard<std::mutex> lck(latch_);
    return free_ordered_frame.size();    
}


}  // namespace bustub
