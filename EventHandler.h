#pragma once

#include "BridgeData.h"

#include <ArcdpsExtension/CombatEventHandler.h>
#include <ArcdpsExtension/Singleton.h>
#include <Nexus/Nexus.h>

template<bool Local>
class EventHandler final : public ArcdpsExtension::CombatEventHandler, public ArcdpsExtension::Singleton<EventHandler<Local>> {
public:
	void SetNexusApi(AddonAPI* pNexusApi) {
		mNexusApi = pNexusApi;
	}

protected:
	void EventInternal(cbtevent* pEvent, ag* pSrc, ag* pDst, const char* pSkillname, uint64_t pId, uint64_t pRevision) override {
		CombatEventHandler::EventInternal(pEvent, pSrc, pDst, pSkillname, pId, pRevision);
		CombatEventHandler::EventInternal(pEvent, pSrc, pDst, pSkillname, pId, pRevision);

		ArcdpsEvent event(pEvent, pSrc, pDst, pSkillname, pId, pRevision);

		const char* eventName;
		if constexpr (Local) {
			eventName = "EV_ARCDPS_COMBATEVENT_LOCAL_RAW";
		} else {
			eventName = "EV_ARCDPS_COMBATEVENT_SQUAD_RAW";
		}

		mNexusApi->RaiseEvent(eventName, &event);
	}

private:
	AddonAPI* mNexusApi = nullptr;
};

using SquadEventHandler = EventHandler<false>;
using LocalEventHandler = EventHandler<true>;
