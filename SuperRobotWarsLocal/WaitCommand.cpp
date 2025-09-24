#include "WaitCommand.h"
#include <SDL.h>

WaitCommand::WaitCommand(const nlohmann::json& evt)
    : durationMs_(evt.value("duration", 1000))
{
}

void WaitCommand::execute(ExecutionEngine& /*engine*/) {
    SDL_Delay(durationMs_);
}