#pragma once

namespace OrigoEditor {
class EditorPanel {
public:
	virtual ~EditorPanel() = default;

	virtual const char* GetName() const = 0;
	virtual void OnImGuiRender() = 0;

	virtual bool IsOpen() const { return m_Open; }
	virtual bool& IsOpenRef() { return m_Open; }
	virtual void SetOpen(bool open) { m_Open = open; }

protected:
	bool m_Open = true;
};
}
