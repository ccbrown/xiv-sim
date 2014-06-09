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
	, _identifierHash(configuration->identifier)
	, _rng(rng)
	, _baseStats(configuration->stats)
{
	_updateStats();
	if (configuration->petConfiguration) {
		_pet = new Actor(configuration->petConfiguration, rng);
		_pet->_owner = this;
		_pet->addAlly(this);
	}
	_mp = maximumMP();
}

Actor::~Actor() {
	delete _pet;
}

void Actor::prepareForBattle() {
	_configuration->model->prepareForBattle(this);
}

void Actor::act(Actor* target) {
	auto action = _configuration->rotation ? _configuration->rotation->nextAction(this, target) : nullptr;
	if (!action) { return; }

	if (auto replacement = action->replacement(this, target)) {
		action = replacement;
	}

	if (!action->isUsable(this)) { return; }

	// TODO: get this out of here
	bool swift = dispelAura("swiftcast", this);

	if (!swift && action->castTime(this).count()) {
		_beginCast(action, target);
	} else {
		_executeAction(action, target);
	}
}

void Actor::pretick() {
	bool checkAuras = true;
	while (checkAuras) {
		checkAuras = false;
		for (auto& akv : _auras) {
			for (auto& kv : akv.second.applications) {
				if (kv.second.aura->shouldCancel(this, kv.second.source, kv.second.count)) {
					dispelAura(kv.second.aura->identifierHash(), kv.second.source);
					checkAuras = true;
					break;
				}
			}
			if (checkAuras) { break; }
		}
	}
}

void Actor::tick() {
	auto mpRegen = maximumMP() * 0.02;
	
	for (auto& akv : _auras) {
		for (auto& kv : akv.second.applications) {
			bool isCritical = false;
			if (kv.second.aura->tickDamage()) {
				auto damage = acceptDamage(_generateTickDamage(kv.second));
				isCritical = damage.isCritical;
				kv.second.source->integrateDamageStats(damage, kv.second.aura->identifier().c_str());
			}
			kv.second.aura->tick(this, kv.second.source, kv.second.count, isCritical);
			mpRegen *= kv.second.aura->mpRegenMultiplier();
		}
	}

	setTP(tp() + 60);
	setMP(mp() + (int)(mpRegen));
}

void Actor::addAlly(Actor* actor) {
	if (actor == this) { return; }
	
	if (!_allies.insert(actor).second) { return; }
	actor->_allies.insert(this);

	for (auto ally : actor->allies()) {
		addAlly(ally);
	}
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
		_executeAction(action, target);
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
	
	std::vector<AuraApplication> expired;

	for (auto& akv : _auras) {
		for (auto it = akv.second.applications.begin(); it != akv.second.applications.end();) {
			if (_time - it->second.time >= it->second.duration) {
				_integrateAuraApplicationCountChange(it->second.aura, it->second.count, 0);
				expired.emplace_back(std::move(it->second));
				it = akv.second.applications.erase(it);
				needsStatUpdate = true;
			} else {
				++it;
			}
		}
	}
	
	for (auto& aura : expired) {
		aura.aura->afterEffect(this, aura.source, aura.count);
	}

	if (needsStatUpdate) {
		_updateStats();
	}
}

Damage Actor::performAutoAttack() {
	_lastAutoAttackTime = _time;
	return _configuration->model->generateAutoAttackDamage(this);
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
	
	if (isAutoAttacking()) {
		auto aadel = autoAttackDelayRemaining();
		if (aadel.count() && aadel < ret) { ret = aadel; }
	}
	
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

	for (auto& akv : _auras) {
		for (auto& kv : akv.second.applications) {
			if (kv.second.duration != std::chrono::microseconds::max()) {
				endTime = std::min(kv.second.time + kv.second.duration, endTime);
			}
		}
	}

	if (endTime != std::chrono::microseconds::max()) {
		ret = std::min(endTime - _time, ret);
	}

	return ret;
}

void Actor::applyAura(const Aura* aura, Actor* source, int count) {
	for (auto& akv : _auras) {
		for (auto& kv : akv.second.applications) {
			if (kv.second.aura->providesImmunity(aura)) {
				return;
			}
		}
	}
	
	auto& appliedAura = _auras[aura->identifierHash()];
	appliedAura.isSharedBetweenSources = aura->isSharedBetweenSources();

	auto& application = appliedAura.applications[appliedAura.isSharedBetweenSources ? FNV1AHash() : source->identifierHash()];

	application.aura = aura;
	application.source = source;
	application.time = _time;
	application.duration = aura->duration();
	
	application.baseTickDamage = source->_configuration->model->baseTickDamage(source, aura);
	application.tickCriticalHitChance = source->_configuration->model->tickCriticalHitChance(source);

	if (application.count < aura->maximumCount()) {
		auto newCount = std::min(aura->maximumCount(), application.count + count);
		_integrateAuraApplicationCountChange(aura, application.count, newCount);
		application.count = newCount;
	}

	_updateStats();
}

int Actor::dispelAura(FNV1AHash identifierHash, const Actor* source, int count) {
	auto ait = _auras.find(identifierHash);
	if (ait == _auras.end()) { return 0; }

	auto it = ait->second.applications.find(ait->second.isSharedBetweenSources ? FNV1AHash() : source->identifierHash());
	if (it == ait->second.applications.end()) { return 0; }

	auto dispelled = std::min(it->second.count, count);

	auto newCount = it->second.count - dispelled;
	_integrateAuraApplicationCountChange(it->second.aura, it->second.count, newCount);
	it->second.count = newCount;

	if (!it->second.count) {
		auto application = std::move(it->second);
		ait->second.applications.erase(it);
		_updateStats();
		
		application.aura->afterEffect(this, application.source, dispelled);
	}

	return dispelled;
}

void Actor::extendAura(FNV1AHash identifierHash, const Actor* source, const std::chrono::microseconds& extension) {
	auto ait = _auras.find(identifierHash);
	if (ait == _auras.end()) { return; }

	auto it = ait->second.applications.find(ait->second.isSharedBetweenSources ? FNV1AHash() : source->identifierHash());
	if (it == ait->second.applications.end()) { return; }

	it->second.duration = it->second.duration - (_time - it->second.time) + extension;
	it->second.time = _time;
}

int Actor::auraCount(FNV1AHash identifierHash, const Actor* source) const {
	auto ait = _auras.find(identifierHash);
	if (ait == _auras.end()) { return 0; }

	auto it = ait->second.applications.find(ait->second.isSharedBetweenSources ? FNV1AHash() : source->identifierHash());
	if (it == ait->second.applications.end()) { return 0; }

	return it->second.count;
}

std::chrono::microseconds Actor::auraTimeRemaining(FNV1AHash identifierHash, const Actor* source) const {
	auto ait = _auras.find(identifierHash);
	if (ait == _auras.end()) { return 0_us; }

	auto it = ait->second.applications.find(ait->second.isSharedBetweenSources ? FNV1AHash() : source->identifierHash());
	if (it == ait->second.applications.end()) { return 0_us; }

	return (it->second.duration - (_time - it->second.time));
}

double Actor::damageMultiplier() const {
	double ret = 1.0;
	for (auto& akv : _auras) {
		for (auto& kv : akv.second.applications) {
			ret *= (1.0 + kv.second.aura->increasedDamage() * kv.second.count);
		}
	}
	return ret;
}

double Actor::autoAttackSpeedMultiplier() const {
	double ret = 1.0;
	for (auto& akv : _auras) {
		for (auto& kv : akv.second.applications) {
			ret *= (1.0 + kv.second.aura->increasedAutoAttackSpeed() * kv.second.count);
		}
	}
	return ret;
}

void Actor::transformIncomingDamage(Damage* damage) const {
	for (auto& akv : _auras) {
		for (auto& kv : akv.second.applications) {
			kv.second.aura->transformIncomingDamage(damage);
		}
	}
}

double Actor::additionalCriticalHitChance() const {
	double ret = 0.0;
	for (auto& akv : _auras) {
		for (auto& kv : akv.second.applications) {
			ret += kv.second.aura->additionalCriticalHitChance() * kv.second.count;
		}
	}
	return ret;
}

int Actor::strikesPerAutoAttack() const {
	int ret = 1;
	for (auto& akv : _auras) {
		for (auto& kv : akv.second.applications) {
			ret += kv.second.aura->additionalStrikesPerAutoAttack() * kv.second.count;
		}
	}
	return ret;
}

double Actor::globalCooldownMultiplier() const {
	double ret = 1.0;
	for (auto& akv : _auras) {
		for (auto& kv : akv.second.applications) {
			ret *= (1.0 - kv.second.aura->reducedGlobalCooldown() * kv.second.count);
		}
	}
	return ret;
}

void Actor::triggerCooldown(FNV1AHash identifierHash, std::chrono::microseconds duration) {
	auto& cooldown = _cooldowns[identifierHash];
	cooldown.duration = std::max((cooldown.time + cooldown.duration) - _time, duration);
	cooldown.time = _time;
}

void Actor::endCooldown(FNV1AHash identifierHash) {
	_cooldowns.erase(identifierHash);
}

std::chrono::microseconds Actor::cooldownRemaining(FNV1AHash identifierHash) const {
	auto it = _cooldowns.find(identifierHash);
	return it == _cooldowns.end() ? 0_us : ((it->second.time + it->second.duration) - _time);
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

void Actor::_integrateAuraApplicationCountChange(const Aura* aura, int before, int after) {
	if (_configuration->keepsHistory && !aura->isHidden()) {
		auto& samples = _simulationStats.auraSamples[aura->identifier()];
		auto lastSampleCount = samples.empty() ? 0 : samples.back().second;
		auto newSampleCount = lastSampleCount + (after - before);
		if (newSampleCount != lastSampleCount) {
			samples.emplace_back(_time, newSampleCount);
		}
	}
}

Damage Actor::_generateTickDamage(const Actor::AuraApplication& application) const {
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
	for (auto& akv : _auras) {
		for (auto& kv : akv.second.applications) {
			for (int i = 0; i < kv.second.count; ++i) {
				_stats *= kv.second.aura->statsMultiplier();
			}
		}
	}
}

bool Actor::_beginCast(const Action* action, Actor* target) {
	if (!action->isUsable(this)) { return false; }
		
	if (currentCast() || globalCooldownRemaining().count() || animationLockRemaining().count()) { return false; }

	_castAction = action;
	_castTarget = target;
	_castStartTime = _time;

	return true;
}

bool Actor::_executeAction(const Action* action, Actor* target) {
	if (!action->resolve(this, target)) { return false; }

	if (_configuration->keepsHistory) {
		_simulationStats.actions.push_back(action);
	}
	if (!action->isOffGlobalCooldown()) {
		_comboAction = action;
		_comboActionTime = _time;
	}
	return true;
}