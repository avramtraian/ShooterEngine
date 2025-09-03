// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/HashMap.h>
#include <Runtime/Core/Containers/RefPtr.h>
#include <Runtime/Core/Containers/Vector.h>

namespace SE
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// DELEGATE HELPER TYPES AND FUNCTIONS. //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region

template<typename UserClass, typename ReturnType, typename... ParameterTypes>
using PFN_ClassMethod = ReturnType(UserClass::*)(ParameterTypes...);

template<typename ReturnType, typename... ParameterTypes>
using PFN_RawFunction = ReturnType(*)(ParameterTypes...);

using DelegateHandle = uint64;
constexpr DelegateHandle INVALID_DELEGATE_HANDLE = 0;

NODISCARD SHOOTER_API DelegateHandle GenerateDelegateHandle();

#pragma endregion
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// SINGLE DELEGATES. ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region

#define SE_DECLARE_DELEGATE(DelegateType)                                                             using DelegateType = ::SE::GenericDelegate<void>;
#define SE_DECLARE_DELEGATE_ONE_PARAM(DelegateType, ParamType1)                                       using DelegateType = ::SE::GenericDelegate<void, ParamType1>;
#define SE_DECLARE_DELEGATE_TWO_PARAMS(DelegateType, ParamType1, ParamType2)                          using DelegateType = ::SE::GenericDelegate<void, ParamType1, ParamType2>;
#define SE_DECLARE_DELEGATE_THREE_PARAMS(DelegateType, ParamType1, ParamType2, ParamType3)            using DelegateType = ::SE::GenericDelegate<void, ParamType1, ParamType2, ParamType3>;
#define SE_DECLARE_DELEGATE_FOUR_PARAMS(DelegateType, ParamType1, ParamType2, ParamType3, ParamType4) using DelegateType = ::SE::GenericDelegate<void, ParamType1, ParamType2, ParamType3, ParamType4>;

#define SE_DECLARE_DELEGATE_WITH_RETURN(DelegateType, ReturnType)                                                             using DelegateType = ::SE::GenericDelegate<ReturnType>;
#define SE_DECLARE_DELEGATE_WITH_RETURN_ONE_PARAM(DelegateType, ReturnType, ParamType1)                                       using DelegateType = ::SE::GenericDelegate<ReturnType, ParamType1>;
#define SE_DECLARE_DELEGATE_WITH_RETURN_TWO_PARAMS(DelegateType, ReturnType, ParamType1, ParamType2)                          using DelegateType = ::SE::GenericDelegate<ReturnType, ParamType1, ParamType2>;
#define SE_DECLARE_DELEGATE_WITH_RETURN_THREE_PARAMS(DelegateType, ReturnType, ParamType1, ParamType2, ParamType3)            using DelegateType = ::SE::GenericDelegate<ReturnType, ParamType1, ParamType2, ParamType3>;
#define SE_DECLARE_DELEGATE_WITH_RETURN_FOUR_PARAMS(DelegateType, ReturnType, ParamType1, ParamType2, ParamType3, ParamType4) using DelegateType = ::SE::GenericDelegate<ReturnType, ParamType1, ParamType2, ParamType3, ParamType4>;

template<typename ReturnType, typename... ParameterTypes>
class GenericDelegate
{
    template<typename... Q>
    friend class GenericMulticastDelegate;

private:
    ///////////////////////////////////////////////////////////////////////////
    ////////// DELEGATE INSTANCE INTERFACE.
    ///////////////////////////////////////////////////////////////////////////

    class DelegateInstanceInterface : public RefCounted
    {
    public:
        FORCEINLINE DelegateInstanceInterface()
            : m_Handle(GenerateDelegateHandle())
        {}
        virtual ~DelegateInstanceInterface() override = default;

    public:
        NODISCARD FORCEINLINE DelegateHandle GetHandle() const { return m_Handle; }
        NODISCARD virtual bool IsBound() const = 0;
        virtual ReturnType Execute(ParameterTypes... parameters) = 0;

    protected:
        DelegateHandle m_Handle;
    };

    ///////////////////////////////////////////////////////////////////////////
    ////////// RAW FUNCTION POINTER DELEGATE INSTANCE.
    ///////////////////////////////////////////////////////////////////////////

    class DelegateRawFunction : public DelegateInstanceInterface
    {
    public:
        FORCEINLINE DelegateRawFunction(PFN_RawFunction<ReturnType, ParameterTypes...> function)
            : m_Function(function)
        {}
        virtual ~DelegateRawFunction() override = default;

    public:
        NODISCARD virtual bool IsBound() const override
        {
            // NOTE(Traian): A raw function pointer can never be invalidated.
            return true;
        }

        virtual ReturnType Execute(ParameterTypes... parameters) override
        {
            SE_ASSERT(m_Function != nullptr);
            return m_Function(Forward<ParameterTypes>(parameters)...);
        }

    private:
        PFN_RawFunction<ReturnType, ParameterTypes...> m_Function;
    };

    ///////////////////////////////////////////////////////////////////////////
    ////////// RAW FUNCTION POINTER DELEGATE INSTANCE.
    ///////////////////////////////////////////////////////////////////////////

    template<typename LambdaType>
    class DelegateLambda : public DelegateInstanceInterface
    {
    public:
        FORCEINLINE DelegateLambda(LambdaType lambda)
            : m_Lambda(Move(lambda))
        {}
        virtual ~DelegateLambda() override = default;

    public:
        NODISCARD virtual bool IsBound() const override
        {
            // NOTE(Traian): A lambda can never be invalidated.
            return true;
        }

        virtual ReturnType Execute(ParameterTypes... parameters) override
        {
            return m_Lambda(Forward<ParameterTypes>(parameters)...);
        }

    private:
        LambdaType m_Lambda;
    };

    ///////////////////////////////////////////////////////////////////////////
    ////////// REFERENCE COUNTED METHOD DELEGATE INSTANCE.
    ///////////////////////////////////////////////////////////////////////////

    template<typename UserClass>
    class DelegateRefCountedMethod : public DelegateInstanceInterface
    {
    public:
        FORCEINLINE DelegateRefCountedMethod(const StrongRefPtr<UserClass>& userObject, PFN_ClassMethod<UserClass, ReturnType, ParameterTypes...> classMethod)
            : m_UserObject(userObject)
            , m_ClassMethod(classMethod)
        {}
        virtual ~DelegateRefCountedMethod() override = default;

    public:
        NODISCARD virtual bool IsBound() const override
        {
            return m_UserObject.IsValid();
        }

        virtual ReturnType Execute(ParameterTypes... parameters) override
        {
            SE_ASSERT(m_UserObject.IsValid());
            UserClass* userObjectInstance = m_UserObject.Get();
            return (userObjectInstance->*m_ClassMethod)(Forward<ParameterTypes>(parameters)...);
        }

    private:
        WeakRefPtr<UserClass> m_UserObject;
        PFN_ClassMethod<UserClass, ReturnType, ParameterTypes...> m_ClassMethod;
    };

public:
    GenericDelegate() = default;
    ~GenericDelegate() = default;

    FORCEINLINE GenericDelegate(const GenericDelegate& other)
        : m_DelegateInstance(other.m_DelegateInstance)
    {}

    FORCEINLINE GenericDelegate(GenericDelegate&& other) noexcept
        : m_DelegateInstance(Move(other.m_DelegateInstance))
    {}

    FORCEINLINE GenericDelegate& operator=(const GenericDelegate& other)
    {
        m_DelegateInstance = other.m_DelegateInstance;
        return *this;
    }

    FORCEINLINE GenericDelegate& operator=(GenericDelegate&& other) noexcept
    {
        m_DelegateInstance = Move(other.m_DelegateInstance);
        return *this;
    }
    
public:
    NODISCARD FORCEINLINE bool IsBound() const
    {
        if (!m_DelegateInstance.IsValid())
            return false;

        if (!m_DelegateInstance->IsBound())
        {
            m_DelegateInstance.Release();
            return false;
        }

        return true;
    }
    
    FORCEINLINE ReturnType Execute(ParameterTypes... parameters)
    {
        SE_ASSERT(IsBound());
        return m_DelegateInstance->Execute(Forward<ParameterTypes>(parameters)...);
    }

    FORCEINLINE void ExecuteIfBound(ParameterTypes... parameters)
    requires(std::is_same_v<ReturnType, void>)
    {
        if (IsBound())
        {
            Execute(Forward<ParameterTypes>(parameters)...);
        }
    }

    NODISCARD FORCEINLINE DelegateHandle GetBoundHandle() const
    {
        if (!IsBound())
            return INVALID_DELEGATE_HANDLE;
        return m_DelegateInstance->GetHandle();
    }

    FORCEINLINE void Release()
    {
        m_DelegateInstance.Release();
    }

    FORCEINLINE void ReleaseIfHandleMatches(DelegateHandle handle)
    {
        if (GetBoundHandle() == handle)
        {
            Release();
        }
    }

public:
    FORCEINLINE DelegateHandle BindRaw(PFN_RawFunction<ReturnType, ParameterTypes...> function)
    {
        m_DelegateInstance = CreateRef<DelegateRawFunction>(function);
        return m_DelegateInstance->GetHandle();
    }

    template<typename LambdaType>
    FORCEINLINE DelegateHandle BindLambda(LambdaType lambda)
    {
        m_DelegateInstance = CreateRef<DelegateLambda<LambdaType>>(Move(lambda));
        return m_DelegateInstance->GetHandle();
    }

    template<typename UserClass>
    FORCEINLINE DelegateHandle BindRefCounted(const StrongRefPtr<UserClass>& userObject, PFN_ClassMethod<UserClass, ReturnType, ParameterTypes...> classMethod)
    {
        m_DelegateInstance = CreateRef<DelegateRefCountedMethod<UserClass>>(userObject, classMethod);
        return m_DelegateInstance->GetHandle();
    }

private:
    // NOTE(Traian): The delegate instance pointer is marked as mutable because when checking if the
    // delegate is bound (using 'IsBound') if the answer is negative but the delegate instance is valid
    // (when for example we have a 'DelegateRefCountedMethod' instance whose object instance was invalidated)
    // we release the pointer.
    mutable StrongRefPtr<DelegateInstanceInterface> m_DelegateInstance;
};

#pragma endregion
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// MULTICAST DELEGATES. //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region

#define SE_DECLARE_MULTICAST_DELEGATE(DelegateType)                                                             using DelegateType = ::SE::GenericMulticastDelegate;
#define SE_DECLARE_MULTICAST_DELEGATE_ONE_PARAM(DelegateType, ParamType1)                                       using DelegateType = ::SE::GenericMulticastDelegate<ParamType1>;
#define SE_DECLARE_MULTICAST_DELEGATE_TWO_PARAMS(DelegateType, ParamType1, ParamType2)                          using DelegateType = ::SE::GenericMulticastDelegate<ParamType1, ParamType2>;
#define SE_DECLARE_MULTICAST_DELEGATE_THREE_PARAMS(DelegateType, ParamType1, ParamType2, ParamType3)            using DelegateType = ::SE::GenericMulticastDelegate<ParamType1, ParamType2, ParamType3>;
#define SE_DECLARE_MULTICAST_DELEGATE_FOUR_PARAMS(DelegateType, ParamType1, ParamType2, ParamType3, ParamType4) using DelegateType = ::SE::GenericMulticastDelegate<ParamType1, ParamType2, ParamType3, ParamType4>;

template<typename... ParameterTypes>
class GenericMulticastDelegate
{
public:
    using Delegate = GenericDelegate<void, ParameterTypes...>;

public:
    GenericMulticastDelegate() = default;
    ~GenericMulticastDelegate() = default;

    GenericMulticastDelegate(const GenericMulticastDelegate& other) = default;
    GenericMulticastDelegate(GenericMulticastDelegate&& other) noexcept = default;

    GenericMulticastDelegate& operator=(const GenericMulticastDelegate& other) = default;
    GenericMulticastDelegate& operator=(GenericMulticastDelegate&& other) noexcept = default;

public:
    FORCEINLINE void Broadcast(ParameterTypes... parameters)
    {
        // List of delegates that are no longer bound.
        Vector<DelegateHandle> handlesToRemove;

        for (auto& [delegateHandle, delegate] : m_Delegates)
        {
            if (delegate.IsBound())
                delegate.Execute(Forward<ParameterTypes>(parameters)...);
            else
                handlesToRemove.Add(delegateHandle);
        }

        // Remove the delegates that are no longer bound.
        for (DelegateHandle handle : handlesToRemove)
            m_Delegates.RemoveUnchecked(handle);
    }

public:
    FORCEINLINE DelegateHandle AddRaw(PFN_RawFunction<void, ParameterTypes...> function)
    {
        Delegate delegate;
        delegate.BindRaw(function);
        const DelegateHandle handle = delegate.GetBoundHandle();
        m_Delegates.Add(handle, Move(delegate));
        return handle;
    }

    template<typename LambdaType>
    FORCEINLINE DelegateHandle AddLambda(LambdaType lambda)
    {
        Delegate delegate;
        delegate.BindLambda<LambdaType>(Move(lambda));
        const DelegateHandle handle = delegate.GetBoundHandle();
        m_Delegates.Add(handle, Move(delegate));
        return handle;
    }

    template<typename UserClass>
    FORCEINLINE DelegateHandle AddRefCounted(const StrongRefPtr<UserClass>& userObject, PFN_ClassMethod<UserClass, void, ParameterTypes...> classMethod)
    {
        Delegate delegate;
        delegate.BindRefCounted<UserClass>(userObject, classMethod);
        const DelegateHandle handle = delegate.GetBoundHandle();
        m_Delegates.Add(handle, Move(delegate));
        return handle;
    }

    FORCEINLINE void Remove(DelegateHandle handle)
    {
        if (handle == INVALID_DELEGATE_HANDLE)
            return;
        
        m_Delegates.RemoveIfExist(handle);
    }

private:
    HashMap<DelegateHandle, Delegate> m_Delegates;
};

#pragma endregion

}
