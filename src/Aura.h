#pragma once

#include <chrono>
#include <string>

class Aura {
	public:
		Aura(const char* identifier) : _identifier(identifier) {}
		virtual ~Aura() = default;
	
		virtual const std::string& identifier() const { return _identifier; }
	
		virtual std::chrono::milliseconds duration() const = 0;

		virtual int tickDamage() const { return 0; }
			
	private:
		const std::string _identifier;
};