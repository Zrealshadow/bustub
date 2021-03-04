//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer.h
//
// Identification: src/include/buffer/clock_replacer.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <list>
#include <mutex>  // NOLINT
#include <vector>
#include <unordered_map>
#include "buffer/replacer.h"
#include "common/config.h"

namespace bustub {

/**
 * ClockReplacer implements the clock replacement policy, which approximates the Least Recently Used policy.
 */
class ClockReplacer : public Replacer {
 public:
  /**
   * Create a new ClockReplacer.
   * @param num_pages the maximum number of pages the ClockReplacer will be required to store
   */
  explicit ClockReplacer(size_t num_pages);

  /**
   * Destroys the ClockReplacer.
   */
  ~ClockReplacer() override;

  
  bool Victim(frame_id_t *frame_id) override;

  void Pin(frame_id_t frame_id) override;

  void Unpin(frame_id_t frame_id) override;

  size_t Size() override;
  
 private:
  // TODO(student): implement me!
  // std::unordered_map
  struct unit_frame{
    bool ref;
    bool pin;
    frame_id_t frame_id;
    unit_frame(frame_id_t a, bool b, bool c){
      this->ref = b;
      this->pin = c;
      this->frame_id =a;
    }
  };

  // std::unordered_map<frame_id_t,unit_frame *> mapper;
  std::vector<unit_frame *> clock_array;
  size_t num_pages;
  size_t clock_hand;
  std::mutex latch_;
};

}  // namespace bustub
