#pragma once


class RandomManager{
private:
  unsigned int mBaseSeed;
  unsigned int mSeed;
  
public:
  RandomManager(unsigned int seed):mBaseSeed(seed),mSeed(seed)
  {

  }
  unsigned int Get()
  {
    mSeed = ( mSeed + 1 ) * 17;
    return mSeed;
  }
  unsigned int GetSeed()const
  {
    return mBaseSeed;
  }
  void Reset()
  {
    mSeed = mBaseSeed;
  }
};


