#include <iostream>
#include <optional>
#include <vector>
#include <array>
#include <memory>
#include <utility>

#include "randomManager.h"

#ifdef _WIN32
#include <windows.h>
#endif

enum class EffectType
{
  Attack, // 攻撃
  Heal,   // 回復
};

enum class ParamType
{
  Physical, // 物理
  Magic,    // 魔法
};

enum class TargetSide
{
  Enemy,  // 敵
  Friend, // 味方
  Self,   // 自身
};

enum class TargetSelect
{
  LowestHp, // Hpが低い
  Random,   // ランダム
};

enum class TargetRange
{
  All, // 全体
  One, // 単体
};

class Action
{
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
      unsigned int per) : mEffectType(et),
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
  ActionTable(ActionTable &&a)
  {
    mTable = std::move(a.mTable);
  }
  void AddAction(std::unique_ptr<Action> action)
  {
    mTable.push_back(std::move(action));
  }
};

enum class ActionId
{
  AttackOne,      // 単体攻撃
  AttackDouble,   // ２回攻撃
  MagicAttackOne, // 単体魔法攻撃
  MagicAttackAll, // 全体魔法攻撃
  HealOne,        // 単体回復
  HealAll,        // 全体回復
};

class CharacterAction
{
  ActionId mId;
  int mPercent;
};

enum class CharacterType
{
  Warrior, // 戦士
  Caster,  // 魔法使い
  Healer,  // 回復役
  End,
};

enum class CharacterParam
{
  Hp,
  MaxHp,
  Attack,
  Magic,
  Defence,
  Speed,
  End
};

class Character
{
private:
  CharacterType mType;
  std::array<int, static_cast<int>(CharacterParam::End)> mParam{};
  std::vector<CharacterAction> mAction;
  int mRandomBase;

public:
  Character(
      CharacterType type)
      : mType(type)
  {
  }
  CharacterType GetType() const
  {
    return mType;
  }
  void SetParam(CharacterParam param, int value)
  {
    mParam[static_cast<int>(param)] = value;
  }
  int GetParam(CharacterParam param) const
  {
    return mParam[static_cast<int>(param)];
  }
  void SetRandomBase(const int randomBase)
  {
    mRandomBase = randomBase;
  }
  int GetRadomBase() const
  {
    return mRandomBase;
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

std::array<int, static_cast<int>(CharacterParam::End)> GetDefaultParam(const CharacterType ct)
{
  std::array<int, static_cast<int>(CharacterParam::End)> param;

  switch (ct)
  {
  case CharacterType::Warrior: // 基礎パラメータ(HP50 攻撃力 30 防御力15 魔力5 素早さ10 )
    param[static_cast<int>(CharacterParam::MaxHp)] = 50;
    param[static_cast<int>(CharacterParam::Hp)] = 50;
    param[static_cast<int>(CharacterParam::Attack)] = 30;
    param[static_cast<int>(CharacterParam::Magic)] = 5;
    param[static_cast<int>(CharacterParam::Defence)] = 15;
    param[static_cast<int>(CharacterParam::Speed)] = 10;
    break;
  case CharacterType::Caster: // 基礎パラメータ(HP30 攻撃力 20 防御力10 魔力40 素早さ7 )
    param[static_cast<int>(CharacterParam::MaxHp)] = 30;
    param[static_cast<int>(CharacterParam::Hp)] = 30;
    param[static_cast<int>(CharacterParam::Attack)] = 20;
    param[static_cast<int>(CharacterParam::Magic)] = 40;
    param[static_cast<int>(CharacterParam::Defence)] = 10;
    param[static_cast<int>(CharacterParam::Speed)] = 7;
    break;
  case CharacterType::Healer: // 基礎パラメータ基礎パラメータ(HP35 攻撃力 20 防御力15 魔力30 素早さ5 )
    param[static_cast<int>(CharacterParam::MaxHp)] = 35;
    param[static_cast<int>(CharacterParam::Hp)] = 35;
    param[static_cast<int>(CharacterParam::Attack)] = 20;
    param[static_cast<int>(CharacterParam::Magic)] = 30;
    param[static_cast<int>(CharacterParam::Defence)] = 15;
    param[static_cast<int>(CharacterParam::Speed)] = 5;
    break;
  }

  return param;
}

std::unique_ptr<Character> CreateCharacter(const unsigned int randomBase)
{

  RandomManager rnd(randomBase);

  // 最初に種類を選択
  const int ict = rnd.Get() % static_cast<int>(CharacterType::End);
  CharacterType ct = static_cast<CharacterType>(ict);

  // 種類ごとのデフォルトのパラメータを取得
  std::array<int, static_cast<int>(CharacterParam::End)> param = GetDefaultParam(ct);

  std::unique_ptr<Character> character = std::make_unique<Character>(ct);

  (character.get())->SetRandomBase(randomBase);

  // デフォルトのパラメータを設定
  for (int i = 0; i < static_cast<int>(CharacterParam::End); i++)
  {
    int v = param[i];
    (character.get())->SetParam(static_cast<CharacterParam>(i), v);
  }

  std::cout << "乱数でパラメータを割り振り randumBase " << randomBase << "\n";
  // 乱数でパラメータを割り振り
  const int TotalBonusPoints = 100;
  for (int i = 0; i < TotalBonusPoints; i++)
  {
    // CharacterParam::Hpは選ばない。割り振りが終わった後、MaxHpをコピーする
    const int paramId = 1 + (rnd.Get() % (static_cast<int>(CharacterParam::End) - 1));
    std::cout << paramId << " ";
    const int value = (character.get())->GetParam(static_cast<CharacterParam>(paramId));
    (character.get())->SetParam(static_cast<CharacterParam>(paramId), value + 1);
  }
  std::cout << "\n";
  const int MaxHp = (character.get())->GetParam(CharacterParam::MaxHp);
  (character.get())->SetParam(CharacterParam::Hp, MaxHp);

  // 差分確認
  std::cout << "差分確認 ";
  for (int i = 0; i < static_cast<int>(CharacterParam::End); i++)
  {
    int v = (character.get())->GetParam(static_cast<CharacterParam>(i)) - param[i];
    std::cout << " " << v << " ";
  }
  std::cout << "\n";

  return character;
}

void DisplayCharacter(const Character &c)
{
  switch (c.GetType())
  {
  case CharacterType::Warrior:
    std::cout << "戦士 ";
    break;
  case CharacterType::Caster:
    std::cout << "魔法使い ";
    break;
  case CharacterType::Healer:
    std::cout << "僧侶 ";
    break;
  default:
    std::cout << "不明なタイプ";
    break;
  }
  std::cout << "Hp:" << c.GetParam(CharacterParam::Hp) << " ";
  std::cout << "MaxHp:" << c.GetParam(CharacterParam::MaxHp) << " ";
  std::cout << "Attack:" << c.GetParam(CharacterParam::Attack) << " ";
  std::cout << "Magic:" << c.GetParam(CharacterParam::Magic) << " ";
  std::cout << "Defence:" << c.GetParam(CharacterParam::Defence) << " ";
  std::cout << "Speed:" << c.GetParam(CharacterParam::Speed) << "\n";
}

void DisplayMenu()
{
  ;
}

int main()
{
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
#endif
  std::vector<std::unique_ptr<ActionTable>> actionTable;

  // 単体攻撃
  std::unique_ptr<Action> actionAttack = std::make_unique<Action>(
      EffectType::Attack,     // 攻撃
      ParamType::Physical,    // 物理
      TargetSide::Enemy,      // 敵
      TargetSelect::LowestHp, // HPが一番低い
      TargetRange::One,       // 1体
      100);

  std::unique_ptr<ActionTable> actionTableAttack = std::make_unique<ActionTable>();
  (*actionTableAttack).AddAction(std::move(actionAttack));
  actionTable.push_back(std::move(actionTableAttack));

  // ２回攻撃
  std::unique_ptr<Action> actionAttackDouble0 = std::make_unique<Action>(
      EffectType::Attack,   // 攻撃
      ParamType::Physical,  // 物理
      TargetSide::Enemy,    // 敵
      TargetSelect::Random, // ランダム
      TargetRange::One,     // 1体
      50);
  std::unique_ptr<Action> actionAttackDouble1 = std::make_unique<Action>(
      EffectType::Attack,   // 攻撃
      ParamType::Physical,  // 物理
      TargetSide::Enemy,    // 敵
      TargetSelect::Random, // ランダム
      TargetRange::One,     // 1体
      50);
  std::unique_ptr<ActionTable> actionTableAttackDouble = std::make_unique<ActionTable>();
  (*actionTableAttackDouble).AddAction(std::move(actionAttackDouble0));
  (*actionTableAttackDouble).AddAction(std::move(actionAttackDouble1));
  actionTable.push_back(std::move(actionTableAttackDouble));

  // 単体魔法攻撃
  std::unique_ptr<Action> actionAttackMagic = std::make_unique<Action>(
      EffectType::Attack,     // 攻撃
      ParamType::Magic,       // 魔法
      TargetSide::Enemy,      // 敵
      TargetSelect::LowestHp, // Hpが一番低い
      TargetRange::One,       // 1体
      100);
  std::unique_ptr<ActionTable> actionTableAttackMagic = std::make_unique<ActionTable>();
  (*actionTableAttackMagic).AddAction(std::move(actionAttackMagic));
  actionTable.push_back(std::move(actionTableAttackMagic));

  // 全体魔法攻撃
  std::unique_ptr<Action> actionAttackMagicAll = std::make_unique<Action>(
      EffectType::Attack, // 攻撃
      ParamType::Magic,   // 魔法
      TargetSide::Enemy,  // 敵
      std::nullopt,       // なし
      TargetRange::All,   // 全体
      33);
  std::unique_ptr<ActionTable> actionTableAttackMagicAll = std::make_unique<ActionTable>();
  (*actionTableAttackMagicAll).AddAction(std::move(actionAttackMagicAll));
  actionTable.push_back(std::move(actionTableAttackMagicAll));

  // 単体回復
  std::unique_ptr<Action> actionHeal = std::make_unique<Action>(
      EffectType::Heal,       // 回復
      ParamType::Magic,       // 魔法
      TargetSide::Friend,     // 味方
      TargetSelect::LowestHp, // Hpが一番低い
      TargetRange::One,       // 1体
      100);
  std::unique_ptr<ActionTable> actionTableHeal = std::make_unique<ActionTable>();
  (*actionTableHeal).AddAction(std::move(actionHeal));
  actionTable.push_back(std::move(actionTableHeal));

  // 全体回復
  std::unique_ptr<Action> actionHealAll = std::make_unique<Action>(
      EffectType::Heal,   // 回復
      ParamType::Magic,   // 魔法
      TargetSide::Friend, // 味方
      std::nullopt,       // なし
      TargetRange::All,   // 全体
      33);
  std::unique_ptr<ActionTable> actionTableHealAll = std::make_unique<ActionTable>();
  (*actionTableHealAll).AddAction(std::move(actionHealAll));
  actionTable.push_back(std::move(actionTableHealAll));

  // キャラクター
  std::cout << "キャラクター生成\n";
  std::vector<std::unique_ptr<Character>> character;
  unsigned int rnd = 12345;
  for (int i = 0; i < 6; i++)
  {
    RandomManager randCharacter(rnd);
    unsigned int rndSub = randCharacter.Get();
    character.push_back(CreateCharacter(rndSub));
    rnd = rnd * rndSub;
  }
  for (const auto &c : character)
  {
    DisplayCharacter(*c);
  }

  std::cout << "乱数テスト\n";
  RandomManager r(12345);
  RandomManager r2(12346);
  std::array<int, 6> a{};
  std::array<int, 6> a2{};

  for (int i = 0; i < 100000; i++)
  {
    unsigned int value = r.Get();
    a[value % 6]++;
    unsigned int value2 = r2.Get();
    a2[value2 % 6]++;
  }
  for (int &v : a)
  {
    std::cout << " " << v;
    ;
  }
  std::cout << "r2\n";
  for (int &v : a2)
  {
    std::cout << " " << v;
    ;
  }

  std::cout << "Character Manager\n";
  return 0;
}
