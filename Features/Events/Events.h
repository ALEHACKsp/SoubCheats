#pragma once
#include "../../SDK/IGameEvent.h"

#define CREATE_EVENT_LISTENER(class_name)\
class class_name : public IGameEventListener\
{\
public:\
	~class_name() { g_pEventManager->RemoveListener(this); }\
\
	virtual void FireGameEvent(IGameEvent* GameEvent);\
};\


class Events
{
public:
	void InitializeEventListeners();

	CREATE_EVENT_LISTENER(ItemPurchaseListener, "item_purchase");
	CREATE_EVENT_LISTENER(PlayerHurtListener, "player_hurt");
	CREATE_EVENT_LISTENER(BulletImpactListener, "bullet_impact");
	CREATE_EVENT_LISTENER(RoundStartListener, "round_start");
	CREATE_EVENT_LISTENER(WeaponFiredListener, "weapon_fire");
};

extern Events g_Events;