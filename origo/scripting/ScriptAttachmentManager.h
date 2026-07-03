#pragma once

#include <cstddef>
#include <unordered_map>
#include <vector>

#include "origo/core/RID.h"
#include "origo/serialization/ISerializer.h"
#include "origo/scripting/ReflectedFieldStorage.h"
#include "origo/scripting/ScriptDescriptor.h"

namespace Origo {

struct ScriptAttachmentData {
	ScriptTypeID ID { UUID::bad() };
	ReflectedFieldStorage Fields {};
};

class ScriptAttachmentManager {
public:
	ScriptAttachmentManager();
	ScriptAttachmentManager(const ScriptAttachmentManager& other);
	ScriptAttachmentManager& operator=(const ScriptAttachmentManager& other);
	~ScriptAttachmentManager();

	void add(RID entity, ScriptTypeID type);
	ScriptAttachmentData* get(RID entity, ScriptTypeID type);
	const ScriptAttachmentData* get(RID entity, ScriptTypeID type) const;
	bool has(RID entity, ScriptTypeID type) const;
	void copy_scripts(RID source, RID target);
	void serialize_entity(RID entity, ISerializer& backend) const;
	void deserialize_entity(RID entity, ISerializer& backend);
	void remove(RID entity, ScriptTypeID type);
	bool remove_if_exists(RID entity, ScriptTypeID type);
	void remove_all_scripts(RID entity);
	const std::unordered_map<RID, std::vector<ScriptAttachmentData>>& get_all() const { return m_data; }
	void migrate_script(RID entity, ScriptTypeID type);

private:
	void subscribe_to_registry_updates();
	void unsubscribe_from_registry_updates();

	std::unordered_map<RID, std::vector<ScriptAttachmentData>> m_data {};
	std::size_t m_script_updated_listener {};
	std::size_t m_script_removed_listener {};
	bool m_has_script_updated_listener {};
	bool m_has_script_removed_listener {};
};

}
