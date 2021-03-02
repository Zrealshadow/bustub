//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/buffer_pool_manager.h"

#include <list>
#include <unordered_map>

namespace bustub {

BufferPoolManager::BufferPoolManager(size_t pool_size, DiskManager *disk_manager, LogManager *log_manager)
    : pool_size_(pool_size), disk_manager_(disk_manager), log_manager_(log_manager) {
  // We allocate a consecutive memory space for the buffer pool.
  pages_ = new Page[pool_size_];
  replacer_ = new LRUReplacer(pool_size);

  // Initially, every page is in the free list.
  for (size_t i = 0; i < pool_size_; ++i) {
    free_list_.emplace_back(static_cast<int>(i));
  }
}

BufferPoolManager::~BufferPoolManager() {
  delete[] pages_;
  delete replacer_;
}

Page *BufferPoolManager::FetchPageImpl(page_id_t page_id) {
  // 1.     Search the page table for the requested page (P).
  // 1.1    If P exists, pin it and return it immediately.
  // 1.2    If P does not exist, find a replacement page (R) from either the free list or the replacer.
  //        Note that pages are always found from the free list first.
  // 2.     If R is dirty, write it back to the disk.
  // 3.     Delete R from the page table and insert P.
  // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.
  auto got = page_table_.find(page_id);
  Page * pages = GetPages();
  frame_id_t frame_id;
  if (got != page_table_.end()){
    // P exists
    frame_id = page_table_.at(page_id);
    replacer_->Pin(frame_id);
    return pages+frame_id;
  }
  else if (!free_list_.empty()){
      //free_list is not empty, fetch one frame from freeList
      // I choose to get the first frame
      frame_id = free_list_.front();
      free_list_.pop_front();
      // load in page_table
      page_table_.insert(page_id, frame_id);
      //Readpage from disk to memory of database
      disk_manager_->ReadPage(page_id, pages[frame_id].GetData()); 
      return pages + frame_id; 
  }
  else if (replacer_->Victim(&frame_id)){
    // no free frame, replace one frame from buffer
    // if it is possible to replace frame, frame_id is updated.
    Page * replaced_page = pages + frame_id;
    page_id_t replaced_page_id = replaced_page->GetPageId();
    if(replaced_page->IsDirty()){
      // if the data is dirty, we should write it to disk and keep data in sync
      disk_manager_->WritePage(replaced_page_id, replaced_page->GetData());
    }
    // if the data is sync in disk and memory
    // there is no need to write it to disk.
    page_table_.erase(replaced_page_id);
    // update page table
    page_table_.insert(page_id, frame_id);
    disk_manager_->ReadPage(page_id, pages[frame_id].GetData());
    return pages + frame_id;
  }
  else{
    // no replaced page.
    return nullptr;
  }
}

bool BufferPoolManager::UnpinPageImpl(page_id_t page_id, bool is_dirty) { 
  return false; 
}

bool BufferPoolManager::FlushPageImpl(page_id_t page_id) {
  // Make sure you call DiskManager::WritePage!
  return false;
}

Page *BufferPoolManager::NewPageImpl(page_id_t *page_id) {
  // 0.   Make sure you call DiskManager::AllocatePage!
  // 1.   If all the pages in the buffer pool are pinned, return nullptr.
  // 2.   Pick a victim page P from either the free list or the replacer. Always pick from the free list first.
  // 3.   Update P's metadata, zero out memory and add P to the page table.
  // 4.   Set the page ID output parameter. Return a pointer to P.
  frame_id_t frame_id;
  Page* pages = GetPages();
  if (!free_list_.empty()){
    frame_id = free_list_.front();
    free_list_.pop_front();
  }
  else if (replacer_->Victim(&frame_id)){
    Page * replaced_page = pages + frame_id;
    page_id_t replaced_page_id = replaced_page->GetPageId();
    if (replaced_page->IsDirty()){
      disk_manager_->WritePage(replaced_page_id, replaced_page->GetData());
    }
    page_table_.erase(replaced_page_id);
  } else {
    return nullptr;
  }

  page_id_t pid = disk_manager_->AllocatePage();
  // set value
  *page_id = pid;
  //update buffer bool
  page_table_.insert(pid,frame_id);
  disk_manager_->ReadPage(pid, pages[frame_id].GetData());
  return pages + frame_id;
}

bool BufferPoolManager::DeletePageImpl(page_id_t page_id) {
  // 0.   Make sure you call DiskManager::DeallocatePage!
  // 1.   Search the page table for the requested page (P).
  // 1.   If P does not exist, return true.
  // 2.   If P exists, but has a non-zero pin-count, return false. Someone is using the page.
  // 3.   Otherwise, P can be deleted. Remove P from the page table, reset its metadata and return it to the free list.
  return false;
}

void BufferPoolManager::FlushAllPagesImpl() {
  // You can do it!
}

}  // namespace bustub
