#pragma once

#include <string>
#include <vector>

/// 武器データ
struct WeaponData {
    std::string name;          // 武器名（例：ビームライフル）
    int power;                 // 攻撃力
    int minRange;              // 最小射程
    int maxRange;              // 最大射程
    int ammo;                  // 弾数（-1で無限）
    int enCost;                // EN消費
    bool isMap;                // MAP兵器か
    std::string attribute;     // 属性（ビーム、実弾など）
};

/// 機体データ
struct RobotData {
    std::string id;            // 機体ID（例：gundam_rx78）
    std::string name;          // 機体名（例：ガンダム）
    std::string series;        // 作品名（例：機動戦士ガンダム）

    // 基本性能
    int maxHp;                 // 最大HP
    int maxEn;                 // 最大EN
    int armor;                 // 装甲
    int mobility;              // 運動性
    int moveRange;             // 移動力
    char movementType;         // 移動タイプ（L:陸, A:空, S:海、W:宇宙）

    // 地形適応（A,B,C,D）
    char terrainLand;
    char terrainSea;
    char terrainAir;
    char terrainSpace;

    // サイズ（S,M,L,LL）
    char size;

    // 武器リスト
    std::vector<WeaponData> weapons;
};

/// パイロットデータ
struct PilotData {
    std::string id;            // パイロットID
    std::string name;          // パイロット名
    std::string series;        // 作品名

    // 能力値
    int fighting;              // 格闘
    int shooting;              // 射撃
    int defense;               // 防御
    int skill;                 // 技量
    int reaction;              // 反応

    // 精神コマンド
    std::vector<std::string> spirits;

    // 特殊能力
    std::vector<std::string> abilities;
};

/// ユニット（機体+パイロット）
struct UnitData {
    std::string unitId;        // ユニットID
    std::string robotId;       // 機体ID
    std::string pilotId;       // パイロットID
    int level;                 // レベル
    int experience;            // 経験値
};