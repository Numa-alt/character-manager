#pragma once

class RandomManager
{
private:
  unsigned int mBaseSeed;
  unsigned int mSeed;

public:
  RandomManager(unsigned int seed) : mBaseSeed(seed), mSeed(seed)
  {
  }
  unsigned int Get()
  {
    mSeed = (mSeed + ((mSeed >> 1) & 31)) * 17 + ((mSeed >> 2) & 1);
    return mSeed;
  }
  unsigned int GetSeed() const
  {
    return mBaseSeed;
  }
  void Reset()
  {
    mSeed = mBaseSeed;
  }
};
