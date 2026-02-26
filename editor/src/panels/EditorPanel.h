#pragma once

namespace OrigoEditor {
class EditorPanel {
public:
	virtual ~EditorPanel() = default;

	virtual const char* get_name() const = 0;
	virtual void on_im_gui_render() = 0;

	virtual bool is_open() const { return m_open; }
	virtual bool& is_open_ref() { return m_open; }
	virtual void set_open(bool open) { m_open = open; }

	virtual bool is_collapsable() const { return false; }

protected:
	bool m_open = true;
};
}
