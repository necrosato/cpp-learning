//
// loading_cache.h
// Naookie Sato
//
// Created by Naookie Sato on 09/24/2019
// Copyright © 2018 Naookie Sato. All rights reserved.
//

#ifndef _HOME_NSATO_DOCUMENTS_FW_WEEKLY_TRAINING_2019_09_27_LOADING_CACHE_LOADING_CACHE_H_
#define _HOME_NSATO_DOCUMENTS_FW_WEEKLY_TRAINING_2019_09_27_LOADING_CACHE_LOADING_CACHE_H_

#include <functional>
#include <map>
#include <memory>
#include <utility>

namespace nsato {

template <class K, class V>
class LoadingCache {
 public:
  LoadingCache() = delete;
  virtual ~LoadingCache() = default;
  LoadingCache(const LoadingCache& other) = delete;
  LoadingCache& operator=(const LoadingCache& other) = delete;

  /**
   * This class will have logic to evict from the data_
   */
  class EvictionPolicy;

  /**
   * Constructor
   */
  LoadingCache(std::unique_ptr<EvictionPolicy> ep) : ep_(std::move(ep)) {}

  /**
   * Static create functions to create a loading cache with specific policies
   */
  static std::shared_ptr<LoadingCache<K, V>> CreateKeepForever();

  static std::shared_ptr<LoadingCache<K, V>> CreateKeepNever();

  static std::shared_ptr<LoadingCache<K, V>> CreateLeastRecentlyUsed(int n);

  void Set(const K& key, std::function<V()> loader);

  const V& Load(const K& key);

 private:
  std::map<K, V> data_;
  std::map<K, std::function<V()>> loading_closures_;
  std::unique_ptr<EvictionPolicy> ep_;
};

/**
 * Base EvictionPolicy class
 */
template <class K, class V>
class LoadingCache<K, V>::EvictionPolicy {
 public:
  EvictionPolicy() = default;
  virtual ~EvictionPolicy() = default;
  EvictionPolicy(const EvictionPolicy& other) = delete;
  EvictionPolicy& operator=(const EvictionPolicy& other) = delete;

  /**
   * This will possibly evict a value from a map to make space to load new value for key according to policy.
   * All subclasses must implement.
   */
  virtual void Evict(const K& key, std::map<K, V>* cache) = 0;
};

/**
 * Eviction policy which never evicts
 */
template <class K, class V>
class KeepForeverEvictionPolicy : public LoadingCache<K, V>::EvictionPolicy {
 public:
  KeepForeverEvictionPolicy() = default;
  virtual ~KeepForeverEvictionPolicy() = default;
  KeepForeverEvictionPolicy(const KeepForeverEvictionPolicy& other) = delete;
  KeepForeverEvictionPolicy& operator=(const KeepForeverEvictionPolicy& other) = delete;

  void Evict(const K& key, std::map<K, V>* data) override {
    return;
  }
};

/**
 * Eviction policy which always evicts
 */
template <class K, class V>
class KeepNeverEvictionPolicy : public LoadingCache<K, V>::EvictionPolicy {
 public:
  KeepNeverEvictionPolicy() = default;
  virtual ~KeepNeverEvictionPolicy() = default;
  KeepNeverEvictionPolicy(const KeepNeverEvictionPolicy& other) = delete;
  KeepNeverEvictionPolicy& operator=(const KeepNeverEvictionPolicy& other) = delete;

  void Evict(const K& key, std::map<K, V>* data) override {
    data->clear();
  }
};

/**
 * Eviction policy which keeps the n least recently used values
 */
template <class K, class V>
class LeastRecentlyUsedEvictionPolicy : public LoadingCache<K, V>::EvictionPolicy {
  public:
   LeastRecentlyUsedEvictionPolicy() = delete;
   virtual ~LeastRecentlyUsedEvictionPolicy() = default;
   LeastRecentlyUsedEvictionPolicy(const LeastRecentlyUsedEvictionPolicy& other) = delete;
   LeastRecentlyUsedEvictionPolicy& operator=(const LeastRecentlyUsedEvictionPolicy& other) = delete;

   /**
    * n: max number of elements to keep cached at any given time
    */
   LeastRecentlyUsedEvictionPolicy(int n) : n_(n) {}

   void Evict(const K& key, std::map<K, V>* data) override {
     // if already loaded, no need to evict. Just remove old counter_to_key_ entry
     if (key_to_counter_.find(key) != key_to_counter_.end()) {
       counter_to_key_.erase(key_to_counter_.at(key));
       // Else if not loaded but cache full, need to evict from both maps
     } else if (key_to_counter_.size() == n_) {
       K to_evict = counter_to_key_.begin()->second;
       data->erase(to_evict);
       key_to_counter_.erase(to_evict);
       counter_to_key_.erase(counter_to_key_.begin());
     }
     // Else don't need to evict
     key_to_counter_[key] = counter_;
     counter_to_key_[counter_] = key;
     assert(key_to_counter_.size() == counter_to_key_.size());
     assert(key_to_counter_.size() <= n_);
     ++counter_;
   }

 private:
  int n_;
  // Incremented each time evict is called
  int counter_;
  std::map<K, int> key_to_counter_;
  std::map<int, K> counter_to_key_;
};

//////////////////////////////
// TEMPLATE IMPLEMENTATIONS //
//////////////////////////////

template <class K, class V>
void LoadingCache<K,V>::Set(const K& key, std::function<V()> loader) {
  loading_closures_[key] = loader;
}

template <class K, class V>
const V& LoadingCache<K,V>::Load(const K& key) {
  ep_->Evict(key, &data_);
  if (data_.find(key) == data_.end()) {
    data_.emplace(key, loading_closures_[key]());
  }
  return data_.at(key);
}

template <class K, class V>
std::shared_ptr<LoadingCache<K, V>> LoadingCache<K, V>::CreateKeepForever() {
  return std::make_unique<LoadingCache<K, V>>(std::make_unique<KeepForeverEvictionPolicy<K, V>>());
}

template <class K, class V>
std::shared_ptr<LoadingCache<K, V>> LoadingCache<K, V>::CreateKeepNever() {
  return std::make_unique<LoadingCache<K, V>>(std::make_unique<KeepNeverEvictionPolicy<K, V>>());
}

template <class K, class V>
std::shared_ptr<LoadingCache<K, V>> LoadingCache<K, V>::CreateLeastRecentlyUsed(int n) {
  return std::make_unique<LoadingCache<K, V>>(std::make_unique<LeastRecentlyUsedEvictionPolicy<K, V>>(n));
}

}  // namespace nsato

#endif  // _HOME_NSATO_DOCUMENTS_FW_WEEKLY_TRAINING_2019_09_27_LOADING_CACHE_LOADING_CACHE_H_
