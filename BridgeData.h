#pragma once

#include <ArcdpsExtension/arcdps_structs_slim.h>

struct ArcdpsEvent {
	cbtevent* Event;
	ag* Src;
	ag* Dst;
	const char* Skillname;
	uint64_t Id;
	uint64_t Revision;

	ArcdpsEvent(
			cbtevent* pEvent,
			ag* pSrc,
			ag* pDst,
			const char* pSkillname,
			uint64_t pId,
			uint64_t pRevision
	)
		: Event(pEvent),
		  Src(pSrc),
		  Dst(pDst),
		  Skillname(pSkillname),
		  Id(pId),
		  Revision(pRevision) {
	}
};
