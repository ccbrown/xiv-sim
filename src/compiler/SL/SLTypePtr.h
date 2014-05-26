#pragma once

#include <memory>

class SLType;

struct SLTypePtr : private std::shared_ptr<SLType> {
	SLTypePtr() = default;
	
	template <typename... Args>
	SLTypePtr(Args&&... args) : std::shared_ptr<SLType>(std::forward<Args>(args)...) {}
		
	SLType& operator*() const { return std::shared_ptr<SLType>::operator*(); }
	SLType* operator->() const { return std::shared_ptr<SLType>::operator->(); }
		
	explicit operator bool() const { return std::shared_ptr<SLType>::operator bool(); }
};
