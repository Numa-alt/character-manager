#pragma once

class RandomManager
{
private:
  unsigned int mBaseSeed;
  unsigned int mSeed;
  unsigned int mAdd;
  unsigned int mCount;

public:
  RandomManager(unsigned int seed) : mBaseSeed(seed), mSeed(seed), mAdd(seed), mCount(0)
  {
  }
  unsigned int Get()
  {
    //mSeed = (mSeed + ((mSeed >> 1) & 31)) * ((mSeed) & 15);
    mSeed = ( mSeed + 1 ) * ( ( mAdd & 0xFFFF ) + 13 );
    mAdd = mAdd * 123; 
    mCount++;
    return mSeed + ( ( mAdd >> ( mCount & 0x1f ) ) & 1 );
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
