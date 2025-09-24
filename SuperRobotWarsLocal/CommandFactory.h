#pragma once

#include <nlohmann/json.hpp>
#include <memory>
#include "Command.h"

/**
 * CommandFactory
 * JSON の "type" フィールドに応じて適切な Command サブクラスを生成します
 */
class CommandFactory {
public:
    /**
     * @param evt JSON オブジェクト (例: {"type":"showMessage", ...})
     * @return 対応する Command オブジェクトの unique_ptr
     * @throws std::runtime_error 未知の type や JSON 不正の場合
     */
    static std::unique_ptr<Command> create(const nlohmann::json& evt);
};