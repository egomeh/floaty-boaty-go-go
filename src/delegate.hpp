#pragma once

#include <vector>
#include <memory>

template<typename ArgumentType>
class DelegateCallbackBase
{
public:
    DelegateCallbackBase() : m_MarkedForDeletion(false)
    {
    }

    virtual void Invoke(const ArgumentType &argument) = 0;
    virtual void *GetObjectPointer() = 0;
    
    bool IsMarkedForDeletion() const
    {
        return m_MarkedForDeletion;
    }

    void MarkForDeletion()
    {
        m_MarkedForDeletion = true;
    }

private:
    bool m_MarkedForDeletion;
};

template<typename ArgumentType, typename T>
class DelegateCallback : public DelegateCallbackBase<ArgumentType>
{
public:
    DelegateCallback(T *object, void (T::*callback)(const ArgumentType&)) : DelegateCallbackBase(), m_Object(object), m_Callback(callback)
    {
    }

    void Invoke(const ArgumentType &argument) override
    {
        (m_Object->*m_Callback)(argument);
    }

    void *GetObjectPointer() override
    {
        return static_cast<void*>(m_Object);
    }

private:
    T* m_Object;
    void (T::*m_Callback) (const ArgumentType&);
};

template<typename ArgumentType>
class Delegate
{
public:
    void Invoke(const ArgumentType &argument)
    {
        for (const auto &callbackFunctor : m_Callbacks)
        {
            callbackFunctor->Invoke(argument);
        }

        for (auto callbackIt = m_Callbacks.begin(); callbackIt != m_Callbacks.end();)
        {
            if (callbackIt->get()->IsMarkedForDeletion())
            {
                callbackIt = m_Callbacks.erase(callbackIt);
            }
            else
            {
                ++callbackIt;
            }
        }
    }

    template<typename T>
    void RegisterCallback(T *listenerObject, void (T::*callback)(const ArgumentType&))
    {
        auto callbackFunctor = std::make_shared<DelegateCallback<ArgumentType, T>>(listenerObject, callback);
        m_Callbacks.push_back(callbackFunctor);
    }

    template<typename T>
    void UnregisterListener(T *listenerObject)
    {
        void *object = static_cast<void*>(listenerObject);

        for (auto callbackIt = m_Callbacks.begin(); callbackIt != m_Callbacks.end(); ++callbackIt)
        {
            void *otherObject = callbackIt->get()->GetObjectPointer();

            if (object == otherObject)
            {
                callbackIt->get()->MarkForDeletion();
            }
        }
    }

private:
    std::vector<std::shared_ptr<DelegateCallbackBase<ArgumentType>>> m_Callbacks;
};
