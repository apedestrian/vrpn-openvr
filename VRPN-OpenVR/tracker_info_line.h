#pragma once

#include <string>
#include <openvr.h>
#include "windows.h"

class tracker_info_line
{
public:
	tracker_info_line(const std::string& name);

	void update_status_and_print(vr::ETrackingResult new_result);

private:
	static HANDLE console_handle;

	const std::string tracker_name;
	size_t last_char_count;
	COORD line_coord;
	vr::ETrackingResult last_result;

	COORD get_console_cursor_position();
};

