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
  EvictionPolicy& operator&(const EvictionPolicy& other) = delete;

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
  KeepForeverEvictionPolicy& operator&(const KeepForeverEvictionPolicy& other) = delete;

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
  KeepNeverEvictionPolicy& operator&(const KeepNeverEvictionPolicy& other) = delete;

  void Evict(const K& key, std::map<K, V>* data) override {
    data->clear();
    return;
  }
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

}  // namespace nsato

#endif  // _HOME_NSATO_DOCUMENTS_FW_WEEKLY_TRAINING_2019_09_27_LOADING_CACHE_LOADING_CACHE_H_
