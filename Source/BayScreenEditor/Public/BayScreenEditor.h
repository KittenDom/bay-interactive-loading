#pragma once

#include "Modules/ModuleManager.h"

class FBayScreenEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
