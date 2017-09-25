// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <mutex>

#include "gtest/gtest.h"
#include "db/Env.h"
#include "db/atomic_pointer.h"

static const int kDelayMicros = 100000;

class EnvPosixTest : public ::testing::Test { };

static void SetBool(void* ptr) {
  reinterpret_cast<releveldb::AtomicPointer*>(ptr)->NoBarrier_Store(ptr);
}

TEST(EnvPosixTest, RunImmediately) {
  releveldb::AtomicPointer called (NULL);
  releveldb::Env::Default()->Schedule(&SetBool, &called);
  releveldb::Env::Default()->SleepForMicroseconds(kDelayMicros);
  ASSERT_TRUE(called.NoBarrier_Load() != NULL);
}

TEST(EnvPosixTest, RunMany) {
  releveldb::AtomicPointer last_id (NULL);

  struct CB {
    releveldb::AtomicPointer* last_id_ptr;   // Pointer to shared slot
    uintptr_t id;             // Order# for the execution of this callback

    CB(releveldb::AtomicPointer* p, int i) : last_id_ptr(p), id(i) { }

    static void Run(void* v) {
      CB* cb = reinterpret_cast<CB*>(v);
      void* cur = cb->last_id_ptr->NoBarrier_Load();
      ASSERT_EQ(cb->id-1, reinterpret_cast<uintptr_t>(cur));
      cb->last_id_ptr->Release_Store(reinterpret_cast<void*>(cb->id));
    }
  };

  // Schedule in different order than start time
  CB cb1(&last_id, 1);
  CB cb2(&last_id, 2);
  CB cb3(&last_id, 3);
  CB cb4(&last_id, 4);
  releveldb::Env::Default()->Schedule(&CB::Run, &cb1);
  releveldb::Env::Default()->Schedule(&CB::Run, &cb2);
  releveldb::Env::Default()->Schedule(&CB::Run, &cb3);
  releveldb::Env::Default()->Schedule(&CB::Run, &cb4);

  releveldb::Env::Default()->SleepForMicroseconds(kDelayMicros);
  void* cur = last_id.Acquire_Load();
  ASSERT_EQ(4, reinterpret_cast<uintptr_t>(cur));
}

struct State {
  std::mutex mu;
  int val;
  int num_running;
};

static void ThreadBody(void* arg) {
  State* s = reinterpret_cast<State*>(arg);
  s->mu.lock();
  s->val += 1;
  s->num_running -= 1;
  s->mu.unlock();
}

TEST(EnvPosixTest, StartThread) {
  State state;
  state.val = 0;
  state.num_running = 3;
  for (int i = 0; i < 3; i++) {
    releveldb::Env::Default()->StartThread(&ThreadBody, &state);
  }
  while (true) {
    state.mu.lock();
    int num = state.num_running;
    state.mu.unlock();
    if (num == 0) {
      break;
    }
    releveldb::Env::Default()->SleepForMicroseconds(kDelayMicros);
  }
  ASSERT_EQ(state.val, 3);
}
