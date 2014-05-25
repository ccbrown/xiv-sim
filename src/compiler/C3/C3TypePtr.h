#pragma once

#include <memory>

class C3Type;

struct C3TypePtr : private std::shared_ptr<C3Type> {
	C3TypePtr() = default;
	
	template <typename... Args>
	C3TypePtr(Args&&... args) : std::shared_ptr<C3Type>(std::forward<Args>(args)...) {}
		
	C3Type& operator*() const { return std::shared_ptr<C3Type>::operator*(); }
	C3Type* operator->() const { return std::shared_ptr<C3Type>::operator->(); }
		
	explicit operator bool() const { return std::shared_ptr<C3Type>::operator bool(); }
};
