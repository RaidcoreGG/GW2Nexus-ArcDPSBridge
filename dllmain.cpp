#include "EventHandler.h"

#include <ArcdpsExtension/arcdps_structs.h>
#include <ArcdpsExtension/ArcdpsExtension.h>
#include <ArcdpsExtension/UpdateCheckerBase.h>
#include <Nexus/Nexus.h>

#include <Windows.h>

// It is guaranteed that arcdps loads first and Nexus second.
// except when users do weird shit!

namespace {
	HINSTANCE selfHandle;
	arcdps_exports arcExports{};
	AddonDefinition NexusDef{};
	AddonAPI* nexusApi = nullptr;
} // namespace

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
			selfHandle = hinstDLL;
			break;

		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE; // Successful DLL_PROCESS_ATTACH.
}

void mod_combat_squad(cbtevent* pEvent, ag* pSrc, ag* pDst, const char* pSkillname, uint64_t pId, uint64_t pRevision) {
	SquadEventHandler::instance().Event(pEvent, pSrc, pDst, pSkillname, pId, pRevision);
}

void mod_combat_local(cbtevent* pEvent, ag* pSrc, ag* pDst, const char* pSkillname, uint64_t pId, uint64_t pRevision) {
	LocalEventHandler::instance().Event(pEvent, pSrc, pDst, pSkillname, pId, pRevision);
}

arcdps_exports* mod_init() {
	// if this gets called before Nexus init, we don't want to be loaded!
	// Don't load and give arcdps an error message.
	std::string error_message;
	bool error = false;

	if (nexusApi == nullptr) {
		error = true;
		error_message = "Arcdps was loaded first, we do not support this loading order";
	}

	arcExports.out_name = "Nexus ArcDPS Bridge";
	// hardcoded imgui version, since we don't use it!
	arcExports.imguivers = 18000;
	auto res = ArcdpsExtension::UpdateCheckerBase::GetCurrentVersion(selfHandle);
	if (res) {
		auto version = ArcdpsExtension::UpdateCheckerBase::GetVersionAsString(res.value());
		char* version_c_str = new char[version.length() + 1];
		strcpy_s(version_c_str, version.length() + 1, version.c_str());
		arcExports.out_build = version_c_str;
	} else {
		error = true;
		error_message = "Error loading version";
	}

	if (error) {
		// we are in an error state, tell arcdps about it
		arcExports.sig = 0;
		const std::string::size_type size = error_message.size();
		char* buffer = new char[size + 1]; // we need extra char for NUL
		memcpy(buffer, error_message.c_str(), size + 1);
		arcExports.size = reinterpret_cast<uintptr_t>(buffer);
	} else {
		arcExports.sig = 0xFED81763; // -19392669
		arcExports.size = sizeof(arcdps_exports);
		arcExports.combat = mod_combat_squad;
		arcExports.combat_local = mod_combat_local;
	}

	return &arcExports;
}

extern "C" __declspec(dllexport) ModInitSignature get_init_addr(const char* arcversion, ImGuiContext* imguictx, void* dxptr, HMODULE arcdll, MallocSignature mallocfn, FreeSignature freefn, UINT dxver) {
	// we are not interested in anything arcdps gives us here
	return mod_init;
}

uint64_t mod_release() {
	ArcdpsExtension::Shutdown();

	return 0;
}

extern "C" __declspec(dllexport) ModReleaseSignature get_release_addr() {
	return mod_release;
}

void AddonLoad(AddonAPI* pApi) {
	// this has to be called before arcpds init!

	nexusApi = pApi;

	SquadEventHandler::instance().SetNexusApi(pApi);
	LocalEventHandler::instance().SetNexusApi(pApi);
}

extern "C" __declspec(dllexport) AddonDefinition* GetAddonDef() {
	NexusDef.Signature = -0x127E89D; // 0xFED81763 // -19392669
	NexusDef.APIVersion = NEXUS_API_VERSION;
	NexusDef.Name = "Nexus ArcDPS Bridge";

	auto res = ArcdpsExtension::UpdateCheckerBase::GetCurrentVersion(selfHandle);
	if (res) {
		auto version = res.value();
		NexusDef.Version.Major = version[0];
		NexusDef.Version.Minor = version[1];
		NexusDef.Version.Build = version[2];
		NexusDef.Version.Revision = version[3];
	} else {
		// we don't know our version if the above failed :(
		NexusDef.Version.Major = 0;
		NexusDef.Version.Minor = 0;
		NexusDef.Version.Build = 0;
		NexusDef.Version.Revision = 0;
	}

	NexusDef.Author = "Raidcore";
	NexusDef.Description = "Raises Nexus Events from ArcDPS combat API callbacks.";
	NexusDef.Load = AddonLoad;
	NexusDef.Flags = EAddonFlags_DisableHotloading;

	return &NexusDef;
}
