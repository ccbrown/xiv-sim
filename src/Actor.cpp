#include "Actor.h"

#include "Action.h"
#include "Aura.h"
#include "Model.h"
#include "Rotation.h"

Actor::Stats& Actor::Stats::operator*=(const Aura::StatsMultiplier& multiplier) {
	strength      *= multiplier.strength;
	dexterity     *= multiplier.dexterity;
	intelligence  *= multiplier.intelligence;
	piety         *= multiplier.piety;

	criticalHitRate  *= multiplier.criticalHitRate;
	skillSpeed       *= multiplier.skillSpeed;
	spellSpeed       *= multiplier.spellSpeed;
	determination    *= multiplier.determination;

	static_assert(sizeof(multiplier) == sizeof(double) * 8, "forget something?");

	return *this;
}

Actor::Actor(const Configuration* configuration, std::mt19937* rng)
	: _configuration(configuration)
	, _identifierHash(FNV1AHash(configuration->identifier))
	, _rng(rng)
	, _baseStats(configuration->stats)
{
	_updateStats();
	if (configuration->petConfiguration) {
		_pet = new Actor(configuration->petConfiguration, rng);
		_pet->_owner = this;
	}
	_mp = maximumMP();
}

Actor::~Actor() {
	delete _pet;
}

const Action* Actor::act(const Actor* target) {
	auto action = _configuration->rotation ? _configuration->rotation->nextAction(this, target) : nullptr;
	if (!action) { return nullptr; }
	if (auto replacement = action->replacement(this, target)) {
		return replacement;
	}
	return action;
}

void Actor::tick() {
	auto mpRegen = maximumMP() * 0.02;
	
	for (auto& kv : _auras) {
		bool isCritical = false;
		if (kv.second.aura->tickDamage()) {
			auto damage = acceptDamage(_generateTickDamage(kv.second));
			isCritical = damage.isCritical;
			kv.second.source->integrateDamageStats(damage, kv.second.aura->identifier().c_str());
		}
		kv.second.aura->tick(this, kv.second.source, kv.second.count, isCritical);
		mpRegen *= kv.second.aura->mpRegenMultiplier();
	}

	setTP(std::min(tp() + 60, 1000));
	setMP(std::min(mp() + (int)(mpRegen), maximumMP()));
}

void Actor::integrateDamageStats(const Damage& damage, const char* effect) {
	EffectSimulationStats effectStats;
	effectStats.count = 1;
	effectStats.criticalHits = damage.isCritical ? 1 : 0;
	effectStats.damageDealt = damage.amount;
	_simulationStats += effectStats;
	_effectSimulationStats[effect] += effectStats;
}

Damage Actor::generateDamage(const Action* action, const Actor* target) {
	return _configuration->model->generateDamage(action, this, target);
}

Damage Actor::acceptDamage(const Damage& incoming) const {
	return _configuration->model->acceptDamage(incoming, this);
}

void Actor::advanceTime(const std::chrono::microseconds& time) {
	if (_configuration->keepsHistory) {
		if (_simulationStats.tpSamples.empty() || _simulationStats.tpSamples.back().second != _tp) {
			_simulationStats.tpSamples.emplace_back(_time, _tp);
		}
		if (_simulationStats.mpSamples.empty() || _simulationStats.mpSamples.back().second != _mp) {
			_simulationStats.mpSamples.emplace_back(_time, _mp);
		}
	}

	_time = time;

	std::chrono::microseconds castTimeRemaining;
	if (currentCast(&castTimeRemaining) && !castTimeRemaining.count()) {
		auto action = _castAction;
		auto target = _castTarget;
		_castAction = nullptr;
		_castTarget = nullptr;
		executeAction(action, target);
		_lastAutoAttackTime = _time;
		_globalCooldownStartTime = _castStartTime;
		_animationLockEndTime = _castStartTime + 1_s;
	}

	for (auto it = _cooldowns.begin(); it != _cooldowns.end();) {
		if (_time - it->second.time >= it->second.duration) {
			it = _cooldowns.erase(it);
		} else {
			++it;
		}
	}

	bool needsStatUpdate = false;
	
	std::vector<AppliedAura> expired;

	for (auto it = _auras.begin(); it != _auras.end();) {
		if (_time - it->second.time >= it->second.duration) {
			auto aura = it->second.aura;
			expired.emplace_back(std::move(it->second));
			_integrateAuraApplicationCountChange(aura, 0);
			it = _auras.erase(it);
			needsStatUpdate = true;
		} else {
			++it;
		}
	}
	
	for (auto& aura : expired) {
		aura.aura->expiration(this, aura.source, aura.count);
	}

	if (needsStatUpdate) {
		_updateStats();
	}
}

Damage Actor::performAutoAttack() {
	_lastAutoAttackTime = _time;
	return _configuration->model->generateAutoAttackDamage(this);
}

bool Actor::executeAction(const Action* action, Actor* target) {
	if (action->resolve(this, target)) {
		if (_configuration->keepsHistory) {
			_simulationStats.actions.push_back(action);
		}
		if (!action->isOffGlobalCooldown()) {
			_comboAction = action;
			_comboActionTime = _time;
		}
		return true;
	}
	return false;
}

const Action* Actor::comboAction() const {
	return (_comboAction && _time - _comboActionTime <= 10_s) ? _comboAction : nullptr;
}

int Actor::maximumMP() const {
	return _configuration->model->maximumMP(this);
}

std::chrono::microseconds Actor::autoAttackDelayRemaining() const {
	// TODO: make sure this matches ffxiv for changes to greased lightning between auto-attacks
	auto elapsed = _time - _lastAutoAttackTime;
	auto interval = _configuration->model->autoAttackInterval(this);
	return elapsed < interval ? interval - elapsed : 0_us;
}

std::chrono::microseconds Actor::globalCooldown() const {
	return _configuration->model->globalCooldown(this);
}

std::chrono::microseconds Actor::globalCooldownRemaining() const {
	// TODO: make sure this matches ffxiv for changes to skill / spell speed mid-gcd
	auto elapsed = _time - _globalCooldownStartTime;
	auto gcd = globalCooldown();
	return elapsed < gcd ? gcd - elapsed : 0_us;
}

std::chrono::microseconds Actor::animationLockRemaining() const {
	return _time >= _animationLockEndTime ? 0_us : (_animationLockEndTime - _time);
}

std::chrono::microseconds Actor::timeUntilNextTimeOfInterest() const {
	auto ret = std::chrono::microseconds::max();
	
	auto aadel = autoAttackDelayRemaining();
	if (aadel.count() && aadel < ret) { ret = aadel; }
	
	auto gcd = globalCooldownRemaining();
	if (gcd.count() && gcd < ret) { ret = gcd; }

	auto alock = animationLockRemaining();
	if (alock.count() && alock < ret) { ret = alock; }
	
	std::chrono::microseconds remaining;
	if (currentCast(&remaining)) {
		ret = std::min(ret, remaining);
	}
	
	std::chrono::microseconds endTime = std::chrono::microseconds::max();

	for (auto& kv : _cooldowns) {
		endTime = std::min(kv.second.time + kv.second.duration, endTime);
	}

	for (auto& kv : _auras) {
		endTime = std::min(kv.second.time + kv.second.duration, endTime);
	}

	if (endTime != std::chrono::microseconds::max()) {
		ret = std::min(endTime - _time, ret);
	}

	return ret;
}

void Actor::applyAura(const Aura* aura, Actor* source, int count) {
	for (auto& kv : _auras) {
		if (kv.second.aura->providesImmunity(aura)) {
			return;
		}
	}
	
	auto& application = _auras[_appliedAuraKey(aura->identifier(), source)];

	application.aura = aura;
	application.source = source;
	application.time = _time;
	application.duration = aura->duration();
	
	application.baseTickDamage = source->_configuration->model->baseTickDamage(source, aura);
	application.tickCriticalHitChance = source->_configuration->model->tickCriticalHitChance(source);

	if (application.count < aura->maximumCount()) {
		application.count = std::min(aura->maximumCount(), application.count + count);
		_integrateAuraApplicationCountChange(aura, application.count);
	}

	_updateStats();
}

int Actor::dispelAura(const std::string& identifier, Actor* source, int count) {
	auto it = _auras.find(_appliedAuraKey(identifier, source));
	if (it == _auras.end()) { return 0; }

	auto dispelled = std::min(it->second.count, count);

	it->second.count = it->second.count - dispelled;
	_integrateAuraApplicationCountChange(it->second.aura, it->second.count);
	if (!it->second.count) {
		_auras.erase(it);
		_updateStats();
	}
	
	return dispelled;
}

void Actor::extendAura(const std::string& identifier, Actor* source, const std::chrono::microseconds& extension) {
	auto it = _auras.find(_appliedAuraKey(identifier, source));
	if (it == _auras.end()) { return; }

	it->second.duration = it->second.duration - (_time - it->second.time) + extension;
	it->second.time = _time;
}

int Actor::auraCount(const std::string& identifier, const Actor* source) const {
	auto it = _auras.find(_appliedAuraKey(identifier, source));
	if (it == _auras.end()) { return 0; }
	return it->second.count;
}

std::chrono::microseconds Actor::auraTimeRemaining(const std::string& identifier, const Actor* source) const {
	auto it = _auras.find(_appliedAuraKey(identifier, source));
	if (it == _auras.end()) { return 0_us; }
	return (it->second.duration - (_time - it->second.time));
}

double Actor::damageMultiplier() const {
	double ret = 1.0;
	for (auto& kv : _auras) {
		ret *= (1.0 + kv.second.aura->increasedDamage() * kv.second.count);
	}
	return ret;
}

double Actor::autoAttackSpeedMultiplier() const {
	double ret = 1.0;
	for (auto& kv : _auras) {
		ret *= (1.0 + kv.second.aura->increasedAutoAttackSpeed() * kv.second.count);
	}
	return ret;
}

void Actor::transformIncomingDamage(Damage* damage) const {
	for (auto& kv : _auras) {
		kv.second.aura->transformIncomingDamage(damage);
	}
}

double Actor::additionalCriticalHitChance() const {
	double ret = 0.0;
	for (auto& kv : _auras) {
		ret += kv.second.aura->additionalCriticalHitChance() * kv.second.count;
	}
	return ret;
}

int Actor::strikesPerAutoAttack() const {
	int ret = 1;
	for (auto& kv : _auras) {
		ret += kv.second.aura->additionalStrikesPerAutoAttack() * kv.second.count;
	}
	return ret;
}

double Actor::globalCooldownMultiplier() const {
	double ret = 1.0;
	for (auto& kv : _auras) {
		ret *= (1.0 - kv.second.aura->reducedGlobalCooldown() * kv.second.count);
	}
	return ret;
}

void Actor::triggerCooldown(const std::string& identifier, std::chrono::microseconds duration) {
	auto& cooldown = _cooldowns[identifier];
	cooldown.duration = std::max((cooldown.time + cooldown.duration) - _time, duration);
	cooldown.time = _time;
}

void Actor::endCooldown(const std::string& identifier) {
	_cooldowns.erase(identifier);
}

std::chrono::microseconds Actor::cooldownRemaining(const std::string& identifier) const {
	auto it = _cooldowns.find(identifier);
	return it == _cooldowns.end() ? 0_us : ((it->second.time + it->second.duration) - _time);
}

bool Actor::beginCast(const Action* action, Actor* target) {
	if (!action->isUsable(this)) { return false; }
		
	if (currentCast() || globalCooldownRemaining().count() || animationLockRemaining().count()) { return false; }

	_castAction = action;
	_castTarget = target;
	_castStartTime = _time;

	return true;
}

const Action* Actor::currentCast(std::chrono::microseconds* remaining, Actor** target) const {
	if (_castAction) {
		if (target) {
			*target = _castTarget;
		}
		if (remaining) {
			*remaining = std::max(_configuration->model->castTime(_castAction, this) - (_time - _castStartTime), 0_us);
		}
	}

	return _castAction;
}

void Actor::triggerGlobalCooldown() {
	_globalCooldownStartTime = _time;
}

void Actor::triggerAnimationLock() {
	_animationLockEndTime = _time + 1_s;
}

uint64_t Actor::_appliedAuraKey(const std::string& auraIdentifier, const Actor* source) const {
	uint64_t h1 = 14695981039346656037ull;
	for (auto& c : auraIdentifier) {
		h1 = (h1 ^ c) * 1099511628211ull;
	}

	uint64_t h2 = source->identifierHash();

	return ((h1 << 32) | (h1 >> 32)) ^ h2;
}

void Actor::_integrateAuraApplicationCountChange(const Aura* aura, int count) {
	if (_configuration->keepsHistory && !aura->isHidden()) {
		auto& samples = _simulationStats.auraSamples[aura->identifier()];
		samples.emplace_back(_time, count);
	}
}

Damage Actor::_generateTickDamage(const Actor::AppliedAura& application) const {
	Damage ret;

	std::uniform_real_distribution<double> distribution(0.0, 1.0);

	ret.isCritical = (distribution(*_rng) < application.tickCriticalHitChance);

	double amount = application.baseTickDamage;

	// TODO: this sway is a complete guess off the top of my head and should be researched
	amount *= 1.0 + (0.5 - distribution(*_rng)) * 0.1;

	if (ret.isCritical) {
		amount *= 1.5;
	}
	
	ret.amount = amount;

	return ret;
}

void Actor::_updateStats() {
	_stats = _baseStats;
	for (auto& kv : _auras) {
		for (int i = 0; i < kv.second.count; ++i) {
			_stats *= kv.second.aura->statsMultiplier();
		}
	}
}
