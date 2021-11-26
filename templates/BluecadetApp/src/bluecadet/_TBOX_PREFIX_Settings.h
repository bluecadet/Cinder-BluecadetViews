#pragma once

#include "bluecadet/core/SettingsManager.h"

namespace bluecadet {

typedef std::shared_ptr<class _TBOX_PREFIX_Settings> _TBOX_PREFIX_SettingsRef;

class _TBOX_PREFIX_Settings : public bluecadet::core::SettingsManager {

public:
	static _TBOX_PREFIX_SettingsRef get() {
		static auto instance = std::make_shared<_TBOX_PREFIX_Settings>();
		return instance;
	}

	void mapFields() override;

protected:
};

} // namespace bluecadet