#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "randomManager.h"

#ifdef _WIN32
#include <windows.h>
#endif

enum class EffectType {
    Attack, // 攻撃
    Heal,   // 回復
};

enum class ParamType {
    Physical, // 物理
    Magic,    // 魔法
};

enum class TargetSide {
    Enemy,  // 敵
    Friend, // 味方
    Self,   // 自身
};

enum class TargetSelect {
    LowestHp, // Hpが低い
    Random,   // ランダム
};

enum class TargetRange {
    All, // 全体
    One, // 単体
};

//--------------------------------------------------------------------------------------------------

class Action {
  private:
    EffectType mEffectType;
    ParamType mParamType;
    TargetSide mTargetSide;
    std::optional<TargetSelect> mTargetSelect;
    TargetRange mTargetRange;
    unsigned int mPercent;

  public:
    Action(EffectType et, ParamType pt, TargetSide ts,
           std::optional<TargetSelect> tse, TargetRange tr, unsigned int per)
        : mEffectType(et), mParamType(pt), mTargetSide(ts), mTargetSelect(tse),
          mTargetRange(tr), mPercent(per) {}
    EffectType GetEffectType() const { return mEffectType; }

    ParamType GetParamType() const { return mParamType; }

    TargetSide GetTargetSide() const { return mTargetSide; }

    std::optional<TargetSelect> GetTargetSelect() const {
        return mTargetSelect;
    }
    TargetRange GetTargetRange() const { return mTargetRange; }
};

//--------------------------------------------------------------------------------------------------
class ActionTable {
  private:
    std::vector<std::unique_ptr<Action>> mTable;

  public:
    ActionTable() = default;
    ActionTable(ActionTable &&a) { mTable = std::move(a.mTable); }
    void AddAction(std::unique_ptr<Action> action) {
        mTable.push_back(std::move(action));
    }

    const std::vector<std::unique_ptr<Action>> &GetTable() const {
        return mTable;
    }
};

enum class ActionId {
    AttackOne,      // 単体攻撃
    AttackDouble,   // ２回攻撃
    MagicAttackOne, // 単体魔法攻撃
    MagicAttackAll, // 全体魔法攻撃
    HealOne,        // 単体回復
    HealAll,        // 全体回復
};

class CharacterAction {
  public:
    CharacterAction(ActionId id, unsigned int per) : mId(id), mPercent(per) {}
    unsigned int GetPercent() const { return mPercent; }
    ActionId GetActionId() const { return mId; }

  private:
    ActionId mId;
    unsigned int mPercent;
};

enum class CharacterType {
    Warrior, // 戦士
    Caster,  // 魔法使い
    Healer,  // 回復役
    End,
};

enum class CharacterParam { Hp, MaxHp, Attack, Magic, Defence, Speed, End };

//--------------------------------------------------------------------------------------------------
class Character {
  private:
    std::string mName;
    CharacterType mType;
    int mTeamNo;
    std::array<int, static_cast<int>(CharacterParam::End)> mParam{};
    std::vector<CharacterAction> mAction;
    int mRandomBase;
    unsigned int mIndex;

  public:
    Character(CharacterType type, unsigned int index)
        : mType(type), mIndex(index), mTeamNo(0) {}

    // 行動IDとその確率を設定
    void SetAction(ActionId actionId, unsigned int probability) {
        mAction.push_back(CharacterAction(actionId, probability));
    }

    // 何個の行動が選択できるか
    int GetActionSelectNum() const { return mAction.size(); }
    // 指定のインデックスのアクションを選択する確率を取得
    unsigned int GetActionProbability(const unsigned int index) const {
        return mAction[index].GetPercent();
    }

    // 現在HPを最大HPに
    void ResetHp() {
        mParam[static_cast<int>(CharacterParam::Hp)] =
            mParam[static_cast<int>(CharacterParam::MaxHp)];
    }

    // ダメージ
    void SetDamage(const int damage) {
        int hp = mParam[static_cast<int>(CharacterParam::Hp)] - damage;
        if (hp < 0) {
            hp = 0;
        }
        SetParam(CharacterParam::Hp, hp);
    }
    // 回復
    void HealHp(const int heal) {
        int hp = mParam[static_cast<int>(CharacterParam::Hp)] + heal;
        if (hp > mParam[static_cast<int>(CharacterParam::MaxHp)]) {
            hp = mParam[static_cast<int>(CharacterParam::MaxHp)];
        }
        SetParam(CharacterParam::Hp, hp);
    }

    unsigned int GetIndex() const { return mIndex; }

    CharacterType GetType() const { return mType; }

    void SetName(std::string &name) { mName = std::move(name); }
    const std::string &GetName() const { return mName; }

    void SetTeamNo(int teamNo) { mTeamNo = teamNo; }

    int GetTeamNo() const { return mTeamNo; }

    void SetParam(CharacterParam param, int value) {
        mParam[static_cast<int>(param)] = value;
    }
    int GetParam(CharacterParam param) const {
        return mParam[static_cast<int>(param)];
    }
    void SetRandomBase(const int randomBase) { mRandomBase = randomBase; }
    int GetRadomBase() const { return mRandomBase; }
};

//--------------------------------------------------------------------------------------------------
std::array<int, static_cast<int>(CharacterParam::End)>
GetDefaultParam(const CharacterType ct) {
    std::array<int, static_cast<int>(CharacterParam::End)> param;

    switch (ct) {
        case CharacterType::Warrior: // 基礎パラメータ(HP50 攻撃力 30 防御力15
                                     // 魔力5 素早さ10 )
            param[static_cast<int>(CharacterParam::MaxHp)] = 50;
            param[static_cast<int>(CharacterParam::Hp)] = 50;
            param[static_cast<int>(CharacterParam::Attack)] = 30;
            param[static_cast<int>(CharacterParam::Magic)] = 5;
            param[static_cast<int>(CharacterParam::Defence)] = 15;
            param[static_cast<int>(CharacterParam::Speed)] = 10;
            break;
        case CharacterType::Caster: // 基礎パラメータ(HP30 攻撃力 20 防御力10
                                    // 魔力40 素早さ7 )
            param[static_cast<int>(CharacterParam::MaxHp)] = 30;
            param[static_cast<int>(CharacterParam::Hp)] = 30;
            param[static_cast<int>(CharacterParam::Attack)] = 20;
            param[static_cast<int>(CharacterParam::Magic)] = 40;
            param[static_cast<int>(CharacterParam::Defence)] = 10;
            param[static_cast<int>(CharacterParam::Speed)] = 7;
            break;
        case CharacterType::Healer: // 基礎パラメータ基礎パラメータ(HP35 攻撃力
                                    // 20 防御力15 魔力30 素早さ5 )
            param[static_cast<int>(CharacterParam::MaxHp)] = 35;
            param[static_cast<int>(CharacterParam::Hp)] = 35;
            param[static_cast<int>(CharacterParam::Attack)] = 20;
            param[static_cast<int>(CharacterParam::Magic)] = 30;
            param[static_cast<int>(CharacterParam::Defence)] = 15;
            param[static_cast<int>(CharacterParam::Speed)] = 5;
            break;
        case CharacterType::End:
            break;
    }

    return param;
}

// 勝敗がつかないままこのターンを迎えると引き分けとする
const unsigned int MaxTurn = 500;

// 勝敗
enum class Result {
    Team0Win,
    Team1Win,
    Draw,         // 相打ちによる引き分け
    TurnOverDraw, // ターンオーバーによる引き分け
};

//--------------------------------------------------------------------------------------------------
// キャラクター生成
std::unique_ptr<Character> CreateCharacter(const unsigned int randomBase,
                                           const unsigned int index,
                                           const int teamNo) {
    RandomManager rnd(randomBase);

    // 最初に種類を選択
    const int characterTypeIndex =
        rnd.Get() % static_cast<int>(CharacterType::End);
    CharacterType characterType =
        static_cast<CharacterType>(characterTypeIndex);

    // 種類ごとのデフォルトのパラメータを取得
    std::array<int, static_cast<int>(CharacterParam::End)> param =
        GetDefaultParam(characterType);

    std::unique_ptr<Character> character =
        std::make_unique<Character>(characterType, index);

    (character.get())->SetTeamNo(teamNo);

    (character.get())->SetRandomBase(randomBase);

    std::string names[] = {"りんご", "みかん", "ばなな",
                           "きうい", "なし　", "めろん"};
    (character.get())->SetName(names[index]);

    // デフォルトのパラメータを設定
    for (int i = 0; i < static_cast<int>(CharacterParam::End); i++) {
        int v = param[i];
        (character.get())->SetParam(static_cast<CharacterParam>(i), v);
    }

    // 乱数でパラメータを割り振り
    const int TotalBonusPoints = 100;
    for (int i = 0; i < TotalBonusPoints; i++) {
        // CharacterParam::Hpは選ばない。割り振りが終わった後、MaxHpをコピーする
        const int paramId =
            1 + (rnd.Get() % (static_cast<int>(CharacterParam::End) - 1));
        const int value =
            (character.get())->GetParam(static_cast<CharacterParam>(paramId));
        (character.get())
            ->SetParam(static_cast<CharacterParam>(paramId), value + 1);
    }
    const int MaxHp = (character.get())->GetParam(CharacterParam::MaxHp);
    (character.get())->SetParam(CharacterParam::Hp, MaxHp);

    // 種類ごとの行動確率を設定する
    switch (characterType) {
        case CharacterType::Warrior:
            (character.get())->SetAction(ActionId::AttackOne, 50);
            (character.get())->SetAction(ActionId::AttackDouble, 40);
            (character.get())->SetAction(ActionId::HealOne, 10);
            break;
        case CharacterType::Caster:
            (character.get())->SetAction(ActionId::MagicAttackOne, 60);
            (character.get())->SetAction(ActionId::MagicAttackAll, 10);
            (character.get())->SetAction(ActionId::HealOne, 10);
            break;
        case CharacterType::Healer:
            (character.get())->SetAction(ActionId::AttackOne, 10);
            (character.get())->SetAction(ActionId::MagicAttackOne, 10);
            (character.get())->SetAction(ActionId::HealOne, 60);
            (character.get())->SetAction(ActionId::HealAll, 20);
            break;
        case CharacterType::End:
            std::cout << "不明な CharacterType:"
                      << static_cast<int>(characterType);
            break;
    }

    return character;
}

//--------------------------------------------------------------------------------------------------
void DisplayCharacter(const Character &c) {

    std::cout << "TeamNo:" << c.GetTeamNo() << " ";
    std::cout << "Index:" << c.GetIndex() << " ";

    if (c.GetParam(CharacterParam::Hp) <= 0) {
        std::cout << "*";
    } else {
        std::cout << " ";
    }

    std::cout << c.GetName() << " ";

    switch (c.GetType()) {
        case CharacterType::Warrior:
            std::cout << "戦士　　 ";
            break;
        case CharacterType::Caster:
            std::cout << "魔法使い ";
            break;
        case CharacterType::Healer:
            std::cout << "僧侶 　　";
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

// バトルログのデータの種類
enum class LogType {
    TurnNo,     // ターン番号(ターン番号)
    Action,     // 行動(キャラクターインデックス)
    EffectType, // どんな行動をしたか
    Target,     // 対象(キャラクターインデックス)
    Damage,     // ダメージを与える(HPダメージ数)
    Heal,       // 回復する(HP回復数)
    Dead,       // 死亡(キャラクターインデックス)
    Result,     // 勝敗(結果)
    End,        // バトル終了
};

unsigned int MakeLog(const LogType logType, const unsigned int param) {
    unsigned int value = 0;
    value = ((static_cast<unsigned int>(logType) & 0xFF) << 24) |
            (param & 0xFFFFFF);
    return value;
}

//--------------------------------------------------------------------------------------------------
// 指定されたチームの合計HPを取得する
unsigned int sumHpByTeam(const std::vector<Character *> &list,
                         const int teamNo) {
    int sumHp = 0;
    for (auto c : list) {
        if (c->GetTeamNo() == teamNo) {
            sumHp += c->GetParam(CharacterParam::Hp);
        }
    }
    return sumHp;
}

// 行動のパラメータからターゲットのインデックスのテーブルを作る
std::vector<unsigned int> MakeTarget(std::vector<Character *> &list,
                                     Character *actionCharacter, TargetSide ts,
                                     std::optional<TargetSelect> tsl,
                                     TargetRange tr,
                                     RandomManager &randomManager) {
    std::vector<unsigned int> target;

    if (ts == TargetSide::Enemy) {
        // 敵

        //
        // std::cout << "事前チェック\n";
        // for (const auto &c : list) {
        //     DisplayCharacter(*c);
        // }
        // std::cout << "\n";

        // 生きている敵のリストを作る
        int listCount = 0;
        for (const Character *c : list) {
            if (actionCharacter->GetTeamNo() != c->GetTeamNo()) {
                int hp = c->GetParam(CharacterParam::Hp);
                int index = c->GetIndex();
                if (hp > 0) {
                    // std::cout << "Set Target " << index << "li " << listCount
                    //           << " Hp " << hp << "\n";
                    target.push_back(listCount);
                }
            }
            listCount++;
        }
        // std::cout << "target 生存 " << target.size() << "\n";
        // std::cout << "list 生存 " << list.size() << "\n";

        if (tr == TargetRange::All) {
            // 敵全体
            // 既にリストは完成している
        } else {
            // 単体
            if (tsl == TargetSelect::LowestHp) {
                // HPが低いものをターゲットとして選択

                // 敵のリストをHPを低い順に並び替える
                std::sort(target.begin(), target.end(),
                          [list](const int a, const int b) {
                              return list[a]->GetParam(CharacterParam::Hp) <
                                     list[b]->GetParam(CharacterParam::Hp);
                          });
                // targetの先頭より後ろを削除
                if (1 < target.size()) {
                    target.erase(target.begin() + 1, target.end());
                }
            } else if (tsl == TargetSelect::Random) {
                // ランダム
                // 乱数で敵リストから１体を選択しターゲットリストに追加する
                if (target.size() > 0) {
                    unsigned int selectIndex =
                        randomManager.Get() % target.size();
                    unsigned int selectData = target[selectIndex];
                    auto newEnd = std::remove_if(target.begin(), target.end(),
                                                 [selectData](const auto &a) {
                                                     return (selectData != a);
                                                 });
                    target.erase(newEnd, target.end());
                }
            }
        }

    } else if (ts == TargetSide::Friend) {
        // 味方

        // 生きている味方のリストを作る
        for (const Character *c : list) {
            if (actionCharacter->GetTeamNo() == c->GetTeamNo()) {
                if (c->GetParam(CharacterParam::Hp) > 0) {
                    target.push_back(c->GetIndex());
                }
            }
        }

        if (tr == TargetRange::All) {
            // 味方全体
            // 既にリストは完成している
        } else {
            // 単体
            if (tsl == TargetSelect::LowestHp) {
                // HPが低いものをターゲットとして選択

                // 味方のリストをHPの低い順に並び替える
                std::sort(target.begin(), target.end(),
                          [list](const int a, const int b) {
                              return list[a]->GetParam(CharacterParam::Hp) <
                                     list[b]->GetParam(CharacterParam::Hp);
                          });
                // targetの先頭より後ろを削除
                if (1 < target.size()) {
                    target.erase(target.begin() + 1, target.end());
                }

            } else if (tsl == TargetSelect::Random) {
                // ランダム
                // 乱数で味方リストの１体をターゲットリストに追加する
            }
        }
    } else if (ts == TargetSide::Self) {
        // 自分自身
    }
    return target;
}

//--------------------------------------------------------------------------------------------------
// 行動を実行
void ExecuteAction(std::vector<unsigned int> &log,
                   std::vector<Character *> &list, Character *c,
                   unsigned int actionIndex,
                   const std::vector<std::unique_ptr<ActionTable>> &actionTable,
                   RandomManager &randomManager) {

    auto *acttbl = actionTable[actionIndex].get();
    const auto &act = acttbl->GetTable();

    std::cout << "    行動 " << c->GetName() << "[" << c->GetIndex() << "]"
              << " HP " << c->GetParam(CharacterParam::Hp);
    if (c->GetParam(CharacterParam::Hp) <= 0) {
        std::cout << "?";
    }

    for (const std::unique_ptr<Action> &a : act) {
        // 攻撃か回復か
        EffectType et = a->GetEffectType();
        // 物理か魔法か
        ParamType pt = a->GetParamType();
        // 効果の対象
        TargetSide ts = a->GetTargetSide();
        std::optional<TargetSelect> tsl = a->GetTargetSelect();
        TargetRange tr = a->GetTargetRange();

        // 効果の対象のインデックスのテーブルを作る
        std::vector<unsigned int> targets =
            MakeTarget(list, c, ts, tsl, tr, randomManager);

        log.push_back(MakeLog(LogType::Action, c->GetIndex())); //

        log.push_back(
            MakeLog(LogType::EffectType, static_cast<unsigned int>(et))); //

        switch (et) {

            // 攻撃
            case EffectType::Attack: {
                switch (pt) {
                    // 物理攻撃
                    case ParamType::Physical: {
                        const int attack = c->GetParam(CharacterParam::Attack);

                        for (auto index : targets) {
                            Character *targetCharacter = list[index];
                            int damage = attack - targetCharacter->GetParam(
                                                      CharacterParam::Defence);
                            // どれほど固くても１ダメージは与える
                            if (damage < 1) {
                                damage = 1;
                            }

                            const int baseHp =
                                targetCharacter->GetParam(CharacterParam::Hp);

                            targetCharacter->SetDamage(damage);

                            const int resultHp =
                                targetCharacter->GetParam(CharacterParam::Hp);

                            std::cout
                                << "        物理攻撃 "
                                << " ターゲット:" << targetCharacter->GetName()
                                << "[" << targetCharacter->GetIndex() << "] "
                                << "ダメージ " << damage << " Hp: " << baseHp
                                << " -> " << resultHp;

                            if (targetCharacter->GetParam(CharacterParam::Hp) <=
                                0) {
                                std::cout << " 死亡  ";
                            }
                            std::cout << "\n";

                            // ログに追加
                            log.push_back(
                                MakeLog(LogType::Target,
                                        targetCharacter->GetIndex()));       //
                            log.push_back(MakeLog(LogType::Damage, damage)); //
                            if (targetCharacter->GetParam(CharacterParam::Hp) <=
                                0) {
                                log.push_back(
                                    MakeLog(LogType::Dead,
                                            targetCharacter->GetIndex()));
                            }
                        }

                        break;
                    }

                    // 魔法攻撃
                    case ParamType::Magic: {
                        const int attack = c->GetParam(CharacterParam::Magic);

                        for (auto index : targets) {
                            Character *targetCharacter = list[index];
                            int damage = attack - targetCharacter->GetParam(
                                                      CharacterParam::Defence);
                            // どれほど固くても１ダメージは与える
                            if (damage < 1) {
                                damage = 1;
                            }

                            const int baseHp =
                                targetCharacter->GetParam(CharacterParam::Hp);

                            targetCharacter->SetDamage(damage);

                            const int resultHp =
                                targetCharacter->GetParam(CharacterParam::Hp);

                            std::cout
                                << "        魔法攻撃 "
                                << " ターゲット:" << targetCharacter->GetName()
                                << "[" << targetCharacter->GetIndex() << "] "
                                << "ダメージ " << damage << " Hp: " << baseHp
                                << " -> " << resultHp;

                            if (targetCharacter->GetParam(CharacterParam::Hp) <=
                                0) {
                                std::cout << " 死亡  ";
                            }
                            std::cout << "\n";

                            // ログに追加
                            log.push_back(
                                MakeLog(LogType::Target,
                                        targetCharacter->GetIndex()));       //
                            log.push_back(MakeLog(LogType::Damage, damage)); //
                            if (targetCharacter->GetParam(CharacterParam::Hp) <=
                                0) {
                                log.push_back(
                                    MakeLog(LogType::Dead,
                                            targetCharacter->GetIndex()));
                            }
                        }
                        break;
                    }
                }
                break;
            }

            // 回復
            case EffectType::Heal: {
                const int heal = c->GetParam(CharacterParam::Magic);
                for (auto index : targets) {
                    Character *targetCharacter = list[index];
                    const int baseHp =
                        targetCharacter->GetParam(CharacterParam::Hp);
                    targetCharacter->HealHp(heal);

                    const int resultHp =
                        targetCharacter->GetParam(CharacterParam::Hp);
                    // ログに追加
                    log.push_back(MakeLog(LogType::Target,
                                          targetCharacter->GetIndex())); //
                    log.push_back(MakeLog(LogType::Heal, heal));         //
                }
                break;
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------
// 行動を選択
unsigned int
SelectAction(const std::vector<Character *> &list, const Character *c,
             const std::vector<std::unique_ptr<ActionTable>> &actionTable,
             RandomManager &randomManager) {
    int actionNum = c->GetActionSelectNum();
    if (actionNum > 0) {
        unsigned int random = randomManager.Get() % actionNum;
        return random;
    }
    return 0;
}
//--------------------------------------------------------------------------------------------------
// バトルログの生成
void CreateBattleLog(
    std::vector<unsigned int> &log, unsigned int randomSeed,
    std::vector<std::unique_ptr<Character>> &character,
    const std::vector<std::unique_ptr<ActionTable>> &actionTable) {
    log.clear();
    // // 先頭に乱数を入れる
    // log.push_back(randomSeed);

    RandomManager randomManager(randomSeed);

    std::optional<Result> result;
    int turnNo = 0;

    bool exit = false;
    while (1) {
        // ターン番号を保存
        unsigned int v = MakeLog(LogType::TurnNo, turnNo);
        log.push_back(v);
        turnNo++;

        std::vector<Character *> list;

        // キャラ情報
        for (const auto &c : character) {
            list.push_back(c.get());
        }

        std::cout << "----------------------------\n";
        std::cout << "ターン " << turnNo << " ランダム "
                  << randomManager.Check() << " ";
        {
            // それぞれのチームの残りHP
            const unsigned int teamHp0 = sumHpByTeam(list, 0);
            const unsigned int teamHp1 = sumHpByTeam(list, 1);
            std::cout << "    チーム0 :" << teamHp0;
            std::cout << "    チーム1 :" << teamHp1;
        }
        std::cout << "\n";

        // ターン開始前のキャラの状態
        for (const auto &c : character) {
            std::cout << "    ";
            DisplayCharacter(*c);
        }

        if (turnNo >= MaxTurn) {
            exit = true;
            result = Result::TurnOverDraw;
        }

        // キャラをシャッフル(速度が同じキャラが複数いた場合に、いつも同じ順番にならないよう)

        // 速度順に並び替え
        std::sort(list.begin(), list.end(),
                  [](const Character *a, const Character *b) {
                      return a->GetParam(CharacterParam::Speed) >
                             b->GetParam(CharacterParam::Speed);
                  });

        // 速度順に行動を処理
        for (Character *c : list) {

            // 死んでいるキャラクターは行動しない
            if (c->GetParam(CharacterParam::Hp) <= 0) {
                continue;
            }

            // 行動を選択
            unsigned int actionIndex =
                SelectAction(list, c, actionTable, randomManager);

            // 行動を実行
            ExecuteAction(log, list, c, actionIndex, actionTable,
                          randomManager);

            // もしどちらかのチームが全滅しているなら終了
            {
                const unsigned int teamHp0 = sumHpByTeam(list, 0);
                const unsigned int teamHp1 = sumHpByTeam(list, 1);
                if (teamHp0 <= 0) {
                    if (teamHp1 <= 0) {
                        // 引き分け 同時に０になった
                        exit = true;
                        result = Result::Draw;
                    } else {
                        // team1の勝ち
                        exit = true;
                        result = Result::Team1Win;
                    }
                } else {
                    if (teamHp1 <= 0) {
                        // team0の勝ち
                        exit = true;
                        result = Result::Team0Win;
                    }
                }
            }

            if (exit) {
                break;
            }
        }

        if (exit) {
            break;
        }
    }

    // キャラの状態
    for (const auto &c : character) {
        std::cout << "    ";
        DisplayCharacter(*c);
    }

    if (result.has_value()) {

        switch (result.value()) {
            case Result::Draw: {
                std::cout << "結果 引き分け" << "\n";
                break;
            }

            case Result::Team0Win: {
                std::cout << "結果 チーム0の勝ち " << "\n";
                break;
            }

            case Result::Team1Win: {
                std::cout << "結果 チーム1の勝ち " << "\n";
                break;
            }

            case Result::TurnOverDraw: {
                std::cout << "結果 ターンオーバーの引き分け" << "\n";
                break;
            }
        }

        log.push_back(MakeLog(LogType::Result,
                              static_cast<unsigned int>(result.value()))); //
    }
    log.push_back(MakeLog(LogType::End, turnNo));

    std::cout << "\n";
}

void TestFunc();

//--------------------------------------------------------------------------------------------------
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    std::vector<std::unique_ptr<ActionTable>> actionTable;

    // 単体攻撃
    std::unique_ptr<Action> actionAttack =
        std::make_unique<Action>(EffectType::Attack,     // 攻撃
                                 ParamType::Physical,    // 物理
                                 TargetSide::Enemy,      // 敵
                                 TargetSelect::LowestHp, // HPが一番低い
                                 TargetRange::One,       // 1体
                                 100);

    std::unique_ptr<ActionTable> actionTableAttack =
        std::make_unique<ActionTable>();
    (*actionTableAttack).AddAction(std::move(actionAttack));
    actionTable.push_back(std::move(actionTableAttack));

    // ２回攻撃
    std::unique_ptr<Action> actionAttackDouble0 =
        std::make_unique<Action>(EffectType::Attack,   // 攻撃
                                 ParamType::Physical,  // 物理
                                 TargetSide::Enemy,    // 敵
                                 TargetSelect::Random, // ランダム
                                 TargetRange::One,     // 1体
                                 50);
    std::unique_ptr<Action> actionAttackDouble1 =
        std::make_unique<Action>(EffectType::Attack,   // 攻撃
                                 ParamType::Physical,  // 物理
                                 TargetSide::Enemy,    // 敵
                                 TargetSelect::Random, // ランダム
                                 TargetRange::One,     // 1体
                                 50);
    std::unique_ptr<ActionTable> actionTableAttackDouble =
        std::make_unique<ActionTable>();
    (*actionTableAttackDouble).AddAction(std::move(actionAttackDouble0));
    (*actionTableAttackDouble).AddAction(std::move(actionAttackDouble1));
    actionTable.push_back(std::move(actionTableAttackDouble));

    // 単体魔法攻撃
    std::unique_ptr<Action> actionAttackMagic =
        std::make_unique<Action>(EffectType::Attack,     // 攻撃
                                 ParamType::Magic,       // 魔法
                                 TargetSide::Enemy,      // 敵
                                 TargetSelect::LowestHp, // Hpが一番低い
                                 TargetRange::One,       // 1体
                                 100);
    std::unique_ptr<ActionTable> actionTableAttackMagic =
        std::make_unique<ActionTable>();
    (*actionTableAttackMagic).AddAction(std::move(actionAttackMagic));
    actionTable.push_back(std::move(actionTableAttackMagic));

    // 全体魔法攻撃
    std::unique_ptr<Action> actionAttackMagicAll =
        std::make_unique<Action>(EffectType::Attack, // 攻撃
                                 ParamType::Magic,   // 魔法
                                 TargetSide::Enemy,  // 敵
                                 std::nullopt,       // なし
                                 TargetRange::All,   // 全体
                                 33);
    std::unique_ptr<ActionTable> actionTableAttackMagicAll =
        std::make_unique<ActionTable>();
    (*actionTableAttackMagicAll).AddAction(std::move(actionAttackMagicAll));
    actionTable.push_back(std::move(actionTableAttackMagicAll));

    // 単体回復
    std::unique_ptr<Action> actionHeal =
        std::make_unique<Action>(EffectType::Heal,       // 回復
                                 ParamType::Magic,       // 魔法
                                 TargetSide::Friend,     // 味方
                                 TargetSelect::LowestHp, // Hpが一番低い
                                 TargetRange::One,       // 1体
                                 100);
    std::unique_ptr<ActionTable> actionTableHeal =
        std::make_unique<ActionTable>();
    (*actionTableHeal).AddAction(std::move(actionHeal));
    actionTable.push_back(std::move(actionTableHeal));

    // 全体回復
    std::unique_ptr<Action> actionHealAll =
        std::make_unique<Action>(EffectType::Heal,   // 回復
                                 ParamType::Magic,   // 魔法
                                 TargetSide::Friend, // 味方
                                 std::nullopt,       // なし
                                 TargetRange::All,   // 全体
                                 33);
    std::unique_ptr<ActionTable> actionTableHealAll =
        std::make_unique<ActionTable>();
    (*actionTableHealAll).AddAction(std::move(actionHealAll));
    actionTable.push_back(std::move(actionTableHealAll));

    // キャラクター
    std::cout << "キャラクター生成\n";
    std::vector<std::unique_ptr<Character>> character;
    // unsigned int rnd = 12345;
    RandomManager randCharacter(54321);
    for (unsigned int i = 0; i < 6; i++) {
        int teamNo = (i < 3) ? 0 : 1;
        unsigned int rndSub = randCharacter.Get();
        character.push_back(CreateCharacter(rndSub, i, teamNo));
    }
    for (const auto &c : character) {
        DisplayCharacter(*c);
    }

    // これにバトルログを保存
    std::vector<unsigned int> battleLog;

    TestFunc();

    bool exit = false;
    while (1) {
        std::cout << "メニューを選択してください\n";
        std::cout << "0:キャラクター生成\n";
        std::cout << "1:バトル開始\n";
        std::cout << "2:ログ確認\n";
        std::cout << "3:終了\n";

        int menu = 0;
        std::cin >> menu;

        switch (menu) {
            case 0: // キャラクター生成
            {
                std::cout << "キャラクターの生成\n";
                std::cout << "乱数を入力して下さい \n";
                unsigned int randomSeed = 0;
                std::cin >> randomSeed;
                std::cout << "乱数 " << randomSeed << "でキャラクターを生成\n";

                // 既存のキャラクターを削除
                character.clear();

                RandomManager randCharacter(randomSeed);
                for (unsigned int i = 0; i < 6; i++) {
                    int teamNo = (i < 3) ? 0 : 1;
                    unsigned int rndSub = randCharacter.Get();
                    character.push_back(CreateCharacter(rndSub, i, teamNo));
                }

                // バトルログを初期化
                battleLog.clear();

                for (const auto &c : character) {
                    DisplayCharacter(*c);
                }
                std::cout << "\n";
                break;
            }

            case 1: // バトル開始
            {
                std::cout << "バトル開始 \n";
                std::cout << "乱数を入力してください\n";
                unsigned int randomSeed = 0;
                std::cin >> randomSeed;

                std::cout << "乱数 " << randomSeed << " でバトルを開始します\n";

                // HPを最大にリセットする
                for (auto &a : character) {
                    a->ResetHp();
                }

                // バトルログを初期化
                battleLog.clear();

                // 先頭に乱数種を保存
                // battleLog.push_back(randomSeed);

                // キャラクター、乱数種を渡してバトルログを構築
                CreateBattleLog(battleLog, randomSeed, character, actionTable);

                break;
            }

            case 2: // ログ確認
            {
                std::cout << "ログを確認します\n";

                std::cout.setf(std::ios::dec, std::ios::basefield);

                // HPを最大にリセットする
                for (auto &a : character) {
                    a->ResetHp();
                }

                unsigned int TargetCharacterIndex = 0;

                int lineCount = 0;
                for (const unsigned int log : battleLog) {
                    LogType logType = static_cast<LogType>((log >> 24) & 0xFF);
                    unsigned int param = (log & 0xFFFFFF);
                    switch (logType) {
                        case LogType::TurnNo: {
                            std::cout << "----------\n";
                            unsigned int turnNo = param;
                            std::cout << "ターン " << turnNo << "\n";

                            // ターン開始前のキャラの状態
                            for (const auto &c : character) {
                                std::cout << "    ";
                                DisplayCharacter(*c);
                            }

                            break;
                        }

                        case LogType::Action: {
                            unsigned int characterIndex = param;
                            const auto &c = character[characterIndex];
                            std::cout << "    行動 " << c->GetIndex() << "\n";
                            DisplayCharacter(*c);
                            break;
                        }

                        case LogType::EffectType: {
                            std::cout << "    行動タイプ ";
                            switch (static_cast<EffectType>(param)) {
                                case EffectType::Attack: {
                                    std::cout << "攻撃 ";
                                    break;
                                }

                                case EffectType::Heal: {
                                    std::cout << "回復 ";
                                    break;
                                }
                            }

                            std::cout << "\n";
                            break;
                        }

                        case LogType::Target: {
                            std::cout << "    ターゲット " << param << " ";
                            TargetCharacterIndex = param;
                            const auto &c = character[param];
                            DisplayCharacter(*c);
                            std::cout << "\n";
                            break;
                        }

                        case LogType::Damage: {
                            std::cout << "    ダメージ " << param << " ";
                            auto &c = character[TargetCharacterIndex];

                            c->SetDamage(param);

                            // unsigned int hp =
                            //     c->GetParam(CharacterParam::Hp) - param;
                            // c->SetParam(CharacterParam::Hp, hp);

                            DisplayCharacter(*c);
                            std::cout << "\n";
                            break;
                        }

                        case LogType::Heal: {
                            std::cout << "    回復 " << param << " ";
                            auto &c = character[TargetCharacterIndex];
                            unsigned int hp =
                                c->GetParam(CharacterParam::Hp) + param;
                            c->SetParam(CharacterParam::Hp, hp);
                            DisplayCharacter(*c);
                            std::cout << "\n";
                            break;
                        }

                        case LogType::Dead: {
                            std::cout << "    死亡 " << param << " ";
                            std::cout << "\n";
                            break;
                        }

                        case LogType::Result: {
                            std::cout << "    結果 " << param << " ";
                            std::cout << "\n";
                            break;
                        }

                        case LogType::End: {
                            std::cout << "    終了\n";
                            break;
                        }
                    }
                }
                std::cout << "\n";

                std::cout.setf(std::ios::hex);

                break;
            }

            case 3: // 終了
                exit = true;
                break;

            default: // 不正な値
                std::cout << "不正な値です\n";
                break;
        }

        if (exit) {
            break;
        }
        std::cout << "\n";
    }
    std::cout << "終了しました\n";

    return 0;
}

#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

class Score {
  private:
    int id;
    std::string name;
    int point;

  public:
    Score(int _id, std::string _name, int _point)
        : id(_id), name(std::move(_name)), point(_point) {}
    int GetId() const { return id; }
    const std::string &GetName() const { return name; }
    int GetPoint() const { return point; }
};

class ScoreManager {
  private:
    std::vector<Score> scoreTable;
    int nextId;

    int GetNextId() {
        nextId++;
        return nextId;
    }

  public:
    ScoreManager() : nextId(0) {}

    void AddScore(std::string name, int point) {
        scoreTable.emplace_back(GetNextId(), std::move(name), point);
    }
    std::optional<int> FindPointById(int id) const {
        auto it =
            std::find_if(scoreTable.begin(), scoreTable.end(),
                         [id](const auto &a) { return (a.GetId() == id); });

        if (it != scoreTable.end()) {
            return it->GetPoint();
        }
        return std::nullopt;
    }

    std::optional<int> FindIdByName(const std::string &name) const {
        auto it = std::find_if(
            scoreTable.begin(), scoreTable.end(),
            [name](const auto &a) { return (a.GetName() == name); });

        if (it != scoreTable.end()) {
            return it->GetId();
        }
        return std::nullopt;
    }

    bool RemoveScoreById(int id) {
        auto it =
            std::remove_if(scoreTable.begin(), scoreTable.end(),
                           [id](const auto &a) { return (a.GetId() == id); });

        if (it != scoreTable.end()) {
            scoreTable.erase(it, scoreTable.end());
            return true;
        }
        return false;
    }
    const std::vector<Score> &GetScores() const { return scoreTable; }
};

void DisplayScore(const std::vector<Score> &score) {
    for (const auto &i : score) {
        std::cout << "id " << i.GetId() << " name " << i.GetName() << " point "
                  << i.GetPoint() << "\n";
    }
}
void TestFunc() {
    ScoreManager scoreManager;
    scoreManager.AddScore("test0", 90);
    scoreManager.AddScore("test1", 80);
    scoreManager.AddScore("test2", 85);

    std::optional<int> id = scoreManager.FindIdByName("test0");
    if (id.has_value()) {
        std::optional<int> point = scoreManager.FindPointById(id.value());
        if (point.has_value()) {
            std::cout << "id " << id.value() << " point " << point.value()
                      << "\n";
        }
    }

    DisplayScore(scoreManager.GetScores());

    if (id.has_value()) {
        std::cout << "remove id " << id.value() << "\n";
        scoreManager.RemoveScoreById(id.value());
    }

    DisplayScore(scoreManager.GetScores());
}
