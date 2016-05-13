#include "SceneGame.h"

namespace jop
{
    //----- Command handler -----
    JOP_DERIVED_COMMAND_HANDLER(Component, MyHandler)

    JOP_BIND_MEMBER_COMMAND(&GameController::setAction, "setAction");

    JOP_END_COMMAND_HANDLER(MyHandler)
}