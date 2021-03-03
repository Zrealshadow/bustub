//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer.cpp
//
// Identification: src/buffer/clock_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/clock_replacer.h"
#include "include/common/logger.h"
namespace bustub {

ClockReplacer::ClockReplacer(size_t num_pages) {
    this->num_pages = num_pages;
    for(size_t i = 0; i < num_pages; i++){
        unit_frame * unit = new unit_frame(i,false, true);
        this->clock_array.push_back(unit);
        // LOG_DEBUG("frame %zu", i);
    }
    this->clock_hand = 0;
}

ClockReplacer::~ClockReplacer() = default;

bool ClockReplacer::Victim(frame_id_t *frame_id) { 
    size_t count = 0;
    size_t index;
    bool pin = true;
    while(true){
        // all frames are being used, no spare frame. Return false
        if(count+1 == num_pages and pin){
            return false;
        }
        index = (count + clock_hand) % num_pages;
        unit_frame * unit = clock_array.at(index);
        // LOG_INFO("Old: unit frame id %d, status PIN/REF : %d/%d", unit->frame_id, unit->pin, unit->ref);
        pin = unit->pin and pin;
        if(unit->pin){
            // if the frame is used, skip to next frame
            pin = pin and unit->pin;
            count += 1;
            // LOG_DEBUG("New 1: unit frame id %d, status PIN/REF : %d/%d", unit->frame_id, unit->pin, unit->ref);
        }
        else if(unit->ref){
            // if the frame is not used now, but used recently
            // set ref to false and skip to next frame
            unit->ref = false;
            count += 1;
            // LOG_DEBUG("New 2: unit frame id %d, status PIN/REF : %d/%d", unit->frame_id, unit->pin, unit->ref);
        }
        else{
            // if the frame is not used now and not used recently
            // the frame is what we want to change.
            // reset the new frame
            *frame_id = unit->frame_id;
            clock_hand = unit->frame_id;
            unit->pin = true;
            unit->ref = false;
            return true;
        }
        
    }
}

void ClockReplacer::Pin(frame_id_t frame_id) {
    unit_frame * unit = clock_array.at(frame_id);
    unit->pin = true;
}

void ClockReplacer::Unpin(frame_id_t frame_id) {
    unit_frame * unit = clock_array.at(frame_id);
    unit->pin = false;
    unit->ref = true;
}

size_t ClockReplacer::Size() { 
    std::vector<unit_frame *>::iterator iter =  clock_array.begin();
    size_t ans = 0;
    for(; iter != clock_array.end(); iter++){
        unit_frame * unit = *iter;
        if(unit->pin == false){
            ans += 1;
        }
    }
    return ans;
}

}  // namespace bustub
