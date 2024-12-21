#pragma once

// This flags variable is used across various tests. If it changes (again) at the API level,
// then we only have to change it this one time here.

#define AUTO_FLAGS (EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)