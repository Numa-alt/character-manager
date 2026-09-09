#include <iostream>
#include <optional>
#include <vector>
#include <memory>
#include <utility>

#include "randomManager.h"

enum class EffectType{
    Attack      ,//攻撃
    Heal        ,//回復
};

enum class ParamType{
    Physical    ,//物理
    Magic       ,//魔法
};

enum class TargetSide{
    Enemy       ,//敵
    Friend      ,//味方
    Self        ,//自身
};

enum class TargetSelect{
    LowestHp    ,//Hpが低い
    Random      ,//ランダム
};

enum class TargetRange{
    All         ,//全体
    One         ,//単体
};

class Action{
private:
  EffectType mEffectType;
  ParamType mParamType;
  TargetSide mTargetSide;
  std::optional<TargetSelect> mTargetSelect;
  TargetRange mTargetRange;
  unsigned int mPercent;
public:
  Action(
    EffectType et, 
    ParamType pt, 
    TargetSide ts, 
    std::optional<TargetSelect> tse, 
    TargetRange tr, 
    unsigned int per):
    mEffectType(et),
    mParamType(pt),
    mTargetSide(ts),
    mTargetSelect(tse),
    mTargetRange(tr),
    mPercent(per)
    {
    }
};

class ActionTable
{
private:
  std::vector<std::unique_ptr<Action>> mTable;
public:
  ActionTable() = default;
  ActionTable( ActionTable&& a)
  {
    mTable = std::move(a.mTable);
  }
  void AddAction( std::unique_ptr<Action> action )
  {
    mTable.push_back( std::move( action ) );
  }
};

enum class ActionId{
    AttackOne       ,//単体攻撃
    AttackDouble    ,//２回攻撃
    MagicAttackOne  ,//単体魔法攻撃
    MagicAttackAll  ,//全体魔法攻撃
    HealOne         ,//単体回復
    HealAll         ,//全体回復
};

class CharacterAction{
    ActionId mId;
    int mPercent;
};

class Character{
private:
    int mHp;
    int mMaxHp;
    int mAttack;
    int mMagic;
    int mDefence;
    int mSpeed;
    std::vector<CharacterAction> mAction;
public:
    Character( int maxHp, int attack, int magic, int defence, int speed ):
        mHp(maxHp),mMaxHp(maxHp),mAttack(attack),mMagic(magic),mDefence(defence),mSpeed(speed)
        {

        }
};



// class MenuItemBase
// {
// private:
//   std::string mName;
// public:
//   MenuItemBase(const std::string& name){ mName = std::move(name); }
//   virtual ~MenuItemBase();
//   virtual void Exec();
// };

// class MenuItemMakeCharacter::MenuItemBase{
//   private:
//   public:
//     MenuItemMakeCharacter()
//     {

//     }
//     void Exec(){}
// };

// class MenuItemBattle::MenuItemBase{
//   private:
//   public:
//     void Exec(){}
// };

// class MenuItemBattleLog::MenuItemBase{
//   private:
//   public:
//     void Exec(){}
// };


int main()
{
  std::vector< std::unique_ptr<ActionTable>> actionTable;

   //単体攻撃
   std::unique_ptr<Action> actionAttack = std::make_unique<Action>(
     EffectType::Attack,//攻撃
     ParamType::Physical,//物理
     TargetSide::Enemy,//敵
     TargetSelect::LowestHp,//HPが一番低い
     TargetRange::One,//1体
     100
     );

  std::unique_ptr<ActionTable> actionTableAttack = std::make_unique<ActionTable>();
  (*actionTableAttack).AddAction( std::move( actionAttack ) );
  actionTable.push_back( std::move( actionTableAttack ) );

  //２回攻撃
  std::unique_ptr<Action> actionAttackDouble0 = std::make_unique<Action>(
      EffectType::Attack,//攻撃
      ParamType::Physical,//物理
      TargetSide::Enemy,//敵
      TargetSelect::Random,//ランダム
      TargetRange::One,//1体
      50
      );
  std::unique_ptr<Action> actionAttackDouble1 = std::make_unique<Action>(
      EffectType::Attack,//攻撃
      ParamType::Physical,//物理
      TargetSide::Enemy,//敵
      TargetSelect::Random,//ランダム
      TargetRange::One,//1体
      50
      );
  std::unique_ptr<ActionTable> actionTableAttackDouble = std::make_unique<ActionTable>();
  (*actionTableAttackDouble).AddAction( std::move( actionAttackDouble0 ) );
  (*actionTableAttackDouble).AddAction( std::move( actionAttackDouble1 ) );
  actionTable.push_back( std::move( actionTableAttackDouble ) );

  //単体魔法攻撃
  std::unique_ptr<Action> actionAttackMagic = std::make_unique<Action>(
      EffectType::Attack,//攻撃
      ParamType::Magic,//魔法
      TargetSide::Enemy,//敵
      TargetSelect::LowestHp,//Hpが一番低い
      TargetRange::One,//1体
      100
      );
  std::unique_ptr<ActionTable> actionTableAttackMagic = std::make_unique<ActionTable>();
  (*actionTableAttackMagic).AddAction( std::move( actionAttackMagic ) );
  actionTable.push_back( std::move( actionTableAttackMagic ) );

  //全体魔法攻撃
  std::unique_ptr<Action> actionAttackMagicAll = std::make_unique<Action>(
      EffectType::Attack,//攻撃
      ParamType::Magic,//魔法
      TargetSide::Enemy,//敵
      std::nullopt,//なし
      TargetRange::All,//全体
      33
      );
  std::unique_ptr<ActionTable> actionTableAttackMagicAll = std::make_unique<ActionTable>();
  (*actionTableAttackMagicAll).AddAction( std::move( actionAttackMagicAll ) );
  actionTable.push_back( std::move( actionTableAttackMagicAll ) );

  //単体回復
  std::unique_ptr<Action> actionHeal = std::make_unique<Action>(
      EffectType::Heal,//回復
      ParamType::Magic,//魔法
      TargetSide::Friend,//味方
      TargetSelect::LowestHp,//Hpが一番低い
      TargetRange::One,//1体
      100
      );
  std::unique_ptr<ActionTable> actionTableHeal = std::make_unique<ActionTable>();
  (*actionTableHeal).AddAction( std::move( actionHeal ) );
  actionTable.push_back( std::move( actionTableHeal ) );

   //全体回復
   std::unique_ptr<Action> actionHealAll = std::make_unique<Action>(
       EffectType::Heal,//回復
       ParamType::Magic,//魔法
       TargetSide::Friend,//味方
       std::nullopt,//なし
       TargetRange::All,//全体
       33
       );
  std::unique_ptr<ActionTable> actionTableHealAll = std::make_unique<ActionTable>();
  (*actionTableHealAll).AddAction( std::move( actionHealAll ) );
  actionTable.push_back( std::move( actionTableHealAll ) );

  
  std::cout<<"Character Manager\n";
  return 0;
}
