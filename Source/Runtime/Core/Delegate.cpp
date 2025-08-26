// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Delegate.h>

namespace SE
{

DelegateHandle GenerateDelegateHandle()
{
    static DelegateHandle s_LastGeneratedHandle = INVALID_DELEGATE_HANDLE;
    return ++s_LastGeneratedHandle;
}

}
