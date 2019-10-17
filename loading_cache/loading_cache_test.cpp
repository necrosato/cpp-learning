//
// loading_cache_test.cpp
// Naookie Sato
//
// Created by Naookie Sato on 09/24/2019
// Copyright © 2018 Naookie Sato. All rights reserved.
//

#include <cassert>
#include <iostream>
#include <string>
#include <memory>
#include <utility>
#include "loading_cache.h"

struct TestData {
  TestData(std::string s) : data(s) {}
  std::string data;
};

void LoadingCacheCopyableDataTest() {
  auto cache = nsato::LoadingCache<int,std::string>::CreateKeepForever();
  int loaded = 0;
  cache->Set(1, [&]() {
        ++loaded;
        return "asdf";
      });
  assert(loaded == 0);
  assert(cache->Load(1) == "asdf");
  assert(loaded == 1);
  assert(cache->Load(1) == "asdf");
  assert(loaded == 1);
}

void LoadingCacheUniqueDataTest() {
  auto cache = nsato::LoadingCache<int,std::unique_ptr<TestData>>::CreateKeepForever();
  int loaded = 0;
  cache->Set(1, [&]() {
        ++loaded;
        return std::make_unique<TestData>("asdf");
      });
  assert(loaded == 0);
  assert(cache->Load(1)->data == "asdf");
  assert(loaded == 1);
  assert(cache->Load(1)->data == "asdf");
  assert(loaded == 1);
}

void LoadingCacheKeepNeverTest() {
  auto cache = nsato::LoadingCache<int,std::unique_ptr<TestData>>::CreateKeepNever();
  int loaded = 0;
  cache->Set(1, [&]() {
        ++loaded;
        return std::make_unique<TestData>("asdf");
      });
  assert(loaded == 0);
  assert(cache->Load(1)->data == "asdf");
  assert(loaded == 1);
  assert(cache->Load(1)->data == "asdf");
  assert(loaded == 2);
}

void LoadingCacheLRUTest() {
  auto cache = nsato::LoadingCache<int, std::unique_ptr<TestData>>::CreateLeastRecentlyUsed(2);
  int loaded = 0;
  cache->Set(1, [&]() {
        ++loaded;
        return std::make_unique<TestData>("asdf");
      });
  cache->Set(2, [&]() {
        ++loaded;
        return std::make_unique<TestData>("fdsa");
      });
  cache->Set(3, [&]() {
        ++loaded;
        return std::make_unique<TestData>("1234");
      });
  assert(loaded == 0);
  assert(cache->Load(1)->data == "asdf");
  assert(loaded == 1);
  assert(cache->Load(1)->data == "asdf");
  assert(loaded == 1);
  assert(cache->Load(2)->data == "fdsa");
  assert(loaded == 2);
  assert(cache->Load(2)->data == "fdsa");
  assert(loaded == 2);
  assert(cache->Load(1)->data == "asdf");
  assert(loaded == 2);
  assert(cache->Load(3)->data == "1234");
  assert(loaded == 3);
  assert(cache->Load(3)->data == "1234");
  assert(loaded == 3);
  assert(cache->Load(1)->data == "asdf");
  assert(loaded == 3);
  assert(cache->Load(2)->data == "fdsa");
  assert(loaded == 4);
}

int main(int argc, char* argv[]) {
  LoadingCacheCopyableDataTest();
  LoadingCacheUniqueDataTest();
  LoadingCacheKeepNeverTest();
  LoadingCacheLRUTest();
  return 0;
}
